#ifndef DEBUG
# define DEBUG

# include <iostream>
# include <vector>
# include <map>

class Server;
class ChannDomain;

class Debug
{
	private:
		Server *server;
	
	public:
		Debug(Server *server);
		~Debug();

	void	listClientsOnServer( void );
	void	listChannelsOnServer( void );
	void	listChannelMembers(std::string name);
};

#endif