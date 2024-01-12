/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:44:35 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/12 19:06:28 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(User* owner, std::string name)
{
	this->_name = name;
	if (name[0] == '&')
		this->_invitationOnly = true;
	else
		this->_invitationOnly = false;
	this->_topicCanBeChange = true;
	this->_password = "";
	this->_maxUsers = std::numeric_limits<size_t>::max();
	this->_topic = "No topic is set";
	this->_owner = owner;
	this->_admins.push_back(owner);
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
		this->_topicCanBeChange = src._topicCanBeChange;
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

void Channel::ft_sendAll(std::string msg)
{
	for (size_t i = 0; i < this->_users.size(); i++)
		this->_users[i]->ft_send(msg);
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

std::string Channel::userList(void)
{
	std::string res;
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->isIn(this->_admins))
			res = res + "@" + this->_users[i]->_nickname + " ";
		else
			res = res + this->_users[i]->_nickname + " ";
	}
	return (res);
}

std::string Channel::modsList(void)
{
	std::string res = "+";
	res += (this->_invitationOnly ? "" : "o");
	res += (this->_topicCanBeChange ? "" : "t");
	res += (this->_password.empty() ? "" : "k");
	res += (this->_maxUsers == std::numeric_limits<size_t>::max() ? "" : "k");
	res += " ";
	res += (this->_password.empty() ? "" : this->_password);
	res += (this->_maxUsers == std::numeric_limits<size_t>::max() ? "" : to_string(this->_maxUsers));
	return (res);
}

void Channel::_JOIN(User* user)
{
	this->_users.push_back(user);
	this->ft_sendAll(":" + user->_nickname + "!" + user->_username + "@" + user->_hostname + " JOIN " + this->_name + "\r\n");
	user->ft_send(":" + user->_hostname + " 332 " + user->_nickname + " " + this->_name + " :" + this->_topic + "\r\n");
	user->ft_send(":" + user->_hostname + " 353 " + user->_nickname + " = " + this->_name + " :" + this->userList() + "\r\n");
	user->ft_send(":" + user->_hostname + " 366 " + user->_nickname + " " + this->_name + " :End of name list\r\n");
}

void Channel::_PART(User* user, std::string msg)
{
	this->ft_sendAll(":" + user->_nickname + "!" + user->_username + "@" + user->_hostname + " PART " + this->_name + " " + msg + "\r\n");
	if (user->isIn(this->_admins) && this->_admins.size() == 1 && this->_users.size() > 1)
	{
		this->_admins.push_back(this->_users[1]);
		this->ft_sendAll(":" + user->_hostname + " MODE " + this->_name + " +o " + this->_users[1]->_nickname + "\r\n");
	}
	if (user == this->_owner && this->_admins.size() > 1)
		this->_owner = this->_admins[1];
	this->_users.erase(this->_users.begin() + user->findUserIn(this->_users));
	if (user->isIn(this->_admins))
		this->_admins.erase(this->_admins.begin() + user->findUserIn(this->_admins));
}
