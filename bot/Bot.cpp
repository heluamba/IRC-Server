#include <fcntl.h>
#include <poll.h>

#include "../include/Bot.hpp"

bool Bot::shutDown = false;

Bot::Bot(int port,
		const std::string &host,
		const std::string &pass,
		const std::string &nick,
		const std::string &channel): _nick(nick), _channel(channel)
{

	_cmds = startCMDs();
	_cmdFunc = startCMDsFunc();

	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock < 0)
    	throw std::runtime_error("Failed to create socket");
	
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(host.c_str());
	
	int ret = connect(_sock, (struct sockaddr*)&addr, sizeof(addr));
	if (ret < 0) {
		std::cerr << "Connection Failed: " << strerror(errno) << std::endl;
		close(_sock);
		exit(1);
	}
	
	fcntl(_sock, F_SETFL, O_NONBLOCK);
	sendRaw("PASS " + pass);
	sendRaw("NICK " + nick);
	sendRaw("USER BOT :B");
	sendRaw("JOIN " + channel);
	sendRaw("TOPIC " + channel + " The Channel Bot 🤖");
}

Bot::~Bot() {}

Bot::cmdsFunc *Bot::startCMDsFunc( void )
{
	static cmdsFunc cmds[] = 
	{
		&Bot::cmd_hello,
		&Bot::cmd_debug,
		&Bot::cmd_chat,
	};
	return (cmds);
}

std::string *Bot::startCMDs( void )
{
	static std::string cmds[] = 
	{
		"!hello",
		"!debug",
		"!chat"
	};
	return (cmds);
}


void	Bot::signalHandler(int signum)
{
	(void)signum;
	shutDown = true;
}

void	Bot::sendRaw(const std::string &msg)
{
	std::string line = msg + "\r\n";
	ssize_t n = send(_sock, line.c_str(), line.size(), 0);

	std::cerr << "[send " << n << " bytes] " << msg << std::endl;
	if (n < 0) {
		std::cerr << "Send Failed: " << strerror(errno) << std::endl;
	}
}

void	Bot::handle_line(const std::string &line)
{
	if (line.find("PING") == 0) {
		std::string token = line.substr(5);
		sendRaw("PONG " + token);
		return ;
	}

	if (line.find(" PRIVMSG ") != std::string::npos)
	{
		const BotMessage msg = BotUtils::parseBotMessage(line);

		for (size_t i = 0; i < COUNT_CMDS; i++)
		{
			if (msg.command == _cmds[i]) {
				(this->*_cmdFunc[i])(msg);
				return ;
			}
		}
	}
}

void	Bot::shutDownBot( void )
{
	if (!shutDown)
		return ;
	std::string cmd = "QUIT";
	sendRaw(cmd + " :Bot Bay Bay ..🤖");
	close(_sock);
	exit(0);
}

void	Bot::process_buffer(std::string &buffer)
{
	size_t pos;
	
	while ((pos = buffer.find("\r\n")) != std::string::npos)
	{
		std::string line = buffer.substr(0, pos);
		std::cerr << "[LINE] " << line << std::endl;
		buffer.erase(0, pos + 2);
		handle_line(line);
	}
}

void	Bot::run()
{
	char buf[4096];
	struct pollfd pfd;
	pfd.fd = _sock;
	pfd.events = POLLIN;
	pfd.revents = 0;

	while (!shutDown)
	{
		ssize_t ret = poll(&pfd, 1, 500);

		if (ret < 0) {
			if (errno == EINTR)
				continue;
			break;
		}

		if (ret == 0)
			continue;
		
		if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL))
			break;

		if (pfd.revents & POLLIN)
		{
			ssize_t n = recv(_sock, buf, sizeof(buf) - 1, 0);
			
			if (n <= 0)
				break;
			
			buf[n] = '\0';
			_buffer.append(buf, n);
			process_buffer(_buffer);
		}
	}
	shutDownBot();
}