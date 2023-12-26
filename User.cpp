/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ennollet <ennollet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 16:24:56 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/26 13:12:39 by ennollet         ###   ########.fr       */
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
		this->_nickname = src._nickname;
		this->_username = src._username;
	}
	return (*this);
}

User::~User(void)
{
}

void User::disconect(void)
{
	std::cout << this->_nickname << " has been disconnected !" << std::endl;
	close(this->_socket.fd);
}

bool User::isAuthentified(void)
{
	// std::cout << "irssi : " << this->_irssi << std::endl;
	// std::cout << "password : " << this->_password << std::endl;
	// std::cout << "nickname : " << this->_nickname << std::endl;
	// std::cout << "username : " << this->_username << std::endl;
	if (!this->_irssi || this->_password.empty() || this->_nickname.empty() || this->_username.empty())
		return (false);
	return (true);
}
