/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 16:24:56 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/20 16:54:16 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User(void)
{
	this->_socket = 0;
	this->_nick = "";
	this->_user_name = "";
	this->_session = "";
	this->_host = "";
	this->_real_name = "";
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
		this->_nick = src._nick;
		this->_user_name = src._user_name;
		this->_session = src._session;
		this->_host = src._host;
		this->_real_name = src._real_name;
	}
	return (*this);
}

User::~User(void)
{
}
