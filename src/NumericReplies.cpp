/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericReplies.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misalber <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:22:28 by misalber          #+#    #+#             */
/*   Updated: 2026/09/19 15:22:30 by misalber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/NumericReplies.hpp"

static std::string target(const Client &client) {
  if (client.getNickname().empty())
    return "*";

  return client.getNickname();
}

static std::string make(const Client &client, const std::string &server,
                        const std::string &code, const std::string &text) {
  return ":" + server + " " + code + " " + target(client) + " :" + text +
         "\r\n";
}

std::string NumericReplies::welcome(const Client &client,
                                    const std::string &server) {
  return make(client, server, "001", "Welcome to the IRC server");
}

std::string NumericReplies::noNicknameGiven(const Client &client,
                                            const std::string &server) {
  return make(client, server, "431", "No nickname given");
}

std::string NumericReplies::erroNeusNickNmane(const Client &client,
											  const std::string &server,
											  const std::string &nick){
    return make(client, server, "432", nick + " :Erroneous nickname");
}

std::string NumericReplies::nicknameInUse(const Client &client,
                                          const std::string &server,
                                          const std::string &nick) {
  return make(client, server, "433", nick + "Nickname is already in use");
}

std::string NumericReplies::alreadyRegistered(const Client &client,
                                              const std::string &server) {
  return make(client, server, "462", "Unauthorized command (already registered)");
}

std::string NumericReplies::needMoreParams(const Client &client,
                                           const std::string &server,
                                           const std::string &command) {
  return make(client, server, "461", command + " Not enough parameters");
}

std::string NumericReplies::passMismatch(const Client &client,
                                         const std::string &server) {
  return make(client, server, "464", "Password incorrect");
}

std::string NumericReplies::notRegistered(const Client &client,
                                          const std::string &server)
{
  return make(client, server, "451", "You have not registered");
}

std::string NumericReplies::noSuchNick(const Client &client,
                                       const std::string &server,
                                       const std::string &nick)
{
  return make(client, server, "401", nick + "No such nick/channel");
}

std::string NumericReplies::noRecipient(const Client &client,
                                        const std::string &server)
										{
  return make(client, server, "411", "No recipient given (PRIVMSG)");
}

std::string NumericReplies::noTextToSend(const Client &client,
                                         const std::string &server)
										 {
  return make(client, server, "412", "No text to send");
}

std::string NumericReplies::badchanmask(const Client &client,
                                     const std::string &server,
									 const std::string &channel) {
	std::string msg = channel + " :Bad Channel Mask";
	return make(client, server, "476", msg);
}

std::string NumericReplies::noSuchChannel(const Client &client,
                                     const std::string &server,
									 const std::string &channel)
{
	std::string msg = channel + " :No such channel";
	return make(client, server, "403", msg);
}



std::string NumericReplies::toManyTargets(const Client &client,
                                     const std::string &server,
									 const std::string &abortMessage)
{
	std::string msg = "recepients." + abortMessage;
	return make(client, server, "407", msg);
}


std::string NumericReplies::toManyChannels(const Client &client,
                                     const std::string &server,
									 const std::string &channel)
{
	std::string msg = channel + " :You have joined too many channels";
	return make(client, server, "405", msg);
}


std::string NumericReplies::unavaiLResource(const Client &client,
                                     const std::string &server,
									 const std::string &resource)
{
	std::string msg = resource + " :is temporarily unavailable";
	return make(client, server, "437", msg);
}

std::string NumericReplies::iventeOnlyChannel(const Client &client,
                                     const std::string &server,
									 const std::string &channel)
{
	std::string msg = channel + " :Cannot join channel (+i)";
	return make(client, server, "473", msg);
}

std::string NumericReplies::channelListFull(const Client &client,
                                     const std::string &server,
									 const std::string &channel)
{
	std::string msg = channel + " :Channel list is full";
	return make(client, server, "478", msg);
}

std::string NumericReplies::badChannelKey(const Client &client,
                                     const std::string &server,
									 const std::string &channel)
{
	std::string msg = channel + " :Cannot join channel (+k)";
	return make(client, server, "478", msg);
}


std::string NumericReplies::umodeUnknowFlag(const Client &client,
                                     const std::string &server,
									 const std::string &flag)
{
	std::string msg = flag + " :Unknown MODE flag";
	return make(client, server, "478", msg);
}

std::string NumericReplies::rpl_noTopic(const Client &client,
									  const std::string &server,
									  const std::string &channel)
{
	std::string msg = channel + " :No topic is set";
	return make(client, server, "331", msg);
}

std::string NumericReplies::rpl_topic(const Client &client,
									  const std::string &server,
									  const std::string &channel,
									  const std::string &topic)
{
	std::string msg = channel + " :" + topic;
	return make(client, server, "332", msg);
}

std::string NumericReplies::rpl_naReply(const Client &client,
									  const std::string &server,
									  const std::string &channel,
									  const std::string &symbol,
									  const std::string &members)
{
	std::string msg = symbol + " " + channel + " :" + members;
	return make(client, server, "353", msg);
}

std::string NumericReplies::rpl_endOfNames(const Client &client,
									  const std::string &server,
									  const std::string &channel)
{
	std::string msg = channel + " :End of NAMES list";
	return make(client, server, "366", msg);
}

std::string NumericReplies::rpl_channelModeis(const Client &client,
									  const std::string &server,
									  const std::string &channel,
									  const std::string &modeStatus)
{
	std::string msg = channel + " " + modeStatus;
	return make(client, server, "324", msg);
}

std::string NumericReplies::UnknownCommand(const Client &client,
									  const std::string &server,
									  const std::string &command)
{
	std::string msg = command + " :Unknown command";
	return make(client, server, "421", msg);
}

std::string NumericReplies::notOnChannel(const Client &client,
									  const std::string &server,
									  const std::string &channel)
{
	std::string msg = channel + " :You're not on that channel";
	return make(client, server, "442", msg);
}


std::string NumericReplies::canNotSendToChan(const Client &client,
									  const std::string &server,
									  const std::string &channel)
{
	std::string msg = channel + " :Cannot send to channel";
	return make(client, server, "404", msg);
}

std::string NumericReplies::chaNoPrivsNeed(const Client &client,
									  const std::string &server,
									  const std::string &channel)
{
	std::string msg = channel + " :You're not channel operator";
	return make(client, server, "482", msg);
}


std::string NumericReplies::userNoTinChannel(const Client &client,
									  		 const std::string &server,
									  		 const std::string &channel,
											 const std::string &nick)
{
	std::string msg = nick + " " + channel + " :They aren't on that channel";
	return make(client, server, "441", msg);
}

std::string NumericReplies::userOnChannel(const Client &client,
									  		 const std::string &server,
									  		 const std::string &channel)
{
	std::string msg = channel + " :is already on channel";
	return make(client, server, "443", msg);
}

std::string NumericReplies::rpl_inviting(const Client &client,
									  		 const std::string &server,
									  		 const std::string &channel,
											 const std::string &nick)
{
	std::string msg = channel + " " + nick;
	return make(client, server, "341", msg);
}

std::string NumericReplies::keySet(const Client &client,
								   const std::string &server,
								   const std::string &channel)
{
	std::string msg = channel + " :Channel key already set";
	return make(client, server, "467", msg);
}

std::string NumericReplies::UnknownMode(const Client &client,
								   		const std::string &server,
								   		const std::string &channel,
										const char &mode)
{
	std::string msg = std::string(1, mode) + " :is unknown mode char to me for " + channel;
	return make(client, server, "472", msg);
}

std::string NumericReplies::userDontMatch(const Client &client,
								   		  const std::string &server)
{
	std::string msg = " :Cannot change mode for other users";
	return make(client, server, "502", msg);
}