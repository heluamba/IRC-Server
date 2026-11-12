#include "../include/utils.hpp"
#include <iostream>

utils::utils() {};
utils::~utils() {};

std::string utils::toLower(const std::string &str)
{
  std::string result;
  size_t i;

  result = str;
  i = 0;
  while (i < result.size()) {
    result[i] = std::tolower(static_cast<unsigned char>(result[i]));
    i++;
  }
  return (result);
}

std::vector<std::string> utils::split(std::string str, char c)
{
	size_t pos;
	std::vector<std::string> result;

	while (!str.empty())
	{
		pos = str.find(c);

		if (pos == std::string::npos)
		{
			result.push_back(str);
			break;
		}

		result.push_back(str.substr(0, pos));
		str.erase(0, pos + 1);
	}
	return result;
}

bool	utils::isValidNumber(std::string nu)
{
	unsigned char d;
	size_t size = nu.size();

	for (size_t i = 1; i < size; ++i)
    {
        d = static_cast<unsigned char>(nu[i]);

        if (!(d >= '0' && d <= '9') || d == '-')
            return false;
    }
	return true;
}

bool	utils::isValidNickName(const std::string &nick)
{
    const std::string special = "[]\\`_^{}|";

	if (nick.empty() || nick.size() > 9)
        return false;

	unsigned char c = static_cast<unsigned char>(nick[0]);

    bool isLetter = (c >= 'A' && c <= 'Z')
                 || (c >= 'a' && c <= 'z');

    if (!isLetter && special.find(c) == std::string::npos)
        return false;

    for (size_t i = 1; i < nick.size(); ++i)
    {
        c = static_cast<unsigned char>(nick[i]);

        isLetter = (c >= 'A' && c <= 'Z')
                || (c >= 'a' && c <= 'z');

        bool isDigit = c >= '0' && c <= '9';
        bool isSpecial = special.find(c) != std::string::npos;

        if (!isLetter && !isDigit && !isSpecial && c != '-')
            return false;
    }
    return true;
}

bool	utils::isValidChannelName(const std::string &name)
{
	if (name.empty() || name.size() == 1)
		return (false);

	std::string	start = "#&+!";
	unsigned char c = static_cast<char>(name[0]);

	bool beginOk = start.find(c) != std::string::npos;
	bool contentOk = name.find_first_of(" ,\a") == std::string::npos;

	if (!beginOk || !contentOk)
		return false;
	return true;
}

channelsKeys	utils::extractJoinFields(const std::string &channels,
											const std::string keys)
{
	channelsKeys		chanKeys;

	size_t	size_chan = channels.size();
	bool	stop_key = false;
	size_t	startChan = 0;
	size_t	startKey = 0;
	size_t 	endChan;
	size_t 	endKey;
	while (startChan < size_chan)
	{
		endChan = channels.find_first_of(", ", startChan);
		if (!stop_key)
			endKey = keys.find_first_of(", ", startKey);
		std::string subKey;

		if (channels[endChan] == ' ' || keys[endKey] == ' ')
			return channelsKeys();

		std::string subChan = channels.substr(startChan, endChan - startChan);
		if (!stop_key)
			subKey = keys.substr(startKey, endKey - startKey);
		
		chanKeys.insert(std::make_pair(subChan, subKey));
		if (endChan == std::string::npos)
			break ;
		startChan = endChan + 1;
		if (endKey == std::string::npos)
		{
			stop_key = true;
			continue ;
		}
		startKey = endKey + 1;
	}
	return chanKeys;
}