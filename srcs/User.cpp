/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 16:24:56 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/19 14:56:42 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

int User::nextId = 1;

User::User(int socket)
{
	this->_id = nextId++;
	this->_irssi = false;
	this->_socket.fd = socket;
	this->_socket.events = POLLIN;
	this->_socket.revents = 0;
	this->_password = "";
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
		this->_id = src._id;
		this->_irssi = src._irssi;
		this->_socket = src._socket;
		this->_hostname = src._hostname;
		this->_password = src._password;
		this->_nickname = src._nickname;
		this->_username = src._username;
		this->_buffer = src._buffer;
	}
	return (*this);
}

User::~User(void)
{
	close(this->_socket.fd);
}

void User::readSocket(void)
{
	std::vector<char> tmp(512, 0);

	if (recv(this->_socket.fd, tmp.data(), tmp.size() - 1, 0) == 0)
		this->_buffer = "QUIT\r\n";
	this->_buffer.append(tmp.data());
	std::cout << (this->_nickname.empty() ? "#" + to_string(this->_id) : this->_nickname) << ": " << tmp.data() << std::endl;
}

bool User::isAuthentified(void)
{
	return (this->_irssi && !this->_password.empty() && !this->_nickname.empty() && !this->_username.empty());
}

void User::ft_send(std::string msg)
{
	send(this->_socket.fd, msg.c_str(), msg.size(), 0);
}

bool User::isIn(std::vector<User*> users)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (this->_id == users[i]->_id)
			return (true);
	}
	return (false);
}

int User::findUserIn(std::vector<User*> users)
{
	for (size_t i = 0; i < users.size(); i++)
	{
		if (this->_id == users[i]->_id)
			return (i);
	}
	return (-1);
}
