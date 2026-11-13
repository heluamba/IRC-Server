#ifndef BOTUTILS_HPP 
# define BOTUTILS_HPP

# include <string>
# include <vector>
# include <map>

struct BotMessage {
  std::string command;
  std::string target;
  std::string params;
};

typedef	std::map<std::string, std::string> channelsKeys;

class BotUtils
{
	private:
		BotUtils();
		~BotUtils();
		
		static std::string		extractTarget(const std::string &str);
		static std::string		extractCommandParams(const std::string &str);

	public:
		static bool	isValidNickName(const std::string &nick);
		static bool isValidChannelName(const std::string &nick);
		static bool	isValidNumber(std::string nu);
		
		static std::vector<std::string>	split(std::string str, char c);
		static std::string toLower(const std::string &str);

		static const BotMessage	parseBotMessage(const std::string &line);
		static const std::string toManyArguments(const BotMessage &msg);
};

#endif