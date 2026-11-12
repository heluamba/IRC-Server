#ifndef MESSAGE_HPP
#define MESSAGE_HPP
#define CMD_NUMBERS 14

#include "Debug.hpp"
#include "utils.hpp"
#include "Client.hpp"
#include "IRCMessage.hpp"
#include <string>

class Server;
class Client;

class handle_message
{
	private:
		typedef void (handle_message::*HandleFunc)(Client *, const IRCMessage &);
		
		Server 		*server;
		HandleFunc 	*handleCommands;
		std::string *commandNames;

		// HANDLE MESSAGE 
		void 	tryRegiste(Client *client);
		void 	handleLine(int fd, const std::string &line);
		void 	handleMessage(int fd, const IRCMessage &message);
		bool 	nicknameInUse(const std::string &nickname, Client *client) const;
		void	catJoinError(Client *, std::string &channel, size_t status);
		void	catModeError(Client *, std::string &channel, size_t status, char mode);
		
		bool	isValidModeParams(Client *, const IRCMessage &message);
		bool	isValidPrivmsgParams(Client *, const IRCMessage &message);
		bool	isValidJoinParams(Client *, const IRCMessage &message, channelsKeys &chankeys);
		bool	isValidKickParams(Client *, const std::string &channel, const std::string &user);

		std::string clientPrefix(const Client *client) const;
		std::string *startCommandNames( void );
		
		HandleFunc *startHandleCMDFunc( void );

		Client 		*findClientByNickName(const std::string &nickname);
		
		// HANDLE COMMANDS
		void 	handleNick(Client *, const IRCMessage &message);
		void 	handleUser(Client *, const IRCMessage &message);
		void 	handlePass(Client *, const IRCMessage &message);
		void 	handleQuit(Client *, const IRCMessage &message);
		void 	handlePrivmsg(Client *, const IRCMessage &message);
		void	handle_part(Client *client, const IRCMessage &msg);
		void	handle_join(Client *, const IRCMessage &message);
		void	handle_kick(Client *, const IRCMessage &message);
		void	handle_invite(Client *, const IRCMessage &message);
		void	handle_topic(Client *, const IRCMessage &message);
		void	handle_mode(Client *, const IRCMessage &message);
		void	handle_ping(Client *, const IRCMessage &message);
		void	handle_cap(Client *, const IRCMessage &message);

		void	handle_debug(Client *, const IRCMessage &message);
		
		void 	broadcastQuit(Client *, const std::string &reason);
		void	broadcastJoin(Client *, const std::string &channel);
		void	broadcastInvite(Client *, Client *target, std::string &channel);
		void 	broadcastPart(Client *, const std::string &channel, const std::string &reason);
		void	broadcastTopic(Client *, const std::string &channel, const std::string &topic);
		void	broadcastNickChange(Client *, const std::string &oldPrefix, const std::string &newNick);
		void 	broadcastKick(Client *, const std::string &channel, const std::string &target, std::string reason);
		void	broadcastMode(Client *, const std::string &channel, const std::string &modes, const std::string &params);

	public:
		explicit 	handle_message(Server *server);
		~handle_message();

  		void	processLine(int fd, const std::string &line);
		void	removeClientFromServer(Client *);
		
};

#endif