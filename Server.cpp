/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:01:51 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/12 18:25:29 by djanusz          ###   ########.fr       */
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

int Server::findUser(User* user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->_id == user->_id)
			return (i);
	}
	return (-1);
}

int Server::findUser(User& user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->_id == user._id)
			return (i);
	}
	return (-1);
}

int Server::findUser(std::string user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->_nickname == user)
			return (i);
	}
	return (-1);
}

int Server::findChannel(std::string channel)
{
	// std::cout << "name " << channel << std::endl;
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
	// std::cout << "channel_name " << this->_channels[i]._name << std::endl;
		if (this->_channels[i]._name == channel)
			return (i);
	}
	return (-1);
}

void Server::disconect(User* user)
{
	user->disconect();
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
	this->_commands.insert(std::pair<std::string, cmdFunction>("PART", &Server::_PART));
	this->_commands.insert(std::pair<std::string, cmdFunction>("KICK", &Server::_KICK));
	this->_commands.insert(std::pair<std::string, cmdFunction>("MODE", &Server::_MODE));
	this->_commands.insert(std::pair<std::string, cmdFunction>("INVITE", &Server::_INVITE));
	this->_commands.insert(std::pair<std::string, cmdFunction>("PRIVMSG", &Server::_PRIVMSG));
}

bool isAuthenticationFunction(std::string const& input)
{
	if (input == "CAP" || input == "PASS" || input == "NICK" || input == "USER")
		return (true);
	return (false);
}

void Server::execCommand(std::vector<std::string> command, User* user)
{
	std::cout << "[" << (user->_nickname.empty() ? "IDK" : user->_nickname) << "][";
	for (size_t i = 0; i < command.size(); i++)
		std::cout << command[i] << " ";
	std::cout << "]" << std::endl;
	if (isAuthenticationFunction(command[0]) || user->isAuthentified())
	{
		std::map<std::string, cmdFunction>::iterator it = this->_commands.find(command[0]);
		if (it != this->_commands.end())
			(this->*(it->second))(command, user);
		else
			std::cout << "Invalid command" << std::endl;
	}
	else
		user->ft_send("You are not yet registered\r\n");
}

void Server::_CAP(std::vector<std::string>& command, User* user)
{
	if (command.size() > 1 && command[1] == "LS")
		user->_irssi = true;
}

void Server::_PASS(std::vector<std::string>& command, User* user)
{
	if (!user->_irssi)
		throw ft_exception("Connection closed");
	if (command.size() > 1)
	{
		if (this->_password == command[1])
			user->_password = command[1];
		else
			throw ft_exception("Wrong password");
	}
	else
	{
		user->ft_send("Your password is " + user->_password + "\r\n");
		std::cout << "Your password is " << user->_password << std::endl;
	}
}

void Server::_NICK(std::vector<std::string>& command, User* user)
{
	if (user->_password.empty())
		throw ft_exception("Connection closed");
	if (command.size() > 1)
	{
		if (command[1][0] == '&' || command[1][0] == '#')
		{
			std::string errMsg = "Your nickname can't begin with '#' or '&' !";
			std::cout << errMsg << std::endl;
			user->ft_send(errMsg += "\r\n");
		}
		else
		{
			std::string msg = ":" + user->_nickname;
			user->_nickname = command[1];
			user->ft_send(msg += " NICK " + user->_nickname + "\r\n");
		}
		//oh sa mewe j'ai une idee de fou pour faire ces trois lignes en une seule.
	}
	else
		std::cout << "Your nickname is " << user->_nickname << std::endl;
}

void Server::_USER(std::vector<std::string>& command, User* user)
{
	if (user->_nickname.empty())
			throw ft_exception("Connection closed");
	if (command.size() >= 4)
	{
		if (!user->_username.empty())
			user->ft_send("462 " + user->_nickname + " :You may not reregister\r\n");
		else
		{
			user->_username = command[1];
			user->_hostname = command[3];
			user->ft_send(":" + user->_hostname + " 001 " + user->_nickname + " :Welcome\n");
			std::cout << "Someone is connected . . ." << std::endl;
		}
	}
	else
		user->ft_send("461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
}

void Server::_PING(std::vector<std::string>& command, User* user)
{
	if (command.size() > 1)
		user->ft_send("PONG " + command[1] + "\r\n");
	else
		user->ft_send("409 " + user->_nickname + " :No origin specified\r\n");
}

void Server::_PRIVMSG(std::vector<std::string>& command, User* user)
{
	// this->_users[findUser(command[1])].ft_send(":" + user._nickname + " PRIVMSG " + this->_users[findUser(command[1])]._nickname + command[2] + "\r\n");
	if (command.size() > 2)
	{
		if (command[1][0] != '#' && command[1][0] != '&')
		{
			int i = findUser(command[1]);
			if (i != -1)
				this->_users[i]->ft_send(":" + user->_nickname + " PRIVMSG " + this->_users[findUser(command[1])]->_nickname + " " + command[2] + "\r\n");
			else
				// std::cerr << "User not found" << std::endl;
				user->ft_send("411 " + user->_nickname + " :User not found\n");
		}
		else
		{
			int x = findChannel(command[1]);
			std::cout << "[Users in " << this->_channels[x]._name << "][";
			for (size_t i = 0; i < this->_channels[x]._users.size(); i++)
				std::cout << this->_channels[x]._users[i]->_nickname << " ";
			std::cout << "]" << std::endl;
			if (x != -1)
				for (size_t i = 0; i < this->_channels[x]._users.size(); i++)
					if (this->_channels[x]._users[i]->_nickname != user->_nickname)
						this->_channels[x]._users[i]->ft_send(":" + user->_nickname + " PRIVMSG " + this->_channels[x]._name + " " + command[2] + "\r\n");
		}
	}
	else
		user->ft_send("412 " + user->_nickname + " :No text to send\r\n");
}

void Server::_JOIN(std::vector<std::string>& command, User* user)
{
	if (command.size() == 1)
		user->ft_send(":" + user->_hostname + " 461 " + user->_nickname + " JOIN :Not enough parameters given\r\n");
	else
	{
		std::vector<std::string> args = ft_split(command[1], ',');
		std::vector<std::string> keys;
		if (command.size() >= 3)
			keys = ft_split(command[2], ',');
		for (size_t i = 0; i < args.size(); i++)
		{
			int x;
			std::string chan = (args[i][0] == '#' || args[i][0] == '&') ? args[i] : "#" + args[i];
			if ((x = this->findChannel(chan)) == -1)
				this->_channels.push_back(Channel(user, chan));
			else if (this->_channels[x]._invitationOnly && !user->isIn(this->_channels[x]._invited))
				user->ft_send(":" + user->_hostname + " 473 " + user->_nickname + " " + chan + " :You must be invited\r\n");
			else if (!this->_channels[x]._password.empty() && this->_channels[x]._password != (i < keys.size() ? keys[i] : ""))
				user->ft_send(":" + user->_hostname + " 475 " + user->_nickname + " " + chan + " :Bad channel key\r\n");
			else if (this->_channels[x]._users.size() >= this->_channels[x]._maxUsers)
				user->ft_send(":" + user->_hostname + " 471 " + user->_nickname + " " + chan + " :Channel is full\r\n");
			else if (!user->isIn(this->_channels[x]._users))
				this->_channels[x]._JOIN(user);
		}
	}
}

void Server::_PART(std::vector<std::string>& command, User* user)
{
	if (command.size() == 1)
		user->ft_send(":" + user->_hostname + " 461 " + user->_nickname + " PART :Not enough parameters given\r\n");
	else
	{
		std::vector<std::string> args = ft_split(command[1], ',');
		for (size_t i = 0; i < args.size(); i++)
		{
			int x;
			if ((x = this->findChannel(args[i])) == -1)
				user->ft_send(":" + user->_hostname + " 403 " + user->_nickname + " " + args[i] + " :No such channel\r\n");
			else if (!user->isIn(this->_channels[x]._users))
				user->ft_send(":" + user->_hostname + " 442 " + user->_nickname + " " + args[i] + " :You're not on that channel\r\n");
			else
			{
				this->_channels[x]._PART(user, (command.size() >= 3 ? command[2] : ":For no reason"));
				if (this->_channels[x]._users.size() == 0)
					this->_channels.erase(this->_channels.begin() + x);
			}
		}
	}
}

void Server::_INVITE(std::vector<std::string>& command, User* user)
{
	if (command.size() < 3)
		user->ft_send("461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
	else
	{
		int x;
		if (findUser(command[1]) == -1)
			user->ft_send(":401 " + command[2] + " :No such nick/channel\r\n");
		else if ((x = findChannel(command[2])) != -1)
		{
			int i;
			if 	(!user->isIn(this->_channels[x]._users))
				user->ft_send(":442 " + user->_nickname + " :You're not on that channel\r\n");
			else if ((i = findUser(command[1])))
			{
				if (this->_users[i]->isIn(this->_channels[x]._users))
					user->ft_send(":443 " + command[1] + " " + command[2] + ":is already on channel\r\n");
			}
			else if (i != -1 && this->_users[i]->isIn(this->_channels[x]._admins))
				user->ft_send(":482 " + command[2] + " :You're not channel operator\r\n");
			else
			{
				if (this->_users[i]->isIn(this->_channels[x]._invited) == 0)
					this->_channels[x]._invited.push_back(this->_users[i]);
				this->_users[i]->ft_send(":" + user->_nickname + " INVITE " + this->_users[i]->_nickname + " " + this->_channels[x]._name + "\r\n");
				for (size_t j = 0; j <  this->_channels[x]._invited.size(); j++)
					std::cout << this->_channels[x]._invited[j]->_nickname << std::endl;
				// user->ft_send(":341 " + command[2] + " " + command[1] + "\r\n");
				// user->ft_send(":301 " + command[1] + " is absent\r\n");
			}
		}
	}
}

void Server::_KICK(std::vector<std::string>& command, User* user)
{
	if (command.size() < 3)
		user->ft_send("461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
	else
	{
		int x;
		int i;
		if ((x = findChannel(command[1]) == -1))
			user->ft_send(":403 " + command[1] + ":No such channel\r\n");
		else if (user->isIn(this->_channels[x]._users) == 0)
			user->ft_send(":442 " + command[1] + " :You're not on that channel\r\n");
		else if (user->isIn(this->_channels[x]._admins) == 0)
			user->ft_send(":482 " + command[1] + " :You're not channel operator\r\n");
		else if ((i = this->_channels[x].findUser(command[2])) != -1)
		{
			this->_channels[x]._users[i]->ft_send(":" + user->_nickname + " " + command[0] + " " + command[1] + " " + command[2] + "\r\n");
			this->_channels[x]._users.erase(this->_channels[x]._users.begin() + this->_channels[x].findUser(command[2]));
		}
		else
			std::cout << "YaR" << std::endl;
	}
}

void Server::_MODE(std::vector<std::string>& command, User* user)
{
	if (command.size() < 3)
		user->ft_send("461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
	else
	{
		int x;
		if ((x = findChannel(command[1])) == -1)
			user->ft_send(":403 " + command[1] + " :No such channel\r\n");
		else if (user->isIn(this->_channels[x]._users) == 0)
			user->ft_send(":442 " + command[1] + " :You're not on that channel\r\n");
		else if (user->isIn(this->_channels[x]._admins) == 0)
			user->ft_send(":482 " + command[1] + " :You're not channel operator\r\n");
		else 
		{
			if (command[2][0] != '+' && command[2][0] != '-')
				std::cout << "errrrrrrrrrrr" << std::endl;
			else
			{
				bool option = command[2][0] == '+';
				size_t j = 1;
				for (size_t i = 1; i < command[2].size(); i++)
				{
					if (command[2][i] == 'i')
						this->_channels[x]._invitationOnly = option;
					else if (command[2][i] == 't')
						this->_channels[x]._topicCanBeChange = option;
					else if (command[2][i] == 'o')
					{
						if (2 + j <= command.size() - 1)
						{
						int y;
						if ((y = findUser(command[2 + j++])) != -1 && this->_users[y]->isIn(this->_channels[x]._users))
							{
							std::cout << "ici" << std::endl;

								if (option)
									this->_channels[x]._admins.push_back(this->_users[y]);
								else
									std::cout << "je dois remove" << std::endl;
							}
						else if (y == -1)
							user->ft_send(":" + user->_hostname + " 482 " + user->_nickname + " " + command[1] + " :User not on that channel\r\n");	
							// user->ft_send(":442 " + command[1] + " :User not on that channel\r\n");	
						}
						else
							user->ft_send(":461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
										
					}
					else if (command[2][i] == 'k')
					{
						if (2 + j <= command.size() - 1)
						{
							std::cout << "j + 2 " << j+2 << std::endl;
							std::cout << "command_size " << command.size() << std::endl;
							if (option)
							{
								if (this->_channels[x]._password == "")
									this->_channels[x]._password = command[2 + j++];
								else
									user->ft_send(":467 " + command[1] + " :Chanel key already set");
							}
							else
								{
									if (this->_channels[x]._password == command[2 + j++])
										this->_channels[x]._password = "";
									else
										std::cout << "MAUVAIS PASSWORD" << std::endl;
								}
						}
						else
							user->ft_send(":461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
					}
					else if (command[2][i] == 'l')
					{
						if (2 + j <= command.size() - 1)
						{
							if (option)
							{
								int limit = std::atoi(to_string(command[2 + j]).c_str());
								for (size_t i = 0; i < command[2 + j].size(); i++)
									if (!isdigit(command[2 + j][i]))
										limit = -1;		
								j++;		
								if (limit > 0)
								{
									this->_channels[x]._maxUsers = limit;
									user->ft_send(":" + user->_hostname + " 324 " + user->_nickname + " " + this->_channels[x]._name + " " + this->_channels[x].modsList() + " :SUCCES\r\n");

								}
								else
									std::cout << "ERROR invalid argument for user limit" << std::endl;
							}
							else
								this->_channels[x]._maxUsers = std::numeric_limits<size_t>::max();								
						}
						else
							user->ft_send(":461 " + user->_nickname + " " +  command[0] + " :Not enough parameters\r\n");
					}
						
				}
			}
					
		}
	}
}

