#include "../include/Bot.hpp"

void	Bot::cmd_hello(const BotMessage &msg)
{
	if(msg.command == "!hello")
	{
		if (!msg.params.empty())
		{
			sendRaw("PRIVMSG " + msg.target + BotUtils::toManyArguments(msg));
			return ;
		}
		sendRaw("PRIVMSG " + msg.target + " :Hello! I'm the Bot.");
	}
}

void	Bot::cmd_debug(const BotMessage &msg)
{
	(void)msg;
}

void	Bot::cmd_chat(const BotMessage &msg)
{
	(void)msg;
}