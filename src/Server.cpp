
#include "../include/Server.hpp"
#include "../include/IRCMessage.hpp"
#include "../include/NumericReplies.hpp"
#include "../include/Parser.hpp"

// Redes, sistema
#include <arpa/inet.h> // inet_ntoa(), conversões de endereços IP
#include <cstring>
#include <fcntl.h>      // fcntl(), O_NONBLOCK
#include <netinet/in.h> // struct sockaddr_in, htons(), INADDR_ANY
#include <sys/socket.h> // socket(), bind(), listen(), accept()

#include "../include/Server.hpp"
#include <cerrno>
#include <iostream>
#include <unistd.h>

bool Server::shutdown = false;

void Server::signalHandler(int signum) {
  (void)signum;
  std::cout << "Signal " << signum << " received. Shutting down the server..."
            << std::endl;
  Server::shutdown = true;
}

Server::Server(int port, const std::string pass) : server_fd(-1), serv_name("IRC"),
									passWord(pass), handleMessage(NULL)
{
  handleMessage = new handle_message(this);

  // Criação do socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
    throw std::runtime_error("Failed to create socket");

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  fcntl(server_fd, F_SETFL, O_NONBLOCK); // Define o socket como não bloqueante

  struct sockaddr_in server_addr;
  std::memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  // Bind do socket
  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    throw std::runtime_error("Failed to bind socket");

  // Listen no socket
  if (listen(server_fd, SOMAXCONN) < 0)
    throw std::runtime_error("Failed to listen on socket");

  // Adiciona o socket do servidor à lista de poll_fds
  struct pollfd pfd;
  pfd.fd = server_fd;
  pfd.events = POLLIN;
  poll_fds.push_back(pfd);

  std::cout << GRE << "Server started on port " << port << WHI
            << " (press Ctrl+C to stop)" << std::endl;
}

Server::~Server()
{

  delete handleMessage;

  for (size_t i = 0; i < poll_fds.size(); ++i) {
    close(poll_fds[i].fd);
  }
}

ChannelDomain &Server::getChannelDomain() { return chanDomain; }
const std::string &Server::getPass() const { return passWord; }

void Server::run()
{
    while (!shutdown)
    {
        int ret = poll(poll_fds.data(), poll_fds.size(), -1);
        if (ret < 0) {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("Falha no poll");
        }

        for (int i = static_cast<int>(poll_fds.size()) - 1; i >= 0; --i)
        {
            short rev = poll_fds[i].revents;

            if (rev == 0)
                continue;

            int fd = poll_fds[i].fd;

            if (rev & (POLLERR | POLLHUP | POLLNVAL))
            {
                if (fd != server_fd)
                    removeClient(fd);
                continue;
            }

            if (rev & POLLIN)
            {
                if (fd == server_fd)
                    acceptNewClient();
                else
                    handleClientData(fd);
            }
            if (rev & POLLOUT)
                handleClientWrite(fd);
        }
    }
}

void Server::acceptNewClient() {
  struct sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);

  int clientFd = accept(server_fd, (struct sockaddr *)&clientAddr, &clientLen);
  if (clientFd < 0)
    return;

  fcntl(clientFd, F_SETFL, O_NONBLOCK);

  std::string ip = inet_ntoa(clientAddr.sin_addr);

  struct pollfd clientPollFd;
  clientPollFd.fd = clientFd;
  clientPollFd.events = POLLIN;
  clientPollFd.revents = 0;
  poll_fds.push_back(clientPollFd);

  clients.insert(std::make_pair(clientFd, Client(clientFd, ip, this)));

  std::cout << "Novo cliente ligado: fd " << clientFd << " (" << ip << ")"
            << std::endl;
}

void Server::handleClientData(int fd)
{
	char buffer[512];
	int bytesRead = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesRead == 0)
	{
		removeClient(fd);
		return;
	}

	if (bytesRead < 0)
		return;

	Client *client = findClient(fd);
	if (!client)
		return ;
	std::string &clientBuffer = client->getBuffer();
	clientBuffer.append(buffer, bytesRead);

	size_t pos;
	while ((pos = clientBuffer.find("\r\n")) != std::string::npos)
	{
		std::string line = clientBuffer.substr(0, pos);
		clientBuffer.erase(0, pos + 2);

		if (handleMessage != NULL)
			handleMessage->processLine(fd, line);
	}
}

void Server::handleClientWrite(int fd)
{
	Client *client = findClient(fd);
	if (!client)
		return ;

	std::string &outBuffer = client->getOutBuffer(); 
	if (outBuffer.empty())
	{
		updateServerPollEvents(fd, POLLOUT, false);
		return ;
	}

	int sendBytes = send(fd, outBuffer.c_str(), outBuffer.size(), 0);
	if (sendBytes > 0)
	{
		client->consumeOutputBuffer(sendBytes);

		if (client->isOutputBufferEmpty())
			updateServerPollEvents(fd, POLLOUT, false);
	}
	else if (sendBytes == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
		if (errno == EPIPE || errno == ECONNRESET)
			removeClient(fd);
	}
}

void Server::removeClient(int fd)
{
	handleMessage->removeClientFromServer(findClient(fd));
	close(fd);
	clients.erase(fd);

	for (size_t i = 0; i < poll_fds.size(); i++) {
		if (poll_fds[i].fd == fd) {
			poll_fds.erase(poll_fds.begin() + i);
			break;
		}
	}

	std::cout << RED << "Cliente desligado: fd" << fd << std::endl;
}

void Server::updateServerPollEvents(int fd, short event, bool add)
{
	if (fd < 0)
		return ;
	for (size_t i = 0; i < poll_fds.size(); i++)
	{
		if (fd == poll_fds[i].fd)
		{
			if (add)
				poll_fds[i].events |= event;
			else
				poll_fds[i].events &= ~event;
			return ;
		}
	}
}

Client *Server::findClient(const int &fd)
{
	try
	{
		Client *client = &clients.at(fd);
		return client;
	}
	catch (std::out_of_range &e)
	{
		std::cerr << "Client Not Found" << std::endl;
		return NULL;
	}
}