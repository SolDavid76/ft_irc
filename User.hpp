/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 15:30:59 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/08 15:50:00 by djanusz          ###   ########.fr       */
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
		bool isIn(std::vector<User> users);
		bool isIn(std::vector<User*> users);
	// private:
		int _id;
		bool _irssi;
		pollfd _socket;
		std::string _hostname;
		std::string _password;
		std::string _nickname;
		std::string _username;
		std::string _buffer;

		static int nextId;
};

std::vector<std::string> ft_split(const std::string& input, char separator);

template <typename T>
std::string to_string(T const& value)
{
	std::ostringstream oss;
	oss << value;
	return (oss.str());
}

#endif
