#ifndef SERVER_HPP
# define SERVER_HPP

# include "Debug.hpp"
# include "Client.hpp"
# include "message.hpp"
# include "IRCMessage.hpp"
# include "ChannelDomain.hpp"

# include <map>
# include <poll.h>
# include <vector>

# define RED "\e[1;31m"
# define WHI "\e[0;37m"
# define GRE "\e[1;32m"

class	handle_message;

class Server
{
	friend class Debug;
	friend class Channel;
	friend class handle_message;

	private:
  		int server_fd;
		std::string serv_name;
		const std::string passWord;

		static bool shutdown;
		bool		toRemoveClient;
  		handle_message *handleMessage;
  		std::vector<struct pollfd> poll_fds;
  		std::map<int, Client> clients;

		ChannelDomain	chanDomain;

  		void acceptNewClient();
  		void handleClientData(int fd);
		void handleClientWrite(int fd);
		void removeMarkedClients();
  		void removeClient(int fd);

		Client *findClient(const int &fd);

	public:
  	
		Server(int port, const std::string pass);
  		~Server();

  		static void signalHandler(int signum);

		ChannelDomain		&getChannelDomain();
		const std::string 	&getPass() const;
		
		void markToRemove(bool value);
		void updateServerPollEvents(int fd, short event, bool add);

  	void run();
};

#endif
