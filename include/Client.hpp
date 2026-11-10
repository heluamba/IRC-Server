/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbarber <cbarber@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 19:01:00 by cbarber           #+#    #+#             */
/*   Updated: 2026/09/19 13:17:35 by misalber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <string>
# include <vector>

class	Server;

class Client
{
	private:
		int 			fd;
	
		Server			*server;
		std::string 	ip;
		std::string 	buffer;
		std::string 	prefix;
		std::string 	nickname;
		std::string 	username;
		std::string 	realname;
		std::string 	outbuffer;

		std::vector<std::string> memberShip;
  		
		bool	registered;
		bool	passwordOk;
		bool	toRemove;

	public:
  		Client();
  		Client(int fd);
  		Client(int fd, const std::string &ip, Server *server);
  		~Client();

  		int getFd() const;
  		const std::string &getIp() const;
  		std::string &getBuffer();
  		std::string &getOutBuffer();
  		const std::string &getNickname() const;
  		const std::string &getUsername() const;
  		const std::string &getRealname() const;
		const std::string &get_a_name() const;
		const std::string &getPrefix() const;
		
		const std::vector<std::string> &getMemberShip() const;

		void setPrefix(const std::string &prefi);
		void setNickname(const std::string &nick);
		void setUsername(const std::string &user);
		void setRealname(const std::string &real);
		void setOutBuffer(std::string buffer);
		void setRegistered(bool value);
		void setPasswordOk(bool value);
		void setToRemove(bool value);
		void setFd(int Fd) { fd = Fd; };
		void setIp(const std::string &Ip) { ip = Ip; };
		void appendToOutputBuffer(std::string message);
		void consumeOutputBuffer(size_t n);
		void queueMessage(const std::string &message);
		void addToMemberShip(const std::string &channelName);
		void removeOfMemberShip(const std::string &channelName);
		
		bool isMemberShip(const std::string &channelName);
		bool isOutputBufferEmpty() const;
  		bool isRegistered() const;
  		bool isPasswordOk() const;
		bool isToRemove() const;
};

#endif
