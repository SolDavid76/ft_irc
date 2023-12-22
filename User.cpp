/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 16:24:56 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/22 16:01:33 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int socket)
{
	pollfd tmp;
	tmp.fd = socket;
	tmp.events = POLLIN;
	tmp.revents = 0;
	this->_socket = tmp;
	this->_nickname = "";
	this->_username = "";
}

User::User(User const& src)
{
	*this = src;
}

User& User::operator=(User const& src)
{
	if (this != &src)
	{
		this->_socket = src._socket;
		this->_nickname = src._nickname;
		this->_username = src._username;
	}
	return (*this);
}

User::~User(void)
{
}
