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
		static bool	isValidNickName(const std::string &nick);
		static bool isValidChannelName(const std::string &nick);
		static bool	isValidNumber(std::string nu);
		
		static std::vector<std::string>	split(std::string str, char c);
		static std::string toLower(const std::string &str);
		static channelsKeys extractJoinFields(const std::string &channels,
												const std::string keys);
};

#endif