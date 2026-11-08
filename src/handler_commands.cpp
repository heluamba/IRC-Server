
#include "../include/NumericReplies.hpp"
#include "../include/message.hpp"
#include "../include/Server.hpp"
#include "../include/Parser.hpp"
#include "../include/utils.hpp"
#include <iostream>
#include <sstream>
#include <vector>

void handle_message::handleNick(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;
	if (message.params.size() <= 0 || message.params[0].empty())
	{
		client->queueMessage(NumericReplies::noNicknameGiven(*client, server->serv_name));
		return ;
	}
	std::string nick = message.params[0];
	if (!utils::isValidNickName(nick))
	{
		client->queueMessage(NumericReplies::erroNeusNickNmane(*client, server->serv_name, nick));
		return ;
	}
	if (!server->getPass().empty())
		if (!client->isPasswordOk())
		{
			client->queueMessage("Pass Required First");
			return ;
		}
	if (nicknameInUse(nick, client))
	{
		client->queueMessage(NumericReplies::nicknameInUse(*client, server->serv_name, nick));
		return ;
	}
	std::string oldPrefix = clientPrefix(client);
	client->setNickname(nick);
	client->setPrefix(clientPrefix(client));
	broadcastNickChange(client, oldPrefix, nick);
	tryRegiste(client);
}

void handle_message::handleUser(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;

	size_t size = message.params.size();
	if (size < 2 || size > 4)
	{
		client->queueMessage(NumericReplies::needMoreParams(*client, message.command, server->serv_name));
		return ;
	}
	if (!server->getPass().empty())
		if (!client->isPasswordOk())
		{
			client->queueMessage("Pass Required First");
			return ;
		}
	if (!client->getUsername().empty() && !client->getRealname().empty())
	{
		client->queueMessage(NumericReplies::alreadyRegistered(*client, server->serv_name));
		return ;
	}
	client->setUsername(message.params[0]);
	client->setRealname(message.params[size - 1]);
	client->setPrefix(clientPrefix(client));
	tryRegiste(client);
}

void handle_message::handlePass(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;

	size_t	size = message.params.size();
	if (client->isRegistered())
	{
		client->queueMessage (NumericReplies::alreadyRegistered(*client, server->serv_name));
		return ;
	}
	if (size < 1)
	{
		client->queueMessage(NumericReplies::needMoreParams(*client, server->serv_name, message.command));
		return ;
	}
	if (server->getPass().empty())
		return ;
	if (message.params[0] == server->getPass())
	{
		client->setPasswordOk(true);
		return ;
	}
	client->queueMessage(NumericReplies::passMismatch(*client, server->serv_name));
}

void	handle_message::handle_part(Client *client, const IRCMessage &msg)
{
	if (!client)
		return ;

	if (msg.params.empty()) {
		client->queueMessage(NumericReplies::needMoreParams(*client,
															server->serv_name,
															msg.command));
		return ;
	}

	std::string reason;
	std::vector<std::string> channelsNames = utils::split(msg.params[0], ',');
	ChannelDomain &chaDomain = server->getChannelDomain();
	if (msg.params.size() > 1)
		reason = msg.params[1];
	
	for (size_t i = 0;  i < channelsNames.size(); i++)
	{
		if (!chaDomain.isChannelExistent(channelsNames[i])) {
			client->queueMessage(NumericReplies::noSuchChannel(*client,
																server->serv_name,
																channelsNames[i]));
			continue;
		}
		if (!chaDomain.isChannelMember(client, channelsNames[i])) {
			client->queueMessage(NumericReplies::notOnChannel(*client,
															  server->serv_name,
															  channelsNames[i]));
			continue;
		}
		
		broadcastPart(client, channelsNames[i], reason);
		std::string nick = client->getNickname();
		chaDomain.removeMemberOfChannel(nick, channelsNames[i]);

		if (chaDomain.isChannelEmpty(channelsNames[i]))
			chaDomain.deleteChannel(channelsNames[i]);
	}
}

void	handle_message::handle_join(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;
	
	ChannelDomain &chaDomain = server->getChannelDomain();
	if (message.params.size() == 1) {
		if (message.params[0] == "0") {
			//Remove & Notify client from all channels
			std::vector<std::string> channels = client->getMemberShip();
			for (size_t i = 0; i < channels.size(); i++)
				broadcastPart(client, channels[i], "");
			chaDomain.removeClientFromChannels(client);
			return ;
		}
	}

	channelsKeys chankeys;
	if (!isValidJoinParams(client, message, chankeys))
		return ;

	std::string key;
	std::string channelName;
	channelsKeys::iterator it;
	for (it = chankeys.begin(); it != chankeys.end(); it++)
	{
		size_t status = SUCESS;
		channelName = it->first;
		key = it->second;

		if (!utils::isValidChannelName(channelName)) {
			client->queueMessage(NumericReplies::badchanmask(*client, server->serv_name, channelName));
			return ;
		}

		if (!chaDomain.isChannelExistent(channelName))
			chaDomain.createChannel(client, channelName, key);
		else
			status = chaDomain.tryJoin(client, channelName, key);
		
		if (status == ERR_USERONCHANNEL)
			continue;
		if (status == SUCESS) {
			broadcastJoin(client, channelName);
			continue;
		}
		catJoinError(client, channelName, status);
	}
}

void handle_message::handleQuit(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;
	std::string reason;
	if (message.params.empty())
		reason = "Client Quit";
	 else
		reason = message.params[0];

	broadcastQuit(client, reason);
	server->getChannelDomain().removeClientFromChannels(client);
	server->removeClient(client->getFd());
}

void handle_message::handlePrivmsg(Client *client, const IRCMessage &message)
{
	if (!isValidPrivmsgParams(client, message))
		return ;

    std::vector<std::string> targets = utils::split(message.params[0], ',');
    std::string text   = message.params[1];
	size_t size = targets.size();

	if (size > TARGETS_SIZE) {
		client->queueMessage(NumericReplies::toManyTargets(*client, server->serv_name, " To many targets"));
		return ;
	}
	for (size_t i = 0; i < size; i++)
	{
		
		std::string content = client->getPrefix() + " PRIVMSG " + targets[i] + " :" + text;
		Client *receptor = findClientByNickName(targets[i]);
		if (receptor) {
			receptor->queueMessage(content);
			continue ;
		}
		if (utils::isValidChannelName(targets[i])) {
			ChannelDomain &chanDomain = server->getChannelDomain();
			if (chanDomain.isChannelExistent(targets[i]))
			{
				if (!chanDomain.isChannelMember(client, targets[i])) {
					client->queueMessage(NumericReplies::canNotSendToChan(*client, server->serv_name, targets[i]));
					continue ;
				}

				t_notify notify;
				notify.exceptFd = client->getFd();
				notify.message = content;
				chanDomain.notifyMembersOfChannel(targets[i], notify);
				return ;
			}
		}
		client->queueMessage(NumericReplies::noSuchNick(*client,
														server->serv_name,
														targets[i]));
	}
}

void	handle_message::handle_kick(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;
	size_t size = message.params.size();
	if (size < 2) {
		client->queueMessage(NumericReplies::needMoreParams(*client, server->serv_name, message.command));
		return;
	}

	std::vector<std::string> channels = utils::split(message.params[0], ',');
	std::vector<std::string> targets = utils::split(message.params[1], ',');
	std::string reason;
	if (size > 2)
		reason =  message.params[size - 1];

	size_t size_chan = channels.size();
	size_t size_user = targets.size();
	if (size_chan > size_user) {
		client->queueMessage(NumericReplies::needMoreParams(*client, server->serv_name, message.command));
		return;
	}
	size_t j = 0;
	size_t i = 0;
	ChannelDomain &chanDomain = server->getChannelDomain();
	while ((i < size_chan) && (j < size_user))
	{
		if (isValidKickParams(client, channels[i], targets[j]))
		{
			broadcastKick(client, channels[i], targets[i], reason);
			chanDomain.removeMemberOfChannel(targets[j], channels[i]);
		}
		if (i + 1 < size_chan)
			i++;
		j++;
	}
}

void	handle_message::handle_invite(Client *client, const IRCMessage &message)
{
	ChannelDomain &chanDomain = server->getChannelDomain();

	if (!client)
		return ;
	if (message.params.size() < 2) {
		client->queueMessage(NumericReplies::needMoreParams(*client, server->serv_name, message.command));
		return ;
	}
	std::string nick = message.params[0];
	std::string channel = message.params[1];
	if (!chanDomain.isChannelExistent(channel)) {
			client->queueMessage(NumericReplies::noSuchChannel(*client, server->serv_name, channel));
			return ;
	}
	if (!chanDomain.isChannelMember(client, channel)) {
		client->queueMessage(NumericReplies::notOnChannel(*client, server->serv_name, channel));
		return ;
	}
	if (chanDomain.isChannelInviteOnly(channel)) {
		if (!chanDomain.isOperatorMember(client, channel)) {
			client->queueMessage(NumericReplies::chaNoPrivsNeed(*client, server->serv_name, channel));
			return ;
		}
	}
	Client *target = findClientByNickName(nick);
	if (!target) {
		client->queueMessage(NumericReplies::noSuchNick(*client, server->serv_name, nick));
		return;
	}
	if (chanDomain.isChannelMember(target, channel)) {
		client->queueMessage(NumericReplies::userOnChannel(*target, server->serv_name, channel));
		return ;
	}
	chanDomain.addToChannelInvitedList(channel, nick);
	broadcastInvite(client, target, channel);
}


void	handle_message::handle_topic(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;
	if (message.params.empty()) {
		client->queueMessage(NumericReplies::needMoreParams(*client, server->serv_name, message.command));
		return ;
	}

	ChannelDomain &chanDomain = server->getChannelDomain();
	std::string channel = message.params[0];
	std::string newTopic;
	if (message.params.size() > 1)
		newTopic = message.params[1];
	if (!chanDomain.isChannelExistent(channel)) {
		client->queueMessage(NumericReplies::noSuchChannel(*client, server->serv_name, channel));
		return;
	}
	if (!chanDomain.isChannelMember(client, channel)) {
		client->queueMessage(NumericReplies::notOnChannel(*client, server->serv_name, channel));
		return ;
	}
	if (newTopic.empty())
	{
		 std::string chanTopic = chanDomain.getChannelTopic(channel);
		 if (chanTopic.empty())
		 	client->queueMessage(NumericReplies::rpl_noTopic(*client, server->serv_name, channel));
		else
			client->queueMessage(NumericReplies::rpl_topic(*client, server->serv_name, channel, chanTopic));
		return ;
	}
	if (chanDomain.isChannelTopicModeOn(channel)) {
		if (!chanDomain.isOperatorMember(client, channel)) {
			client->queueMessage(NumericReplies::chaNoPrivsNeed(*client, server->serv_name, channel));
			return ;
		}
	}
	chanDomain.changeChannelTopic(channel, newTopic);
	broadcastTopic(client, channel, newTopic);
}

void	handle_message::handle_mode(Client *client, const IRCMessage &message)
{
	if (!client || !isValidModeParams(client, message))
		return ;

	std::string modes = message.params[1];
	std::string channel = message.params[0];
	std::vector<std::string> params = message.params;
	
	ChannelDomain &chanDomain = server->getChannelDomain();

	bool		activate = true;
	size_t			j = 2;

	char markSig = '\0';
	std::string appliedModes;
	std::string appliedParams;
	
	for (size_t i = 0; i < modes.size(); i++)
	{
		if (modes[i] == '-' || modes[i] == '+') {
			activate = (modes[i] == '+');
			continue;
		}

		bool needsArg = (modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o');
		std::string arg;

		if (needsArg)
		{
			if ( j < params.size())
				arg = params[j++];
		}

		size_t status = chanDomain.applyMode(client, channel, modes[i], arg, activate);
		if (status == SUCESS)
		{
			if (appliedModes.empty() || markSig != (activate ? '+' : '-')) {
				markSig = (activate ? '+' : '-');
				appliedModes += markSig;
			}
			appliedModes += modes[i];
			if (needsArg && !arg.empty())
				appliedParams += arg + " ";
		}
		else
			catModeError(client, channel, status, modes[i]);
	}
	if (!appliedModes.empty())
		broadcastMode(client, channel, appliedModes, appliedParams);
}

void	handle_message::handle_ping(Client *client, const IRCMessage &message)
{
	if (!client)
		return ;

	std::string token;
	if (!message.params.empty())
		token = message.params[0];
	std::string reply;

	if (token.empty())
		reply = ":" + server->serv_name + " PONG " + server->serv_name;
	else
		reply = ":" + server->serv_name + " PONG " + server->serv_name + " :" + token;
	client->queueMessage(reply); 
}

void	handle_message::handle_cap(Client *client, const IRCMessage &message)
{
	if (!client || message.params.empty())
		return ;

	if (message.params[0] == "LS") {
		std::string list = ":IRC CAP * LS :";
		std::cout << list << std::endl;
		client->queueMessage(list);
	}
}