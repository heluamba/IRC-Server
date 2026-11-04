#ifndef CHANNEL
# define CHANNEL

# include "Debug.hpp"
# include "IRCMessage.hpp"

# include <string>
# include <vector>

# define NO_LIMIT -1

class	Client;

class Channel
{
	private:
		friend class Debug;
		friend class ChannelDomain;
		
		std::string				 name;
		std::string 			 modes;
		std::string				 topic;
		std::string				 allow_modes;
		std::string				 channelKey;
		std::vector<Client *> 	 membersList;
		std::vector<std::string> operators;
		std::vector<std::string> inviteOnlyList;
		
		bool					 topicMode;
		int					 	 limitMembers;
		bool 					 inviteOnlyMode;

		Channel();

	public:
		Channel(std::string channelName, Client *cli);
		Channel(std::string ChannelName, Client *cli, std::string key);
		~Channel();

		std::string 			 getName( void ) const;
		std::string				 getTopic( void ) const; 
		std::vector<Client *>	 getMembersList( void ) const;

		void	setTopicMode(bool value);
		void	setInviteOnly(bool value);
		void	setLimitMembers(int limit);
		void	setTopic(const std::string t);
		void	setPass(const std::string &key);
		void 	setName(const std::string &channeName);

		bool	isKeyModeOn() const;
		bool 	isInviteOnly() const;
		bool	isTopicModeOn() const;
		bool	isLimitModeOn() const;
		bool	isFullChannel() const;
		bool	isEmptyChannel() const ;
		bool	isMemberByFd(int fd) const;
		bool	isMemberByName(const std::string &memberName);
		bool	isOperatorMember(const std::string &memberName);
		bool	isOnInviteOnlyList(std::string memberName) const;
		bool	hasChannelKey(const std::string &key) const;
		bool	isAllowMode(const char mode) const;
		bool	hasMode(const char mode) const;

		void	addMode(const char mode);
		void 	addMember(Client *client);
		void	addOperator(const std::string &memberName);
		void	addToInviteOnlyList(const std::string &memberName);
		
		void	removeAllMembers();
		void	removeMode(char mode);
		void 	removeMemberByFd(int fd);
		void 	removeMemberByName(std::string &memberName);
		void	removeOperator(const std::string &memberName);
		void	removeOfInviteOnlyList(std::string &memberName);

		Client *findMemberByFd(int fd) const;
		Client *findMemberByName(const std::string &memberName) const;
};

#endif

// add* e idempotente