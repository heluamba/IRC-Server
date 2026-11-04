/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misalber <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:22:36 by misalber          #+#    #+#             */
/*   Updated: 2026/09/19 15:22:38 by misalber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Parser.hpp"
#include <cctype>
#include <cstddef>
#include <string>

std::string Parser::toUpper(const std::string &str) {
  std::string result;
  size_t i;

  result = str;
  i = 0;
  while (i < result.size()) {
    result[i] = std::toupper(static_cast<unsigned char>(result[i]));
    i++;
  }
  return (result);
}


IRCMessage Parser::Parse(const std::string &line)
{
  IRCMessage message;
  std::string input;
  size_t pos;

  pos = 0;
  input = line;

//   if (!input.empty() && input[0] == ':') {
//     size_t space = input.find(' ');

//     if (space == std::string::npos)
//         return message;

//     message.prefix = input.substr(1, space - 1);
//     pos = space + 1;
//   }

  if (!input.empty() && input[0] == ':')
  {
    size_t  space = input.find(' ');
    if (space == std::string::npos)
    {
      message.prefix = input.substr(1, space - 1);
      pos = space + 1;
    }
  }

  while (pos < input.size() && input[pos] == ' ')
    pos++;
  size_t commandEnd = input.find(' ', pos);
  if (commandEnd == std::string::npos)
  {
    message.command = toUpper(input.substr(pos));
    return message;
  }
  message.command = toUpper(input.substr(pos, commandEnd - pos));
  pos = commandEnd + 1;
  while (pos < input.size())
  {
    while (pos < input.size() && input[pos] == ' ')
      pos++;
    if (pos >= input.size())
      break;
    if (input[pos] == ':')
    {
      message.params.push_back(input.substr(pos + 1));
      break;
    }
    size_t paramEnd = input.find(' ', pos);
    if (paramEnd == std::string::npos)
    {
      message.params.push_back(input.substr(pos));
      break;
    }
    message.params.push_back(input.substr(pos, paramEnd - pos));
    pos = paramEnd + 1;
  }
  return (message);
}
