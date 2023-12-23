/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:01:51 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/23 21:25:11 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string password)
{
	sockaddr_in server_addr;

	this->_socket = socket(AF_INET, SOCK_STREAM, 0);
	this->_password = password;

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(this->_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		close(this->_socket);
		throw ft_exception("Port already used.");
	}

	pollfd fd = { this->_socket, POLLIN, 0 };
	this->_fds.push_back(fd);
	listen(this->_socket, 5);
}

Server::~Server(void)
{
}

void Server::disconect(User user)
{
	user.disconect();
	this->_fds.erase(this->_fds.begin() + user._id - 2);
	this->_users.erase(this->_users.begin() + user._id - 1);
}

void Server::initCommands(void)
{
	this->_commands.insert(std::pair<std::string, cmdFunction>("CAP", &Server::_CAP));
	this->_commands.insert(std::pair<std::string, cmdFunction>("PASS", &Server::_PASS));
	this->_commands.insert(std::pair<std::string, cmdFunction>("NICK", &Server::_NICK));
	this->_commands.insert(std::pair<std::string, cmdFunction>("USER", &Server::_USER));
}

void Server::execCommand(std::vector<std::string> command, User& user)
{
	std::map<std::string, cmdFunction>::iterator it = this->_commands.find(command[0]);
	std::cout << "[IDK]: ";
	for (size_t i = 0; i < command.size(); i++)
		std::cout << command[i] << " ";
	std::cout << std::endl;
	if (it != this->_commands.end())
		(this->*(it->second))(command, user);
}

void Server::_CAP(std::vector<std::string>& command, User& user)
{
	if (command[1] == "LS")
		user._irssi = true;
}

void Server::_PASS(std::vector<std::string>& command, User& user)
{
	(void)command;
	(void)user;
	std::cout << "PASS was detected" << std::endl;
}

void Server::_NICK(std::vector<std::string>& command, User& user)
{
	if (command.size() > 1)
		user._nickname = command[1] + "#" + to_string(user._id);
	else
		std::cout << "Your nickname is " << user._nickname << std::endl;
}

void Server::_USER(std::vector<std::string>& command, User& user)
{
	(void)command;
	(void)user;
	std::cout << "USER was detected" << std::endl;
}
