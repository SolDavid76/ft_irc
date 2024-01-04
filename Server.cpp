/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:01:51 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/04 16:07:14 by djanusz          ###   ########.fr       */
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

	pollfd fd = {this->_socket, POLLIN, 0};
	this->_fds.push_back(fd);
	listen(this->_socket, 5);

	this->initCommands();
}

Server::~Server(void)
{
}

int Server::findUser(User const& user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]._id == user._id)
			return (i);
	}
	return (-1);
}

int Server::findChannel(std::string const& channel)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i]._name == channel)
			return (i);
	}
	return (-1);
}

void Server::disconect(User user)
{
	user.disconect();
	this->_fds.erase(this->_fds.begin() + this->findUser(user) + 1);
	this->_users.erase(this->_users.begin() + this->findUser(user));
}

void Server::initCommands(void)
{
	this->_commands.insert(std::pair<std::string, cmdFunction>("CAP", &Server::_CAP));
	this->_commands.insert(std::pair<std::string, cmdFunction>("PASS", &Server::_PASS));
	this->_commands.insert(std::pair<std::string, cmdFunction>("NICK", &Server::_NICK));
	this->_commands.insert(std::pair<std::string, cmdFunction>("USER", &Server::_USER));
	this->_commands.insert(std::pair<std::string, cmdFunction>("PING", &Server::_PING));
	this->_commands.insert(std::pair<std::string, cmdFunction>("JOIN", &Server::_JOIN));
}

bool isAuthenticationFunction(std::string const& input)
{
	if (input == "CAP" || input == "PASS" || input == "NICK" || input == "USER")
		return (true);
	return (false);
}

void Server::execCommand(std::vector<std::string> command, User& user)
{
	std::cout << "[" << (user._nickname.empty() ? "IDK" : user._nickname) << "][";
	for (size_t i = 0; i < command.size(); i++)
		std::cout << command[i] << " ";
	std::cout << "]" << std::endl;
	if (isAuthenticationFunction(command[0]) || user.isAuthentified())
	{
		std::map<std::string, cmdFunction>::iterator it = this->_commands.find(command[0]);
		if (it != this->_commands.end())
			(this->*(it->second))(command, user);
		else
			std::cout << "Invalid command" << std::endl;
	}
	else
		std::cout << "You are not yet registered" << std::endl;
}

void Server::_CAP(std::vector<std::string>& command, User& user)
{
	if (command.size() > 1 && command[1] == "LS")
		user._irssi = true;
}

void Server::_PASS(std::vector<std::string>& command, User& user)
{
	if (!user._irssi)
		throw ft_exception("Connection closed");
	if (command.size() > 1)
	{
		if (this->_password == command[1])
			user._password = command[1];
		else
			throw ft_exception("Connection closed");
	}
	else
		std::cout << "Your password is " << user._password << std::endl;
}

void Server::_NICK(std::vector<std::string>& command, User& user)
{
	if (user._password.empty())
		throw ft_exception("Connection closed");
	if (command.size() > 1)
	{
		std::string msg = ":" + user._nickname;
		user._nickname = command[1];
		user.ft_send(msg += " NICK " + user._nickname + "\r\n");
		//oh sa mewe j'ai une idee de fou pour faire ces trois lignes en une seule.
	}
	else
		std::cout << "Your nickname is " << user._nickname << std::endl;
}

void Server::_USER(std::vector<std::string>& command, User& user)
{
	if (user._nickname.empty())
		throw ft_exception("Connection closed");
	if (command.size() > 1)
		user._username = command[1];
	user.ft_send("001 " + user._nickname + " :Welcome\n");
	std::cout << "Someone is connected . . ." << std::endl;
}

void Server::_PING(std::vector<std::string>& command, User& user)
{
	user.ft_send("PONG " + command[1] + "\r\n");
}

void Server::_JOIN(std::vector<std::string>& command, User& user)
{
	if (command.size() == 1)
		user.ft_send("Not enough parameters given\r\n");
	else
	{
		std::vector<std::string> args = ft_split(*(command.begin() + 1), ',');
		std::vector<std::string> keys;
		if (command.size() == 3)
			keys = ft_split(*(command.begin() + 2), ',');
		for (size_t i = 0; i < args.size(); i++)
		{
			int x;
			if ((x = findChannel((args[i][0] == '#' || args[i][0] == '&') ? args[i] : "#" + args[i])) != -1 || (x = findChannel((args[i][0] == '#' || args[i][0] == '&') ? args[i] : "#" + args[i])) != -1)
			{
				if (!this->_channels[x]._invitationOnly || user.isIn(this->_channels[x]._invited))
				{
					// if (i > keys.size())
					// 	keys.push_back("");
					if (this->_channels[x]._password.empty() || (this->_channels[x]._password == keys[i]))
						this->_channels[x]._users.push_back(user);
					else
						user.ft_send("Wrong password\r\n");
				}
			}
			else
				this->_channels.push_back(Channel((args[i][0] == '#' || args[i][0] == '&') ? args[i] : "#" + args[i]));
		}
		std::cout << "[channels][";
		for (size_t i = 0; i < this->_channels.size(); i++)
			std::cout << this->_channels[i]._name << " ";
		std::cout << "]" << std::endl;
	}
}