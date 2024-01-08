/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ennollet <ennollet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:44:35 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/08 12:46:48 by ennollet         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void)
{
	this->_name = "";
	this->_password = "";
	this->_invitationOnly = false;
}

Channel::Channel(User& creator, std::string name)
{
	this->_name = name;
	this->_password = "";
	this->_invitationOnly = false;
	this->_users.push_back(&creator);
}

Channel::Channel(Channel const& src)
{
	*this = src;
}

Channel& Channel::operator=(Channel const& src)
{
	if (this != &src)
	{
		this->_name = src._name;
		this->_password = src._password;
		this->_invitationOnly = src._invitationOnly;
		this->_invited = src._invited;
		this->_users = src._users;
	}
	return (*this);
}

Channel::~Channel(void)
{
}

int Channel::findUser(std::string name)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->_nickname == name)
			return (i);
	}
	return (-1);	
}

