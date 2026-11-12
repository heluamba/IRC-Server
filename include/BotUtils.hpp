#ifndef BOTUTILS_HPP 
# define BOTUTILS_HPP

# include <string>
# include <vector>
# include <map>

typedef	std::map<std::string, std::string> channelsKeys;

class BotUtils
{
	private:
		BotUtils();
		~BotUtils();

	public:
		static bool	isValidNickName(const std::string &nick);
		static bool isValidChannelName(const std::string &nick);
		static bool	isValidNumber(std::string nu);
		
		static std::vector<std::string>	split(std::string str, char c);
		static std::string toLower(const std::string &str);

		static std::string extractCommand(const std::string &str);
		static std::string extractTarget(const std::string &str);
};

#endif