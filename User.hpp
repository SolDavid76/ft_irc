/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 15:30:59 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/22 15:27:20 by djanusz          ###   ########.fr       */
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

# include <iostream>
# include <vector>

# include "string"

class User
{
	public:
		User(int socket);
		User(User const& src);
		User& operator=(User const& src);
		~User(void);

		void readSocket(void);
	// private:
		int _socket;
		std::string _nickname;
		std::string _username;
		std::string _buffer;
};

#endif

// CAP LS
// NICK djanusz
// USER djanusz djanusz made-f0Cr10s2 :Dawid JANUSZ