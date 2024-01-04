/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ennollet <ennollet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:01:51 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/04 13:31:24 by ennollet         ###   ########.fr       */
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
	return (0);
}

int Server::findUser(std::string user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]._nickname == user)
			return (i);
	}
	return (-1);
}

int Server::findChannel(std::string channel)
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
	this->_commands.insert(std::pair<std::string, cmdFunction>("PRIVMSG", &Server::_PRIVMSG));
}

bool isAuthenticationFunction(std::string const& input)
{
	if (input == "CAP" || input == "PASS" || input == "NICK" || input == "USER")
		return (true);
	return (false);
}

void Server::execCommand(std::vector<std::string> command, User& user)
{
	std::cout << "[IDK]: ";
	for (size_t i = 0; i < command.size(); i++)
		std::cout << command[i] << " ";
	std::cout << std::endl;
	if (isAuthenticationFunction(command[0]) || user.isAuthentified())
	{
		std::map<std::string, cmdFunction>::iterator it = this->_commands.find(command[0]);
		if (it != this->_commands.end())
			(this->*(it->second))(command, user);
		else
			std::cout << "Invalid command" << std::endl;
	}
	else
		user.ft_send("You are not yet registered\r\n");
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
			throw ft_exception("Wrong password");
	}
	else
	{	
		user.ft_send("Your password is " + user._password + "\r\n");
		std::cout << "Your password is " << user._password << std::endl;
	}
}

void Server::_NICK(std::vector<std::string>& command, User& user)
{
	if (user._password.empty())
		throw ft_exception("Connection closed");
	if (command.size() > 1)
	{
		if (command[1][0] == '&' || command[1][0] == '#')
		{	
			std::string errMsg = "Your nickname can't begin with '#' or '&' !";
			std::cout << errMsg << std::endl;
			user.ft_send(errMsg += "\r\n");
		}
		else
		{
			std::string msg = ":" + user._nickname;
			user._nickname = command[1];
			user.ft_send(msg += " NICK " + user._nickname + "\r\n");
		}
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
	{
		// if (command.size() > 1)
		if (!user._username.empty())
			user.ft_send("462 " + user._nickname + " :You may not reregister\r\n");
		else
		{
		user._username = command[1];
		user.ft_send("001 " + user._nickname + " :Welcome\n");
		std::cout << "Someone is connected . . ." << std::endl;
		}
	}
	else
		user.ft_send("461 " + user._nickname + " " +  command[0] + " :Not enough parameters\r\n");
}

void Server::_PING(std::vector<std::string>& command, User& user)
{
	if (command.size() > 1)
		user.ft_send("PONG " + command[1] + "\r\n");
	else
		user.ft_send("409 " + user._nickname + " :No origin specified\r\n");
}

void Server::_PRIVMSG(std::vector<std::string>& command, User& user)
{
	// this->_users[findUser(command[1])].ft_send(":" + user._nickname + " PRIVMSG " + this->_users[findUser(command[1])]._nickname + command[2] + "\r\n");
	if (command.size() > 2)
	{
		if (command[1][0] != '#' && command[1][0] != '&')
		{
			int i = findUser(command[1]);
			if (i != -1)
				this->_users[i].ft_send(":" + user._nickname + " PRIVMSG " + this->_users[findUser(command[1])]._nickname + " " + command[2] + "\r\n");
			else
				// std::cerr << "User not found" << std::endl;
				user.ft_send("411 " + user._nickname + " :User not found\n");
		}
		else
		{
			int i = findChannel(command[1]);
			if (i != -1)
				for (std::vector<User>::iterator it; it != this->_channels[i]._users.end(); it++)
					if (it->_nickname != user._nickname)
						it->ft_send(":" + user._nickname + " PRIVMSG " + it->_nickname + " " + command[2] + "\r\n");
		}
	}
	else
		user.ft_send("412 " + user._nickname + " : No text to send\r\n");
}
