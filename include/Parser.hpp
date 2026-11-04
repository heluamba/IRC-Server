/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misalber <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:22:12 by misalber          #+#    #+#             */
/*   Updated: 2026/09/19 15:22:14 by misalber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_HPP
#define PARSER_HPP

#include "IRCMessage.hpp"
#include <string>

class Parser {
private:
  static std::string toUpper(const std::string &value);

public:
  Parser();
  Parser(const Parser &other);
  Parser &operator=(const Parser &other);
  ~Parser();
  static IRCMessage Parse(const std::string &line);
};

#endif
