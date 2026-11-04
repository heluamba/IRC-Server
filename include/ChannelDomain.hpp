#ifndef CHANNELDOMAIN
# define CHANNELDOMAIN

# include "Client.hpp"
# include "Channel.hpp"
# include <map>
# include <set>

# define NONE					-1
# define TARGETS_SIZE			50
# define CHANNELS_SIZE			100

# define SUCESS					200

# define ERR_UNDEF				0
# define ERR_CANNOTSENDTOCHAN 	404
# define ERR_TOOMANYCHANNELS	405
# define ERR_TOOMANYTARGETS		407
# define ERR_NORECIPIENT		411
# define ERR_NOTEXTTOSEND		412
# define ERR_USERNOTINCHANNEL	441
# define ERR_USERONCHANNEL		443
# define ERR_KEYSET				467
# define ERR_CHANNELISFULL		471
# define ERR_UNKNOWNMODE		472
# define ERR_INVITEONLYCHAN 	473
# define ERR_BADCHANNELKEY		475
# define ERR_BADCHANMASK		476
# define ERR_UMODEUNKNOWNFLAG	501


typedef struct	s_notify
{
	std::string			message;
	int					exceptFd;
	std::set<int> 		alreadyNotified;
}				t_notify;

class	Server;
class	Channel;
class	Debug;

class ChannelDomain
{
	private:
		friend	class Debug;
		
		// { name: Channel }
		std::map<std::string, Channel>	channels;
			
		Channel 	*getChannel(const std::string &channelName);
		size_t		keyMode(Channel &chan, bool activate, const std::string key);
		size_t		limitMode(Channel &chan, bool activate, const std::string limit);
		size_t		operatorMode(Channel &chan, bool activate, const std::string nick);

	public:
		ChannelDomain();
		~ChannelDomain();

		std::string	getChannelTopic(const std::string &channelName);
		std::string getChannelSymbol(const std::string &channelName);
		std::string extractChannelTopic(const std::string &channleName);
		std::string	getChannelModeStatus(const std::string &channelName);
		std::string getChannelMembersList(const std::string &channelName);

		size_t 		applyMode(Client *, std::string &channelName,
							  const char &mode, const std::string &params, bool activate);
		size_t		tryJoin(Client *, const std::string &channelName, const std::string &key);

		bool		isChannelInviteOnly(const std::string &channelName);
		bool		isChannelTopicModeOn(const std::string &channelName);
		bool		isChannelExistent(const std::string &channelName) const;
		bool		isChannelMember(Client *client, const std::string &channelName);
		bool		isOperatorMember(Client *client, const std::string &channelName);

		void		createChannel(Client *, std::string channelName, std::string key);
		void		addToChannelInvitedList(std::string &channelName, std::string &nick);
		void		changeChannelTopic(std::string &channelName, const std::string &topic);

		void		deleteChannel(const std::string channelName);
		void		removeClientFromChannels(Client *);
		void		removeMemberOfChannel(std::string &memberName, std::string &channelName);
		void		notifyMembersOfChannel(const std::string &channelName, t_notify &notify);
		void		notifyChannelsClient(Client *, const std::string &message);

};

#endif