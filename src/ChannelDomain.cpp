#include <iostream>
#include <cstdlib>

#include "../include/utils.hpp"
#include "../include/ChannelDomain.hpp"

ChannelDomain::ChannelDomain() {}

ChannelDomain::~ChannelDomain() {}

Channel *ChannelDomain::getChannel(const std::string &channelName)
{
	try
	{
		std::string chanName = utils::toLower(channelName);
		Channel *chan = &channels.at(chanName);
		return chan;
	}
	catch (std::out_of_range &e)
	{
		return NULL;
	}
}

std::string ChannelDomain::getChannelTopic(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);

	if (!chan)
		return std::string("");
	return chan->getTopic();
}

std::string ChannelDomain::getChannelSymbol(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return std::string("");
		
	if (chan->isInviteOnly())
		return ("*");
	return ("=");
}

std::string ChannelDomain::getChannelMembersList(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return (std::string)"";

	std::string nameReply;
	std::vector<Client *> memberList = chan->getMembersList();
	for (size_t i = 0; i < memberList.size(); i++)
	{
		std::string nick = memberList[i]->getNickname();
		if (chan->isOperatorMember(nick))
			nameReply += "@" + nick + " ";
		else
			nameReply +=  nick + " ";
	}
	return (nameReply);
}

std::string ChannelDomain::extractChannelTopic(const std::string &channleName)
{
	Channel *chan = getChannel(channleName);
	if (!chan)
		return ("");
	return (chan->getTopic());
}

bool	ChannelDomain::isChannelExistent(const std::string &channelName)
{
	const Channel *chan = getChannel(channelName);
	
	if (!chan)
		return false;
	return true;
}


bool	ChannelDomain::isChannelEmpty(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);

	if (!chan)
		return (true);
	if (chan->isEmptyChannel())
		return (true);
	return (false);
}

bool	ChannelDomain::isChannelMember(Client *client, const std::string &channelName)
{
	if (!client)
		return false;
	Channel *chan = getChannel(channelName);
	if (!chan)
		return false;
	if (chan->isMemberByName(client->getNickname()))
		return true;
	return false;
}

bool	ChannelDomain::isOperatorMember(Client *client, const std::string &channelName)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return false;
	if (chan->isOperatorMember(client->getNickname()))
		return true;
	return false;
}

bool	ChannelDomain::isChannelInviteOnly(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);

	if (!chan)
		return false;
	if (chan->isInviteOnly())
		return true;
	return false;
}

bool	ChannelDomain::isChannelTopicModeOn(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return false;
	if (!chan->isTopicModeOn())
		return false;
	return true;
}

void	ChannelDomain::changeChannelTopic(std::string &channelName, const std::string &topic)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return ;
	chan->setTopic(topic);
}

void 	ChannelDomain::createChannel(Client *cli, std::string channelName, std::string key)
{
	if (channelName.empty() || !cli)
		return ;
	
	channelName = utils::toLower(channelName);
	if (key.empty())
		channels.insert(std::make_pair(channelName, Channel(channelName, cli)));
	else
		channels.insert(std::make_pair(channelName, Channel(channelName, cli, key)));
}


void	ChannelDomain::addToChannelInvitedList(std::string &channelName, std::string &nick)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return ;
	if (chan->isInviteOnly())
		chan->addToInviteOnlyList(nick);
}

void	ChannelDomain::deleteChannel(const std::string channelName)
{
	if (channelName.empty())
		return ;

	channels.erase(utils::toLower(channelName));
}

void ChannelDomain::removeClientFromChannels(Client *client)
{
	if (!client)
		return;

	std::vector<std::string> channelsNames = client->getMemberShip();
	for (size_t i = 0; i < channelsNames.size(); ++i)
    {
		Channel *chan = getChannel(channelsNames[i]);
		if (!chan)
			continue;

		chan->removeMemberByFd(client->getFd());
		if (chan->isEmptyChannel())
			deleteChannel(chan->getName());
    }
}

void	ChannelDomain::removeMemberOfChannel(std::string &memberName, std::string &channelName)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return ;

	chan->removeMemberByName(memberName);
}

void	ChannelDomain::notifyMembersOfChannel(const std::string &channelName, t_notify &notify)
{
	Channel *chan = getChannel(channelName);
	if (!chan)
		return;

	const std::vector<Client*> &members = chan->getMembersList();
	for (size_t i = 0; i < members.size(); ++i)
	{
		int fd = members[i]->getFd();
		if (notify.exceptFd != NONE)
			if (fd == notify.exceptFd)
				continue;
		if (notify.alreadyNotified.count(fd))
			continue;
		members[i]->queueMessage(notify.message);
		notify.alreadyNotified.insert(fd);
	}
}

void	ChannelDomain::notifyChannelsClient(Client *client, const std::string &message)
{
	if (!client)
		return;
	const std::vector<std::string> &memberShip = client->getMemberShip();
	if (memberShip.empty())
		return;

	t_notify notify;
	notify.exceptFd = client->getFd();
	notify.message  = message;

	for (size_t i = 0; i < memberShip.size(); ++i)
    {
		std::map<std::string, Channel>::iterator it = channels.find(memberShip[i]);
		if (it == channels.end())
			continue;
		notifyMembersOfChannel(it->first, notify);
    }
}

std::string ChannelDomain::getChannelModeStatus(const std::string &channelName)
{
	Channel *chan = getChannel(channelName);

	if (!chan)
		return ("");

	std::string mode_status = "+";
	std::string params;

	if (chan->isInviteOnly())
		mode_status += 'i';
	if (chan->isTopicModeOn())
		mode_status += 't';
	if (chan->isKeyModeOn()) {
		mode_status += 'k';
		params +=  chan->channelKey + " ";
	}
	if (chan->isLimitModeOn()) {	
		mode_status += 'l';
		params += chan->limitMembers;
	}
	if (!params.empty())
		mode_status += " " + params;
	return (mode_status);
}

size_t	ChannelDomain::tryJoin(Client *client, const std::string &channelName, const std::string &key)
{
	if (!client)
		return (ERR_UNDEF);

	Channel *chan = getChannel(channelName);
	if (!chan)
		return (ERR_UNDEF);
	std::string nick = client->getNickname();
	if (chan->isMemberByName(nick))
		return (ERR_USERONCHANNEL);
	if (chan->isInviteOnly())
		if (!chan->isOnInviteOnlyList(nick))
			return (ERR_INVITEONLYCHAN);
	if (chan->isKeyModeOn())
		if (!chan->hasChannelKey(key))
			return (ERR_BADCHANNELKEY);
	if (chan->isFullChannel())
		return (ERR_CHANNELISFULL);

	chan->addMember(client);
	return (SUCESS);
}

size_t	ChannelDomain::limitMode(Channel &chan, bool activate, const std::string limit)
{
	int nu = std::atoi(limit.c_str());

	if (nu < 0)
		return (ERR_UNDEF);
	if (activate)
	{
		if (limit.empty())
			return (ERR_UNDEF);
		chan.setLimitMembers(nu);
		return (SUCESS);
	}
	chan.setLimitMembers(NO_LIMIT);
	return (SUCESS);
}

size_t	ChannelDomain::keyMode(Channel &chan, bool activate, const std::string key)
{
	if (key.empty())
		return (ERR_UNDEF);
	if (activate)
	{
		if (chan.isKeyModeOn())  
			return (ERR_KEYSET);
		chan.setPass(key);
		return SUCESS;
	}
	if (!chan.hasChannelKey(key))
		return (ERR_BADCHANNELKEY);
	chan.setPass("");
	return (SUCESS);
}

size_t		ChannelDomain::operatorMode(Channel &chan, bool activate, const std::string nick)
{
	if (nick.empty())
		return (ERR_UNDEF);
	if (!chan.isMemberByName(nick))
		return (ERR_USERNOTINCHANNEL);
	if (activate) {
		chan.addOperator(nick);
		return (SUCESS);
	}
	chan.removeOperator(nick);
	return (SUCESS);
}

size_t ChannelDomain::applyMode(Client *client, std::string &channelName,
								const char &mode, const std::string &params, bool activate)
{
	Channel *chan = getChannel(channelName);

	if (!client || !chan)
		return (ERR_UNDEF);
	if (mode == 'i')
	{
		chan->setInviteOnly(activate);
		return (SUCESS);
	}
	if (mode == 't')
	{
		(chan->setTopicMode(activate));
		return (SUCESS);
	}
	if (mode == 'k')
		return (keyMode(*chan, activate, params));
	if (mode == 'l')
		return (limitMode(*chan, activate, params));
	if (mode == 'o')
		return (operatorMode(*chan, activate, params));
	return (ERR_UNKNOWNMODE);
}