#ifndef BOT
# define BOT
# define COUNT_CMDS 3

# include <csignal>
# include <string>
# include <cerrno>
# include <cstdlib>
# include <netdb.h>
# include <cstring>
# include <iostream>
# include <unistd.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>

#include "../include/BotUtils.hpp"

class Bot
{
	private:
		int			_sock;
		std::string _nick;
		std::string _channel;

		typedef void (Bot::*cmdsFunc)(const BotMessage&);

		cmdsFunc	*_cmdFunc;
		std::string	*_cmds;
		std::string	_buffer;
		static	bool shutDown;

		std::string	*startCMDs( void );
		cmdsFunc	*startCMDsFunc( void );

		void	sendRaw(const std::string &msg);
		void	process_buffer(std::string &buffer);
		void	handle_line(const std::string &line);

	public:
		Bot(int port,
			const std::string &host,
			const std::string &pass,
			const std::string &nick,
			const std::string &channel);
		~Bot();

		void	cmd_chat(const BotMessage &msg);
		void	cmd_hello(const BotMessage &msg);
		void	cmd_debug(const BotMessage &msg);
		
		void	run( void );
		void	shutDownBot( void );
static	void	signalHandler(int signum);

};

#endif