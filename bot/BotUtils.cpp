#include <iostream>
#include "../include/BotUtils.hpp"

BotUtils::BotUtils() {};
BotUtils::~BotUtils() {};

std::string BotUtils::toLower(const std::string &str)
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

std::vector<std::string> BotUtils::split(std::string str, char c)
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

bool	BotUtils::isValidNumber(std::string nu)
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

bool	BotUtils::isValidNickName(const std::string &nick)
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

bool	BotUtils::isValidChannelName(const std::string &name)
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

std::string	BotUtils::extractCommand(const std::string &str)
{
	std::string command;
	size_t	cmdPos = str.find(":!");
	if (cmdPos != std::string::npos)
		command = str.substr(cmdPos + 1);
	return (command);
}

//Try extract channel or nick target from :nick!user@host PRIVMSG #canal :!comando
std::string BotUtils::extractTarget(const  std::string &str)
{
	std::string target;
	size_t targetPos = str.find("#");
	if (targetPos != std::string::npos) {
		target = str.substr(targetPos, str.find(" ", targetPos) - targetPos);
	}
	else {
		if ((targetPos = str.find("!")) != std::string::npos) {
			target = str.substr(1, targetPos - 1);
		}
	}
	return (target);
}
