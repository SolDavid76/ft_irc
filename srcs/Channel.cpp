/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:44:35 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/18 13:08:45 by djanusz          ###   ########.fr       */
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
	this->_topicCanBeChange = false;
	this->_password = "";
	this->_maxUsers = std::numeric_limits<size_t>::max();
	this->_topic = "";
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

void Channel::ft_sendAll(User* user, std::string msg)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (user->getId() != this->_users[i]->getId())
			this->_users[i]->ft_send(msg);
	}
}

int Channel::findUser(std::string name)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->getNickname() == name)
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
			res = res + "@" + this->_users[i]->getNickname() + " ";
		else
			res = res + this->_users[i]->getNickname() + " ";
	}
	return (res);
}

std::string Channel::modsList(void)
{
	std::string res = "+";
	res += (this->_invitationOnly ? "i" : "");
	res += (this->_topicCanBeChange ? "" : "t");
	res += (this->_password.empty() ? "" : "k");
	res += (this->_maxUsers == std::numeric_limits<size_t>::max() ? "" : "l");
	res += " ";
	res += (this->_password.empty() ? "" : this->_password);
	res += (this->_maxUsers == std::numeric_limits<size_t>::max() ? "" : to_string(this->_maxUsers));
	std::cout << "TEST :" << res << std::endl;
	return (res);
}

void Channel::_JOIN(User* user)
{
	this->_users.push_back(user);
	this->ft_sendAll(":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " JOIN " + this->_name + "\r\n");
	if (this->_topic.empty())
		user->ft_send(":" + user->getHostname() + " 331 " + this->_name + " " + this->_name + " :No topic is set\r\n");
	else
		user->ft_send(":" + user->getHostname() + " 332 " + user->getNickname() + " " + this->_name + " :" + this->_topic + "\r\n");
	user->ft_send(":" + user->getHostname() + " 353 " + user->getNickname() + " = " + this->_name + " :" + this->userList() + "\r\n");
	user->ft_send(":" + user->getHostname() + " 366 " + user->getNickname() + " " + this->_name + " :End of name list\r\n");
}

void Channel::leaveChannel(User* user, std::string mod, std::string msg)
{
	this->_users.erase(this->_users.begin() + user->findUserIn(this->_users));
	if (user->isIn(this->_admins))
		this->_admins.erase(this->_admins.begin() + user->findUserIn(this->_admins));

	if (this->_admins.size() == 0 && this->_users.size() > 0)
	{
		this->_admins.push_back(this->_users[0]);
		this->ft_sendAll(":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " MODE " + this->_name + " +o " + this->_users[1]->getNickname() + "\r\n");
	}
	if (user == this->_owner && this->_admins.size() > 0)
		this->_owner = this->_admins[0];
	user->ft_send(":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " " + mod + " " + this->_name + " " + msg + "\r\n");
	this->ft_sendAll(":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " " + mod + " " + this->_name + " " + msg + "\r\n");
}
