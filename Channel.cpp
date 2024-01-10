/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:44:35 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/10 16:03:52 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(User& owner, std::string name)
{
	this->_name = name;
	this->_invitationOnly = false;
	this->_password = "";
	this->_maxUsers = std::numeric_limits<size_t>::max();
	this->_topic = "No topic is set";
	this->_owner = &owner;
	this->_admins.push_back(&owner);
	this->_JOIN(owner);
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
		this->_invitationOnly = src._invitationOnly;
		this->_password = src._password;
		this->_maxUsers = src._maxUsers;
		this->_topic = src._topic;
		this->_owner = src._owner;
		this->_invited = src._invited;
		this->_users = src._users;
		this->_admins = src._admins;
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

