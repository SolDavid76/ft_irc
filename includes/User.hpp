/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 15:30:59 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/18 12:55:21 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <unistd.h>
# include <poll.h>
# include <stdlib.h>

# include <algorithm>
# include <signal.h>
# include <iostream>
# include <vector>
# include <map>
# include <sstream>
# include <limits>
# include <cstdlib>

# include "string"

class User
{
	public:
		User(int socket);
		User(User const& src);
		User& operator=(User const& src);
		bool operator==(User const& other);
		~User(void);

		void readSocket(void);
		void disconect(void);
		bool isAuthentified(void);
		void ft_send(std::string msg);
		bool isIn(std::vector<User*> users);
		int findUserIn(std::vector<User*> users);

		/* GETTERS */
		int getId() const { return _id; }
		bool getIrssi() const { return _irssi; }
		const pollfd& getSocket() const { return _socket; }
		const std::string& getHostname() const { return _hostname; }
		const std::string& getPassword() const { return _password; }
		const std::string& getNickname() const { return _nickname; }
		const std::string& getUsername() const { return _username; }
		const std::string& getBuffer() const { return _buffer; }

		/* SETTERS */
		void setId(int id) { _id = id; }
		void setIrssi(bool irssi) { _irssi = irssi; }
		void setSocket(const pollfd& socket) { _socket = socket; }
		void setHostname(const std::string& hostname) { _hostname = hostname; }
		void setPassword(const std::string& password) { _password = password; }
		void setNickname(const std::string& nickname) { _nickname = nickname; }
		void setUsername(const std::string& username) { _username = username; }
		void setBuffer(const std::string& buffer) { _buffer = buffer; }

		static int nextId;
	private:
		int _id;
		bool _irssi;
		pollfd _socket;
		std::string _hostname;
		std::string _password;
		std::string _nickname;
		std::string _username;
		std::string _buffer;

};

std::vector<std::string> ft_split(std::string const& input);
std::vector<std::string> ft_split(std::string const& input, char separator);

template <typename T>
std::string to_string(T const& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

#endif
