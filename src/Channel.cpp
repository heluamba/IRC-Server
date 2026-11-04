#include "../include/NumericReplies.hpp"
#include "../include/Channel.hpp"
#include "../include/utils.hpp"

#include <iostream>
#include <sstream>

#include <map>

Channel::Channel(std::string ChannelName, Client *client):
name(ChannelName), allow_modes("itklo"),
channelKey(""), topicMode(false), limitMembers(NO_LIMIT), inviteOnlyMode(false)
{ addMember(client); addOperator(client->getNickname()); }

Channel::Channel(std::string ChannelName, Client *client, std::string key):
name(ChannelName), allow_modes("itkl"),
channelKey(key), topicMode(false), limitMembers(NO_LIMIT), inviteOnlyMode(false)
{ addMember(client); addOperator(client->getNickname()); addMode('k'); }


Channel::~Channel() {};

//------------- Get & Set ----------------//

std::string	Channel::getName() const { return name; }
std::string	Channel::getTopic() const { return topic; }
std::vector<Client *>	Channel::getMembersList() const { return membersList; }

void Channel::setTopic(const std::string t) { topic = t; }
void Channel::setTopicMode(bool value) {topicMode = value; }
void Channel::setLimitMembers(int limit) { limitMembers = limit; }
void Channel::setInviteOnly(bool value) { inviteOnlyMode = value; }
void Channel::setName(const std::string &channelName) { name = channelName; }
void Channel::setPass(const std::string &key) { channelKey = key; }



//--------------- CHECKERS ------------//

bool Channel::isAllowMode(const char mode) const
{
	for (size_t i = 0; i < allow_modes.size(); i++)
	{
		if (mode == allow_modes[i])
			return true;
	}
	return false;
}

bool Channel::isTopicModeOn() const { return topicMode; }
bool Channel::isInviteOnly( void ) const { return inviteOnlyMode; }
bool Channel::isKeyModeOn() const { return !(channelKey.empty()); }
bool Channel::isLimitModeOn() const { return !(limitMembers == NO_LIMIT); }

bool Channel::isEmptyChannel() const
{
	if (membersList.empty())
		return true;
	return false;
}

bool Channel::isFullChannel( void) const
{
	if (limitMembers == NO_LIMIT)
		return false;
	if (membersList.size() >= static_cast<size_t>(limitMembers))
		return true;
	return false;
}

bool Channel::isOperatorMember(const std::string &memberName)
{
	std::vector<Client *>::iterator it;

	std::string nick = utils::toLower(memberName);
	for (size_t i = 0; i < operators.size(); i++)
	{
		if (operators[i] == nick)
			return true;
	}
	return false;
}

bool	Channel::isMemberByName(const std::string &memberName)
{
	return (findMemberByName(memberName) != NULL);
}

bool	Channel::isMemberByFd(int fd) const
{
	return (findMemberByFd(fd) != NULL);
}

bool Channel::isOnInviteOnlyList(std::string memberName) const
{
	if (memberName.empty())
		return false;
	memberName = utils::toLower(memberName);
	for (size_t i = 0; i < inviteOnlyList.size(); i++)
	{
		if (inviteOnlyList[i] == memberName)
			return true;
	}
	return false;
}

bool Channel::hasMode(const char mode) const
{
	std::string::const_iterator it;

	if (modes.empty())
		return false;
	for (it = modes.begin(); it != modes.end(); it++)
	{
		if (*it == mode)
			return true;
	}
	return false;
}

bool Channel::hasChannelKey(const std::string &key) const
{
	if (key == channelKey)
		return true;
	return false;
}

//--------------- MEMBERS MANAGERS----------------- //

Client *Channel::findMemberByName(const std::string &memberName) const
{
	Client *member = NULL;

	if (memberName.empty())
		return(NULL);

	std::string nick = utils::toLower(memberName);
	for (size_t i = 0; i < membersList.size(); i++)
	{
		member = membersList[i];
		if (nick == utils::toLower(member->getNickname()))
			return (member);
	}
	return NULL;
}

Client *Channel::findMemberByFd(int fd) const
{
	Client *member;

	if (fd < 0)
		return NULL;
	for (size_t i = 0; i < membersList.size(); i++)
	{
		member = membersList[i];
		if (member->getFd() == fd)
			return (member);
	}
	return NULL;
}

//#--------- ADDs ------------

void Channel::addMember(Client *client)
{
	if (!client)
		return ;
	if (isMemberByName(client->getNickname()))
		return ;
	membersList.push_back(client);
	client->addToMemberShip(name);
}


void Channel::addMode(const char mode)
{
	if (isAllowMode(mode) && !hasMode(mode))
		modes += mode;
}

void Channel::addOperator(const std::string &memberName)
{
	if (isOperatorMember(memberName))
		return ;
	operators.push_back(memberName);
}

void Channel::addToInviteOnlyList(const std::string &memberName)
{
	if (memberName.empty())
		return ;
	std::string nick = utils::toLower(memberName);
	if (isOnInviteOnlyList(nick))
		return ;
	inviteOnlyList.push_back(nick);
}

//#------------ RMs --------------

void Channel::removeMemberByName(std::string &memberName)
{
	Client	*member;

	member = findMemberByName(memberName);
	if (!member)
		return ;

	std::vector<Client *>::iterator it = membersList.begin();	
	while (it != membersList.end())
	{
		if (*it == member)
		{
			removeOperator(memberName);
			member->removeOfMemberShip(name);
			removeOfInviteOnlyList(memberName);
			membersList.erase(it);
			return ;
		}
		it++;	
	}
}

void Channel::removeMemberByFd(int fd)
{
	Client	*member;

	member = findMemberByFd(fd);
	if (!member)
		return ;

	std::string nick = member->getNickname();
	std::vector<Client *>::iterator it = membersList.begin();
	while (it != membersList.end())
	{
		if (*it == member)
		{
			removeOperator(nick);
			member->removeOfMemberShip(name);
			removeOfInviteOnlyList(nick);
			membersList.erase(it);
			return ;
		}
		it++;
	}
}

void	Channel::removeAllMembers()
{
	for (size_t i = 0; i < membersList.size(); i++)
		removeMemberByFd(membersList[i]->getFd());
}


void Channel::removeMode(char mode)
{
	std::string::iterator it;

	if (modes.empty())
		return ;
    for (it = modes.begin();it != modes.end(); ++it)
    {
        if (*it == mode)
        {
            modes.erase(it);
            return;
        }
    }
}

void Channel::removeOperator(const std::string &memberName)
{
	std::vector<std::string>::iterator it;
	for (it = operators.begin(); it != operators.end(); it++)
	{
		if (*it == memberName)
		{
			operators.erase(it);
			return ;
		}
	}
}

void	Channel::removeOfInviteOnlyList(std::string &memberName)
{
	std::vector<std::string>::iterator it;

	if (memberName.empty())
		return ;
	memberName = utils::toLower(memberName);
	for (it = inviteOnlyList.begin(); it != inviteOnlyList.end(); it++)
	{
		if (*it == memberName)
		{
			inviteOnlyList.erase(it);
			return ;
		}
	}
}
