/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:01:51 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/18 14:15:23 by djanusz          ###   ########.fr       */
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
	close(this->_socket);
	for (size_t i = 0; i < this->_users.size(); i++)
		delete (this->_users[i]);
}

int Server::findUser(User* user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->getId() == user->getId())
			return (i);
	}
	return (-1);
}

int Server::findUser(std::string user)
{
	for (size_t i = 0; i < this->_users.size(); i++)
	{
		if (this->_users[i]->getNickname() == user)
			return (i);
	}
	return (-1);
}

int Server::findChannel(std::string channel)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (this->_channels[i].getName() == channel)
			return (i);
	}
	return (-1);
}

void Server::disconect(User* user, std::string msg)
{
	user->ft_send(msg + "\r\n");
	this->_fds.erase(this->_fds.begin() + this->findUser(user) + 1);
	this->_users.erase(this->_users.begin() + this->findUser(user));
	delete (user);
	throw ft_exception("Someone disconnected ! (" + msg + ")");
}

void Server::broadcast(User* user, std::string msg)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (user->isIn(this->_channels[i].getUsers()))
			this->_channels[i].ft_sendAll(user, msg);
	}
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
	this->_commands.insert(std::pair<std::string, cmdFunction>("TOPIC", &Server::_TOPIC));
	this->_commands.insert(std::pair<std::string, cmdFunction>("KICK", &Server::_KICK));
	this->_commands.insert(std::pair<std::string, cmdFunction>("MODE", &Server::_MODE));
	this->_commands.insert(std::pair<std::string, cmdFunction>("INVITE", &Server::_INVITE));
	this->_commands.insert(std::pair<std::string, cmdFunction>("PRIVMSG", &Server::_PRIVMSG));
	this->_commands.insert(std::pair<std::string, cmdFunction>("QUIT", &Server::_QUIT));
}

bool isAuthenticationFunction(std::string const& input)
{
	return (input == "CAP" || input == "PASS" || input == "NICK" || input == "USER" || input == "QUIT");
}

void Server::execCommand(std::vector<std::string> command, User* user)
{
	std::cout << (user->getNickname().empty() ? "#" + to_string(user->getId()) : user->getNickname()) << ": ";
	for (size_t i = 0; i < command.size(); i++)
		std::cout << command[i] << (i != command.size() - 1 ? " " : "");
	std::cout << ";" << std::endl;

	user->setBuffer(user->getBuffer().substr(user->getBuffer().find("\r\n") + 2, user->getBuffer().size()));
	if (command.size() != 0 && (isAuthenticationFunction(command[0]) || user->isAuthentified()))
	{
		std::map<std::string, cmdFunction>::iterator it = this->_commands.find(command[0]);
		if (it != this->_commands.end())
			(this->*(it->second))(command, user);
	}
}

void Server::_CAP(std::vector<std::string>& command, User* user)
{
	if (command.size() > 1 && command[1] == "LS")
		user->setIrssi(true);
}

void Server::_PASS(std::vector<std::string>& command, User* user)
{
	if (!user->getIrssi())
		this->disconect(user, "No CAP LS");
	else if (command.size() == 1)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " PASS :Not enough parameters given\r\n");
	else if (!user->getPassword().empty())
		user->ft_send(":" + user->getHostname() + " 462 " + user->getNickname() + " :Unauthorized command (already registered)\r\n");
	else if (this->_password == command[1])
		user->setPassword(command[1]);
	else
		this->disconect(user, "Wrong password");
}

void Server::_NICK(std::vector<std::string>& command, User* user)
{
	if (user->getPassword().empty())
		this->disconect(user, "No password");
	else if (command.size() == 1)
		user->ft_send("Your nickname is " + user->getNickname() + "\r\n");
	else if (command[1][0] == '&' || command[1][0] == '#')
		user->ft_send("Your nickname can't begin with '#' or '&' !\r\n");
	else if (this->findUser(command[1]) != -1 || command[1] == "Jarvis")
		user->ft_send(":" + user->getHostname() + " 433 * " + command[1] + " :Nickname is already in use\r\n");
	else
	{
		user->ft_send(":" + user->getNickname() + " NICK " + command[1] + "\r\n");
		this->broadcast(user, ":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " NICK " + command[1] + "\r\n");
		user->setNickname(command[1]);
	}
}

void Server::_USER(std::vector<std::string>& command, User* user)
{
	if (user->getNickname().empty())
		this->disconect(user, "Wrong nickname");
	if (command.size() >= 5)
	{
		if (!user->getUsername().empty())
			user->ft_send(":" + user->getHostname() + " 462 " + user->getNickname() + " :Unauthorized command (already registered)\r\n");
		else
		{
			user->setUsername(command[1]);
			user->setHostname(command[3]);
			user->ft_send(":" + user->getHostname() + " 001 " + user->getNickname() + " :Welcome\r\n");
			std::cout << user->getNickname() + " connected !" << std::endl;
		}
	}
	else
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " USER :Not enough parameters given\r\n");
}

void Server::_PING(std::vector<std::string>& command, User* user)
{
	if (command.size() > 1)
		user->ft_send("PONG " + command[1] + "\r\n");
	else
		user->ft_send(":" + user->getHostname() + " 409 " + user->getNickname() + " :No origin specified\r\n");
}

void Server::_PRIVMSG(std::vector<std::string>& command, User* user)
{
	if (command.size() > 2)
	{
		if (command[1][0] != '#' && command[1][0] != '&')
		{
			int i = findUser(command[1]);
			if (i != -1)
				this->_users[i]->ft_send(":" + user->getNickname() + "@" + user->getHostname() + " PRIVMSG " + this->_users[findUser(command[1])]->getNickname() + " " + command[2] + "\r\n");
			else if (command[1] == "Jarvis")
				this->jarvis(command, user);
			else
				user->ft_send(":" + user->getHostname() + " 411 " + user->getNickname() + " :User not found\n");
		}
		else
		{
			int x = findChannel(command[1]);
			if (x != -1 && user->isIn(this->_channels[x].getUsers()))
				this->_channels[x].ft_sendAll(user, ":" + user->getNickname() + "@" + user->getHostname() + " PRIVMSG " + this->_channels[x].getName() + " " + command[2] + "\r\n");
			else
			{
				if (x == -1)
					user->ft_send(":" + user->getHostname() + " 403 " + user->getNickname() + " " + command[1] + " :No such channel\r\n");
				else
					user->ft_send(":" + user->getHostname() + " 442 " + user->getNickname() + " " + command[1] + " :You're not on that channel\r\n");

			}
		}
	}
	else
		user->ft_send(":" + user->getHostname() + " 412 " + user->getNickname() + " :No text to send\r\n");
}

void Server::jarvis(std::vector<std::string>& command, User* user)
{
	if (command[2] == "!help")
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :There is a list of my features: !users, !quoi, !meow, !coinflip, !stark.\r\n");
	else if (command[2] == "!users")
	{
		std::string users;
		for (size_t i = 0; i < this->_users.size(); i++)
			users += this->_users[i]->getNickname() + (i != this->_users.size() - 1 ? ", " : "");
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :There is a list of users on the server: " + users + "\r\n");
	}
	else if (command[2] == "!quoi")
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :FEUR\r\n");
	else if (command[2] == "!meow")
	{
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :  /\\_/\\\r\n");
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " : ( o.o )\r\n");
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :  > ^ <\r\n");
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " : MEEEOWW\r\n");
	}
	else if (command[2] == "!coinflip")
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :" + (std::rand() % 2 ? "Heads\r\n" : "Tails\r\n"));
	else if (command[2] == "!stark")
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :I'm at your service, boss\r\n");
	else
		user->ft_send(":Jarvis@STARK.INC PRIVMSG " + user->getNickname() + " :Get some help with !help\r\n");
}

void Server::_JOIN(std::vector<std::string>& command, User* user)
{
	if (command.size() == 1)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " JOIN :Not enough parameters given\r\n");
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
			else if (this->_channels[x].getInvitationOnly() && !user->isIn(this->_channels[x].getInvitedUsers()))
				user->ft_send(":" + user->getHostname() + " 473 " + user->getNickname() + " " + chan + " :You must be invited\r\n");
			else if (!this->_channels[x].getPassword().empty() && this->_channels[x].getPassword() != (i < keys.size() ? keys[i] : ""))
				user->ft_send(":" + user->getHostname() + " 475 " + user->getNickname() + " " + chan + " :Bad channel key\r\n");
			else if (this->_channels[x].getUsers().size() >= this->_channels[x].getMaxUsers())
				user->ft_send(":" + user->getHostname() + " 471 " + user->getNickname() + " " + chan + " :Channel is full\r\n");
			else if (!user->isIn(this->_channels[x].getUsers()))
				this->_channels[x]._JOIN(user);
		}
	}
}

void Server::_PART(std::vector<std::string>& command, User* user)
{
	if (command.size() == 1)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " PART :Not enough parameters given\r\n");
	else
	{
		std::vector<std::string> args = ft_split(command[1], ',');
		for (size_t i = 0; i < args.size(); i++)
		{
			int x;
			if ((x = this->findChannel(args[i])) == -1)
				user->ft_send(":" + user->getHostname() + " 403 " + user->getNickname() + " " + args[i] + " :No such channel\r\n");
			else if (!user->isIn(this->_channels[x].getUsers()))
				user->ft_send(":" + user->getHostname() + " 442 " + user->getNickname() + " " + args[i] + " :You're not on that channel\r\n");
			else
			{
				this->_channels[x].leaveChannel(user, "PART", (command.size() > 2 ? command[2] : ":For no reason"));
				if (this->_channels[x].getUsers().size() == 0)
					this->_channels.erase(this->_channels.begin() + x);
			}
		}
	}
}

void Server::_TOPIC(std::vector<std::string>& command, User* user)
{
	if (command.size() == 1)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " TOPIC :Not enough parameters given\r\n");
	else
	{
		int x;
		if ((x = this->findChannel(command[1]) == -1))
			user->ft_send(":" + user->getHostname() + " 403 " + user->getNickname() + " " + command[1] + " :No such channel\r\n");
		else if (!user->isIn(this->_channels[x].getUsers()))
			user->ft_send(":" + user->getHostname() + " 442 " + user->getNickname() + " " + command[1] + " :You're not on that channel\r\n");
		else if (command.size() == 2)
			user->ft_send(":" + user->getHostname() + " " + (this->_channels[x].getTopic().empty() ? "331" : "332") + " " + user->getNickname() + " " + this->_channels[x].getName() + " :" + (this->_channels[x].getTopic().empty() ? "No topic is set" : this->_channels[x].getTopic()) + "\r\n");
		else if (this->_channels[x].getTopicChangeable() && !user->isIn(this->_channels[x].getAdmins()))
			user->ft_send(":" + user->getHostname() + " 482 " + user->getNickname() + " " + command[1] + " :You're not channel operator\r\n");
		else
		{
			this->_channels[x].setTopic(command[2]);
			this->_channels[x].ft_sendAll(":" + user->getNickname() + "!" + user->getUsername() + "@" + user->getHostname() + " TOPIC " + this->_channels[x].getName() + " :" + command[2] + "\r\n");
		}
	}
}

void Server::_QUIT(std::vector<std::string>& command, User* user)
{
	for (size_t i = 0; i < this->_channels.size(); i++)
	{
		if (user->isIn(this->_channels[i].getUsers()))
		{
			this->_channels[i].leaveChannel(user, "PART", (command.size() > 1 ? command[1] : ":For no reason"));
			if (this->_channels[i].getUsers().size() == 0)
					this->_channels.erase(this->_channels.begin() + i);
		}
	}
	std::string msg = user->getNickname() + " disconnected !";
	this->_fds.erase(this->_fds.begin() + this->findUser(user) + 1);
	this->_users.erase(this->_users.begin() + this->findUser(user));
	delete (user);
	throw ft_exception(msg);
}

void Server::_INVITE(std::vector<std::string>& command, User* user)
{
	if (command.size() < 3)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " " + command[0] + " :Not enough parameters\r\n");
	else
	{
		int x;
		if (findUser(command[1]) == -1)
			user->ft_send(":401 " + command[2] + " :No such nick/channel\r\n");
		else if ((x = findChannel(command[2])) != -1)
		{
			int i = findUser(command[1]);
			if 	(!user->isIn(this->_channels[x].getUsers()))
				user->ft_send(":" + user->getHostname() + " 442 " + command[2] + " " + user->getNickname() + " :You're not on that channel\r\n");
			else if (this->_users[i]->isIn(this->_channels[x].getUsers()))
					user->ft_send(":" + user->getHostname() + " 443 "+ command[1] + " " + command[2] + " :is already on channel\r\n");
			else if (i != -1 && !user->isIn(this->_channels[x].getAdmins()))
				user->ft_send(":" + user->getHostname() + " 482 " + command[2] +  " " + user->getNickname() +  " :You're not channel operator\r\n");
			else
			{
				if (this->_users[i]->isIn(this->_channels[x].getInvitedUsers()) == 0)
					this->_channels[x].addInvitedUsers(this->_users[i]);
				this->_users[i]->ft_send(":" + user->getNickname() + + "@" + user->getHostname() + " INVITE " + this->_users[i]->getNickname() + " " + this->_channels[x].getName() + "\r\n");

				for (size_t j = 0; j <  this->_channels[x].getInvitedUsers().size(); j++)
					std::cout << this->_channels[x].getInvitedUsers()[j]->getNickname() << std::endl;
				user->ft_send(":" + user->getHostname() + " 341 " + user->getNickname() + " " + command[2] + " :" + command[1] + "\r\n");
			}
		}
	}
}

void Server::_KICK(std::vector<std::string>& command, User* user)
{
	if (command.size() < 3)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " " + command[0] + " :Not enough parameters\r\n");
	else
	{
		int x;
		int i;
		if ((x = findChannel(command[1]) == -1))
			user->ft_send(":" + user->getHostname() + " 403 " + ":No such channel\r\n");
		else if (user->isIn(this->_channels[x].getUsers()) == 0)
			user->ft_send(":" + user->getHostname() + " 442 " + command[1] +  " " + user->getNickname() + " :You're not on that channel\r\n");
		else if (user->isIn(this->_channels[x].getAdmins()) == 0)
			user->ft_send(":" + user->getHostname() + " 482 " + command[1] +  " " + user->getNickname() +  " :You're not channel operator\r\n");
		else if ((i = this->_channels[x].findUser(command[2])) != -1 && !this->_channels[x].getUsers()[i]->isIn(this->_channels[x].getAdmins()))
		{
			this->_channels[x].ft_sendAll(":" + user->getNickname() + + "@" + user->getHostname() + " KICK " + " " + command[1] + " " + command[2] + "\r\n");
			this->_channels[x].getUsers().erase(this->_channels[x].getUsers().begin() + this->_channels[x].findUser(command[2]));
		}
		else if (i == -1)
			user->ft_send(":" + user->getHostname() + " 442 " + command[1] + " " + user->getNickname() + " :User not on that channel\r\n");
		else
			user->ft_send(":" + user->getHostname() + " 4242 " + command[1] + " " + user->getNickname() + " :You can't kick an operator\r\n");
	}
}

void Server::_MODE(std::vector<std::string>& command, User* user)
{
	int y;
	if (command.size() < 2)
		user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " " + command[0] + " :Not enough parameters\r\n");
	else if ((y = findUser(command[1])) != -1)
		return;
	else if (command.size() >= 3)
	{
		int x;
		if ((x = findChannel(command[1])) == -1)
			user->ft_send(":" + user->getHostname() + " 403 " + user->getNickname() + " " + command[1] + " :No such channel\r\n");	
		else if (user->isIn(this->_channels[x].getUsers()) == 0)
			user->ft_send(":" + user->getHostname() + " 442 " + user->getNickname() + " " + command[1] + " :You're not on that channel\r\n");	
		else if (user->isIn(this->_channels[x].getAdmins()) == 0)
			user->ft_send(":" + user->getHostname() + " 482 " + user->getNickname() + " " + command[1] + " :You're not channel operator\r\n");
		else 
		{
			if (command[2][0] != '+' && command[2][0] != '-')
				user->ft_send(":" + user->getHostname() + " 4242 " + command[1] + " " + user->getNickname() + " :Invalid format need '+' or '-' before flags\r\n");
			else
			{
				bool option = command[2][0] == '+';
				size_t j = 1;
				for (size_t i = 1; i < command[2].size(); i++)
				{
					if (command[2][i] == '+' || command[2][i] == '-')
						option = command[2][i] == '+';
					else if (command[2][i] == 'i')
					{
						if (!option == this->_channels[x].getInvitationOnly())
							this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + (option ? " +" : " -") + "i" + "\r\n");

						this->_channels[x].setInvitationOnly(option);
					}
					else if (command[2][i] == 't')
					{
						if (!option == this->_channels[x].getTopicChangeable())
							this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + (option ? " +" : " -") + "t" + "\r\n");
						this->_channels[x].setTopicChangeable(option);
					}
					else if (command[2][i] == 'o')
					{
						if (2 + j <= command.size() - 1)
						{
						int y;
						if ((y = findUser(command[2 + j++])) != -1 && this->_users[y]->isIn(this->_channels[x].getUsers()))
							{
								if (option && !this->_users[y]->isIn(this->_channels[x].getAdmins()))
								{
									this->_channels[x].addAdmins(this->_users[y]);
									this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + " +o :" + this->_users[y]->getNickname() + "\r\n");
								}
								else if (option)
									user->ft_send(":" + user->getHostname() + " 4242 " + command[1] + " " + user->getNickname() + " :is already an operator\r\n");
								else if (this->_users[y] != this->_channels[x].getOwner() && this->_users[y]->isIn(this->_channels[x].getAdmins()) && user->getId() == this->_channels[x].getOwner()->getId())
								{
									this->_channels[x].getAdmins().erase(this->_channels[x].getAdmins().begin() + this->_users[y]->findUserIn(this->_channels[x].getAdmins()));
									this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + " -o :" + this->_users[y]->getNickname() + "\r\n");
								}
								else if (!this->_users[y]->isIn(this->_channels[x].getAdmins()))
									user->ft_send(":" + user->getHostname() + " 4242 " + command[1] + " " + user->getNickname() + " :You can't downgrade this user, he don't have privilege\r\n");
								else if (this->_users[y]->isIn(this->_channels[x].getAdmins()))
									user->ft_send(":" + user->getHostname() + " 4242 " + command[1] + " " + user->getNickname() + " :You can't dowwngrade this user, you have the same privilege \r\n");
							}
						else if (y == -1)
							user->ft_send(":" + user->getHostname() + " 442 " + user->getNickname() + " " + command[1] + " :User not on that channel\r\n");	
						}
						else
							user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " " + command[0] + " :Not enough parameters\r\n");
					}
					else if (command[2][i] == 'k')
					{
						if (2 + j <= command.size() - 1)
						{
							if (option)
							{
								if (this->_channels[x].getPassword() == "")
								{
									this->_channels[x].setPassword(command[2 + j++]);
									this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + " +k :" + "\r\n");
								}
								else
									user->ft_send(":" + user->getHostname() + " 467 " + command[1] + " " + user->getNickname() + " :Channel key already set\r\n");

							}
							else
								{
									if (this->_channels[x].getPassword() == command[2 + j++])
									{
										this->_channels[x].setPassword("");
										this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + " -k :" + "\r\n");
									}
									else
										user->ft_send(":" + user->getHostname() + " 464 " + command[1] + " " + user->getNickname() + " :Wrong password\r\n");

								}
						}
						else
							user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " " + command[0] + " :Not enough parameters\r\n");
					}
					else if (command[2][i] == 'l')
					{
						if ((2 + j <= command.size() - 1))
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
									this->_channels[x].setMaxUsers(limit);
									this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + " +l :" + to_string(limit) + "\r\n");
								}
								else
									user->ft_send(":" + user->getHostname() + " 4242 " + command[1] + " " + user->getNickname() + " :invalid argument for user limit\r\n");
							}
						}
						else if (!option)
						{
							this->_channels[x].setMaxUsers(std::numeric_limits<size_t>::max());
							this->_channels[x].ft_sendAll(":" + user->getNickname() + "@" + user->getHostname() + " MODE " + this->_channels[x].getName() + " -l\r\n");
						}
						else
							user->ft_send(":" + user->getHostname() + " 461 " + user->getNickname() + " " + command[0] + " :Not enough parameters\r\n");
					}
					else
						user->ft_send(":" + user->getHostname() + " 501 " + command[1] + " " + user->getNickname() + " :Unknown MODE " + command[2][i] + "\r\n");
				}
			}
		}
	}
}

