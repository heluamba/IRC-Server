/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misalber <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:22:44 by misalber          #+#    #+#             */
/*   Updated: 2026/09/19 15:22:56 by misalber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Client.hpp"
#include "../include/Server.hpp"

Client::Client() : fd(-1), registered(false),
				   passwordOk(false), toRemove(false) {}

Client::Client(int fd)
    : fd(fd), registered(false), passwordOk(false), toRemove(false) {}

Client::Client(int fd, const std::string &ip, Server *server)
    : fd(fd), server(server),ip(ip), registered(false), passwordOk(false) {}

Client::~Client() {}

int Client::getFd() const { return fd; }

std::string &Client::getBuffer() { return buffer; }
std::string &Client::getOutBuffer() { return outbuffer; }

const std::string &Client::getIp() const { return ip; }
const std::string &Client::getPrefix() const {return prefix; }
const std::string &Client::getNickname() const { return nickname; }
const std::string &Client::getUsername() const { return username; }
const std::string &Client::getRealname() const { return realname; }
const std::vector<std::string> &Client::getMemberShip() const { return memberShip; }

const std::string &Client::get_a_name() const
{
	if (!nickname.empty())
		return nickname;
	if (!username.empty())
		return username;
	return realname; 
}

bool Client::isToRemove() const {return toRemove; }
bool Client::isRegistered() const { return registered; }
bool Client::isPasswordOk() const { return passwordOk; }
bool Client::isOutputBufferEmpty() const { return outbuffer.empty(); }

void Client::setNickname(const std::string &nick) { nickname = nick; }
void Client::setUsername(const std::string &user) { username = user; }
void Client::setRealname(const std::string &real) { realname = real; }
void Client::setOutBuffer(std::string buffer) {outbuffer = buffer; }
void Client::setPrefix(const std::string &prefi){prefix = prefi; }

void Client::setToRemove(bool value) { std::cout << nickname << '\n';  toRemove = value; }
void Client::setRegistered(bool value) { registered = value; }
void Client::setPasswordOk(bool value) { passwordOk = value; }

void Client::appendToOutputBuffer(std::string message)
{
	if (outbuffer.empty())
		outbuffer = message;
	else
		outbuffer.append(message);
}

void Client::consumeOutputBuffer(size_t n)
{
	if (n >= outbuffer.size())
		outbuffer.clear();
	else
		outbuffer.erase(0, n);
}

void Client::queueMessage(const std::string &message)
{
	std::string msg = message;
	if (msg.size() < 2 || msg.substr(msg.size() - 2) != "\r\n")
		msg += "\r\n";

	appendToOutputBuffer(msg);
	server->updateServerPollEvents(fd, POLLOUT, true);
}

bool Client::isMemberShip(const std::string &channelName)
{
	for (size_t i = 0; i < memberShip.size(); i++)
	{
		if (channelName == memberShip[i])
			return true ;
	}
	return false ;
}

void Client::addToMemberShip(const std::string &channelName)
{
	if (!isMemberShip(channelName))
		memberShip.push_back(channelName);
}

void Client::removeOfMemberShip(const std::string &channelName)
{
	std::vector<std::string>::iterator it;

	for (it = memberShip.begin(); it != memberShip.end(); it++)
	{
		if (*it == channelName)
		{
			memberShip.erase(it);
			return;
		}
	}
}
