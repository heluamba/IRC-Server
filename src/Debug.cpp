#include <string>
#include <sstream>

#include "../include/Debug.hpp"
#include "../include/Client.hpp"
#include "../include/Server.hpp"


Debug::Debug(Server *server): server(server) {}

Debug::~Debug() {}

void	Debug::listClientsOnServer()
{
	const std::map<int, Client> &clients = server->clients;
	std::map<int, Client>::const_iterator it;

	std::cout << "CLIENTS ON SERVER" << std::endl;
	for (it = clients.begin(); it != clients.end(); it ++)
	{
		std::cout << "Nick name: " << it->second.getNickname() 
					<< " - FD: " << it->second.getFd() << std::endl;
	}
}
void Debug::listChannelsOnServer()
{
    ChannelDomain chanDomain = server->getChannelDomain();
    const std::map<std::string, Channel> chans = chanDomain.channels;

    std::cout << "CHANNELS ON SERVER" << std::endl;

    std::map<std::string, Channel>::const_iterator it;

    for (it = chans.begin(); it != chans.end(); ++it)
    {
        std::string name = it->first;
        const Channel& chan = it->second;

        std::cout << chanDomain.getChannelSymbol(name)
                  << name << std::endl;

		std::string message;
        std::cout << "-Topic: " << chan.getTopic() << std::endl;
		message = chan.isTopicModeOn() ? "Yes" : "No";
		std::cout << "-Topic Mode: " << message << std::endl;
        message = chan.isInviteOnly() ? "Yes" : "No";
        std::cout << "-Invite-only: " << message << std::endl;

        std::cout << "-Pass: "
                  << (chan.isKeyModeOn()
                      ? "Yes"
                      : "No") ;

        if (chan.isKeyModeOn())
            std::cout << " // " << chan.channelKey;

        std::cout << std::endl;

        std::cout << "-Max Members: ";

        if (chan.limitMembers == NO_LIMIT)
        {
            std::cout << "No limit";
        }
        else
        {
            std::ostringstream oss;
            oss << chan.limitMembers;
            std::cout << oss.str();
        }

        std::cout << "\n\n";
    }
}
void	Debug::listChannelMembers(std::string name)
{
	
	Channel *chan = server->getChannelDomain().getChannel(name);
	if (!chan)
	{
		std::cout << "Channel Not Exist! :" << name << std::endl;
		return ;
	}
	std::vector<Client *> members =  chan->getMembersList();
	std::string	member;
	std::vector<Client *>::const_iterator it;

	std::cout << "MEMBERS OF CHANNEL:" << chan->getName() << std::endl;
	for (size_t i = 0; i < members.size(); i++)
	{
		std::string nick = members[i]->getNickname();
		if (chan->isOperatorMember(nick))
			std::cout << "@" + nick << std::endl;
		else
			std::cout << nick << std::endl;
	}
	std::cout << std::endl;
}