#include "../include/NumericReplies.hpp"
#include "../include/message.hpp"
#include "../include/Server.hpp"
#include "../include/Parser.hpp"
#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <vector>

handle_message::HandleFunc* handle_message::startHandleCMDFunc( void )
{
	static HandleFunc functions[] = 
	{
		&handle_message::handleNick,
		&handle_message::handleUser,
		&handle_message::handlePass,
		&handle_message::handlePrivmsg,
		&handle_message::handle_join,
		&handle_message::handle_kick,
		&handle_message::handle_invite,
		&handle_message::handle_topic,
		&handle_message::handle_part,
		&handle_message::handle_mode,
		&handle_message::handle_ping,
		&handle_message::handle_cap,
		&handle_message::handleQuit,
		&handle_message::handle_debug,
	};
	return (functions);
}

std::string *handle_message::startCommandNames( void )
{
	static std::string commandNames[] = 
	{
		"NICK",
		"USER",
		"PASS",
		"PRIVMSG",
		"JOIN",
		"KICK",
		"INVITE",
		"TOPIC",
		"PART",
		"MODE",
		"PING",
		"CAP",
		"QUIT",
		"DB"
	};
	return (commandNames);
}

handle_message::handle_message(Server *server):
		server(server),
		handleCommands(startHandleCMDFunc()),
		commandNames(startCommandNames()) {}

handle_message::~handle_message() {}

void handle_message::handleLine(int fd, const std::string &line) {
	IRCMessage message = Parser::Parse(line);

	if (message.command.empty())
		return ;

	handleMessage(fd, message);
}

void handle_message::handleMessage(int fd, const IRCMessage &message)
{
	Client *client = server->findClient(fd);
	if (!client)
		return;

	static const std::string preRegCmds[] = {"PASS", "NICK", "USER", "CAP", "PING", "QUIT", "DB"};

	if (!client->isRegistered())
	{
		bool allowed = false;
		for (int i = 0; i < CMD_NUMBERS; i++)
		{
			if (message.command == preRegCmds[i]) {
				allowed = true;
				break ;
			}
		}
        if (!allowed) {
			client->queueMessage(NumericReplies::notRegistered(*client, server->serv_name));
			return ;
		}
    }

	bool	found = false;
    for (size_t i = 0; i < CMD_NUMBERS; i++)
	{
		if (message.command == commandNames[i])
		{
			(this->*handleCommands[i])(client, message);
			found = true;
			break;
		}
	}
	if (!found)
		client->queueMessage(NumericReplies::unknownCommand(*client,
															server->serv_name,
															message.command));
}

void	handle_message::processLine(int fd, const std::string &line)
{
	std::cout << "Linha recebida: " << line << std::endl;
	handleLine(fd, line);
}

void handle_message::tryRegiste(Client *client)
{
	if (!client)
		return ;
	if (client->isRegistered())
		return ;
	if (!server->getPass().empty() && !client->isPasswordOk())
	{

    	client->queueMessage(NumericReplies::passMismatch(*client, server->serv_name));
		return ;
	}
	if (client->getNickname().empty())
		return ;
	if (client->getUsername().empty())
		return ;
	client->setRegistered(true);
	client->queueMessage(NumericReplies::welcome(*client, server->serv_name));
}

bool handle_message::nicknameInUse(const std::string &nickname, Client *client) const
{
	if (nickname.empty() || !client)
		return (false);

	std::map<int, Client>::const_iterator it;
	int exceptFd = client->getFd();
	
	if (exceptFd < 0)
		return (false);

	std::string nick = utils::toLower(nickname);
	for (it = server->clients.begin(); it != server->clients.end(); it++)
	{
		if (it->first != exceptFd)
		{
			const Client *client = &it->second;
			if (!client)
				continue;
			if (nick == utils::toLower(client->getNickname()))
				return (true);
		}
	}
	return (false);
}

Client *handle_message::findClientByNickName(const std::string &nickname)
{
	if (nickname.empty())
		return(NULL);

	Client *client = NULL;
	std::map<int, Client>::iterator it;

	std::string nick = utils::toLower(nickname);
	for (it = server->clients.begin(); it != server->clients.end(); it++)
	{
		client = &it->second;
		if (nick == utils::toLower(client->getNickname()))
			return (client);
	}
	return NULL;
}

std::string handle_message::clientPrefix(const Client *client) const
{
  	return	":" + client->getNickname()
			+ "!" + client->getUsername()
			+ "@" + client->getIp();
}

void handle_message::removeClientFromServer(Client *client)
{
	if (!client)
		return ;
	
	broadcastQuit(client, "Client Quit");
	server->getChannelDomain().removeClientFromChannels(client);
}

//---------------- PARAMS VALIDATION -----------------//

bool	handle_message::isValidJoinParams(Client *client, const IRCMessage &message, channelsKeys &chankeys)
{
	size_t size = message.params.size();
	std::string keys;

	if (!client)
		return false;
	if (size < 1 || size > 2)
	{
		client->queueMessage(NumericReplies::needMoreParams(*client, server->serv_name, message.command));
		return false;
	}
	if (size == 2)
		keys = message.params[1];
	chankeys =  utils::extractJoinFields(message.params[0], keys);
	if (chankeys.empty())
	{
		client->queueMessage(NumericReplies::badchanmask(*client, server->serv_name, message.params[0]));
		return false;
	}
	return true;
}

bool	handle_message::isValidPrivmsgParams(Client *client, const IRCMessage &message)
{
	if (!client)
		return false;

	if (message.params.empty()) {
		client->queueMessage(NumericReplies::noRecipient(*client, server->serv_name));
		return false;
	}
	if (message.params.size() < 2 || message.params[1].empty()) {
		client->queueMessage(NumericReplies::noTextToSend(*client, server->serv_name));
		return false;
	}
    return true;
}

bool	handle_message::isValidKickParams(Client *client, const std::string &channel, const std::string &user)
{
	ChannelDomain &chanDomain = server->getChannelDomain();
	
	if (!chanDomain.isChannelExistent(channel)) {
		client->queueMessage(NumericReplies::noSuchChannel(*client,
															server->serv_name,
															channel));
		return false;
	}
	if (!chanDomain.isChannelMember(client, channel)) {
		client->queueMessage(NumericReplies::notOnChannel(*client,
														  server->serv_name,
														  channel));
		return false;
	}
	if (!chanDomain.isOperatorMember(client, channel)) {
		client->queueMessage(NumericReplies::chaNoPrivsNeed(*client,
															server->serv_name,
															channel));
		return false;
	}
	if (!chanDomain.isChannelMember(findClientByNickName(user), channel)) {
		client->queueMessage(NumericReplies::userNoTinChannel(*client,
															  server->serv_name,
															  channel,
															  user));
		return false;
	}
	return true;
}

bool	handle_message::isValidModeParams(Client *client, const IRCMessage &message)
{
	if (!client)
		return false;

	if (message.params.empty()) {
		client->queueMessage(NumericReplies::needMoreParams(
															*client,
															server->serv_name,
															message.command));
		return false;
	}
	std::string channel = message.params[0];
	ChannelDomain &chanDomain = server->getChannelDomain();
	if (!chanDomain.isChannelExistent(channel)) {
		client->queueMessage(NumericReplies::noSuchChannel(*client,
															server->serv_name,
															channel));
		return false;
	}
	if (!chanDomain.isChannelMember(client, channel)) {
		client->queueMessage(NumericReplies::notOnChannel(*client,
														  server->serv_name,
														channel));
		return false;
	}
	if (!chanDomain.isOperatorMember(client, channel)) {
		client->queueMessage(NumericReplies::chaNoPrivsNeed(*client,
															server->serv_name,
														channel));
		return false;
	}
	if (message.params.size() == 1) {
		client->queueMessage(NumericReplies::rpl_channelModeis(*client,
																server->serv_name,
																channel,
																chanDomain.getChannelModeStatus(channel)));
		return false;
	}
	return true;
}

void	handle_message::catJoinError(Client *client, std::string &channel, size_t status)
{
	switch (status)
	{
		case ERR_INVITEONLYCHAN:
			client->queueMessage(NumericReplies::iventeOnlyChannel(*client,
																	server->serv_name,
																	channel));
			break;
		case ERR_CHANNELISFULL:
			client->queueMessage(NumericReplies::channelListFull(*client,
																server->serv_name,
																channel));
			break;
		case ERR_BADCHANNELKEY:
			client->queueMessage(NumericReplies::badChannelKey(*client,
																server->serv_name,
																channel));
			break;
		default:
			break;
	}
}

void	handle_message::catModeError(Client *client, std::string &channel, size_t status, char mode)
{
	switch (status)
	{
		case ERR_KEYSET:
			client->queueMessage(NumericReplies::keySet(*client,
														server->serv_name,
														channel));
			break;
		case ERR_BADCHANNELKEY:
			client->queueMessage(NumericReplies::badChannelKey(*client,
																server->serv_name,
																channel));
			break;
		case ERR_UNKNOWNMODE:
			client->queueMessage(NumericReplies::UnknownMode(*client,
															 server->serv_name,
															 channel,
															 mode));
		default:
			break;
	}
}

//*************  BROADCASTS  *************/

void	handle_message::broadcastNickChange(Client *client,
											const std::string &oldPrefix,
											const std::string &newNick)
{
	if (!client)
		return ;
	std::string message = oldPrefix + " NICK :" + newNick;
	server->getChannelDomain().notifyChannelsClient(client, message);
}

void handle_message::broadcastQuit(Client *client, const std::string &reason)
{
    if (!client)
        return ;

    std::string message = clientPrefix(client) + " QUIT :" + reason;
	server->getChannelDomain().notifyChannelsClient(client, message);
}

void handle_message::broadcastJoin(Client *client, const std::string &channel)
{
	ChannelDomain &chanDomain = server->getChannelDomain();

	std::string message = client->getPrefix() + " JOIN " + channel;
	std::string topic = chanDomain.getChannelTopic(channel);

	client->queueMessage(message);

	if (topic.empty())
		client->queueMessage(NumericReplies::rpl_noTopic(*client,
													server->serv_name,
													channel
												));
	else
		client->queueMessage(NumericReplies::rpl_topic(*client,
														server->serv_name,
														channel,
														topic
													));

	client->queueMessage(NumericReplies::rpl_naReply(*client,
													server->serv_name,
													channel,
													chanDomain.getChannelSymbol(channel),
													chanDomain.getChannelMembersList(channel)
												));

	client->queueMessage(NumericReplies::rpl_endOfNames(*client,
														 server->serv_name,
														channel));
	t_notify notify;
	notify.message = message;
	notify.exceptFd = client->getFd();
	server->getChannelDomain().notifyMembersOfChannel(channel, notify);	
}

void	handle_message::broadcastPart(Client *client,
									  const std::string &channel,
									  const std::string &reason)
{
	if (!client)
        return ;

	t_notify notify;
	std::string message = client->getPrefix() + " PART " + channel + " :" + reason;
	notify.message = message;
	notify.exceptFd = NONE;
	server->getChannelDomain().notifyMembersOfChannel(channel, notify);	
}


void 	handle_message::broadcastKick(Client *client,
					 const std::string &channel,
					 const	std::string &target,
					 std::string reason)
{
	if (!client)
        return ;

	reason = reason.empty()? client->getNickname() : reason;
	t_notify notify;
	std::string message = client->getPrefix() + " KICK " + channel + " " + target + " :" + reason;
	notify.message = message;
	notify.exceptFd = NONE;
	server->getChannelDomain().notifyMembersOfChannel(channel, notify);	
}

void	handle_message::broadcastInvite(Client *client, Client *target, std::string &channel)
{
	if (!client || !target)
		return ;

	std::string nick = target->getNickname();
	std::string message = client->getPrefix() + " INVITE " + nick + " :" + channel;
	
	client->queueMessage(NumericReplies::rpl_inviting(*client,
													  server->serv_name,
													  channel,
													  nick ));
	target->queueMessage(message);
}


void	handle_message::broadcastTopic(Client *client,
									  const std::string &channel,
									  const std::string &topic)
{
	if (!client)
        return ;

	t_notify notify;
	std::string message = client->getPrefix() + " TOPIC " + channel + " :" + topic;
	notify.message = message;
	notify.exceptFd = NONE;
	server->getChannelDomain().notifyMembersOfChannel(channel, notify);	
}

void	handle_message::broadcastMode(Client *client,
									 const std::string &channel,
									 const std::string &modes,
									 const std::string &params)
{
	if (!client || modes.empty())
        return ;

	std::string message = client->getPrefix() + " MODE " + channel + " " + modes;
	if (!params.empty())
		message += " " + params;
	
	t_notify notify;
	notify.message = message;
	notify.exceptFd = NONE;
	server->getChannelDomain().notifyMembersOfChannel(channel, notify);	
}
