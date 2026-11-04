#ifndef UTILS_HPP 
# define UTILS_HPP

# include <string>
# include <vector>
# include <map>

typedef	std::map<std::string, std::string> channelsKeys;

class utils
{
	private:
		utils();
		~utils();

	public:
		static std::string toLower(const std::string &str);
		static bool	isValidNickName(const std::string &nick);
		static bool isValidChannelName(const std::string &nick);
		static std::vector<std::string>	split(std::string str, char c);
		static channelsKeys extactJoinFields(const std::string &channels,
												const std::string keys);
};

#endif