/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 15:30:59 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/23 20:34:53 by djanusz          ###   ########.fr       */
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

# include <signal.h>
# include <iostream>
# include <vector>
# include <map>
#include <sstream>

# include "string"

class User
{
	public:
		User(int socket);
		User(User const& src);
		User& operator=(User const& src);
		~User(void);

		void readSocket(void);
		void disconect(void);
	// private:
		int _id;
		bool _irssi;
		pollfd _socket;
		std::string _nickname;
		std::string _username;
		std::string _buffer;

		static int nextId;
};

std::vector<std::string> ft_split(const std::string& input);

#endif
