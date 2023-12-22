/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 16:24:56 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/22 18:58:56 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(int socket)
{
	this->_socket.fd = socket;
	this->_socket.events = POLLIN;
	this->_socket.revents = 0;
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
