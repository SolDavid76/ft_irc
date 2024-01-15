/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ennollet <ennollet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 16:24:56 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/12 16:39:56 by ennollet         ###   ########.fr       */
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

bool User::operator==(User const& other)
{
	return (this->_id == other._id);
}

User::~User(void)
{
}

void User::disconect(void)
{
	std::cout << (this->_nickname.empty() ? "#" + to_string(this->_id) : this->_nickname) << " has been disconnected !" << std::endl;
	close(this->_socket.fd);
}

bool User::isAuthentified(void)
{
	if (!this->_irssi || this->_password.empty() || this->_nickname.empty() || this->_username.empty())
		return (false);
	return (true);
}

void User::ft_send(std::string msg)
{
	send(this->_socket.fd, msg.c_str(), msg.size(), 0);
}

bool User::isIn(std::vector<User> users)
{
	return (find(users.begin(), users.end(), *this) != users.end());
}

bool User::isIn(std::vector<User*> users)
{
	return (find(users.begin(), users.end(), this) != users.end());
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
