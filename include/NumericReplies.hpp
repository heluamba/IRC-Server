/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: misalber <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 15:22:04 by misalber          #+#    #+#             */
/*   Updated: 2026/09/19 15:22:07 by misalber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NUMERICREPLIES_HPP
#define NUMERICREPLIES_HPP

#include "Client.hpp"
#include <string>

class NumericReplies {
public:
  static std::string welcome(const Client &,
                             const std::string &serverName);

  static std::string noNicknameGiven(const Client &,
                                     const std::string &serverName);

  static std::string nicknameInUse(const Client &,
                                   const std::string &serverName,
                                   const std::string &nick);

static std::string erroNeusNickNmane(const Client &,
									 const std::string &server,
									 const std::string &nick);

  static std::string alreadyRegistered(const Client &,
                                       const std::string &serverName);

  static std::string needMoreParams(const Client &,
                                    const std::string &serverName,
                                    const std::string &command);

  static std::string passMismatch(const Client &,
                                  const std::string &serverName);

  static std::string notRegistered(const Client &,
                                   const std::string &serverName);

  static std::string noSuchNick(const Client &,
                                const std::string &serverName,
                                const std::string &nick);

  static std::string noRecipient(const Client &,
                                 const std::string &serverName);

  static std::string noTextToSend(const Client &,
                                  const std::string &serverName);

static std::string badchanmask(const Client &,
                        	   const std::string &server,
							   const std::string &channel);

static std::string noSuchChannel(const Client &,
                                 const std::string &server,
								 const std::string &channel);

static std::string toManyTargets(const Client &,
                                 const std::string &server,
								 const std::string &abortMessage);

static std::string toManyChannels(const Client &,
                                  const std::string &server,
								  const std::string &channel);

static std::string unavaiLResource(const Client &,
                                   const std::string &server,
								   const std::string &resource);

									 
static std::string badChannelKey(const Client &,
                                 const std::string &server,
								 const std::string &channel);

static std::string iventeOnlyChannel(const Client &,
                                     const std::string &server,
									 const std::string &channel);

static std::string channelListFull(const Client &,
                                   const std::string &server,
								   const std::string &channel);

static std::string umodeUnknowFlag(const Client &client,
                                   const std::string &server,
								   const std::string &flag);

static std::string rpl_noTopic(const Client &client,
							   const std::string &server,
							   const std::string &channel);

static std::string rpl_topic(const Client &client,
							 const std::string &server,
							 const std::string &channel,
							 const std::string &topic);

static std::string rpl_naReply(const Client &client,
							   const std::string &server,
							   const std::string &channel,
							   const std::string &symbol,
							   const std::string &members);

static std::string rpl_endOfNames(const Client &client,
								  const std::string &server,
								  const std::string &channel);

static std::string rpl_inviting(const Client &client,
									  	 const std::string &server,
									  	 const std::string &channel,
										 const std::string &nick);

static std::string rpl_channelModeis(const Client &client,
									  const std::string &server,
									  const std::string &channel,
									  const std::string &modeStatus);

static std::string UnknownCommand(const Client &client,
								  const std::string &server,
								  const std::string &command);

static std::string notOnChannel(const Client &client,
								const std::string &server,
								const std::string &channel);

static std::string canNotSendToChan(const Client &client,
									const std::string &server,
									const std::string &channel);



static std::string chaNoPrivsNeed(const Client &client,
								  const std::string &server,
								  const std::string &channel);

static std::string userNoTinChannel(const Client &client,
									const std::string &server,
									const std::string &channel,
									const std::string &nick);


static std::string userOnChannel(const Client &client,
								 const std::string &server,
								 const std::string &channel);

static std::string keySet(const Client &client,
								   const std::string &server,
								   const std::string &channel);

static std::string UnknownMode(const Client &client,
								   		const std::string &server,
								   		const std::string &channel,
										const char &mode);


static std::string userDontMatch(const Client &client,
						  const std::string &server);


};


#endif
