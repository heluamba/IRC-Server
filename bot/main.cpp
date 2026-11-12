#include <cstdlib>

#include  "../include/Bot.hpp"
#include "../include/BotUtils.hpp"

int main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, Bot::signalHandler);
    signal(SIGQUIT, Bot::signalHandler);
	
	if (argc < 3 || argc > 5) {
		std::cout << "./bot <port> <pass> <nick> <channel>" << std::endl;
		return (1);
	}

	std::string nick;
	std::string channel;
	std::string port = argv[1];
	std::string pass = argv[2];
	std::string host = "127.0.0.1";

	if (!BotUtils::isValidNumber(port) && port.size() > 4) {
		std::cout << "No valid port!" << std::endl;
		return (1);
	}
	
	if (argc == 3) {
		nick = "bot";
		channel = "#bothome";
	}
	else {
		nick = argv[3];
		channel = argv[4];
	}


	try
	{
		Bot bot(std::atoi(argv[1]), host, pass, nick, channel);
		bot.run();
		return (0);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}