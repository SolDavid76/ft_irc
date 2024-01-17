/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 14:33:03 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/17 23:06:48 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void handler(int signal)
{
	if (signal == SIGINT)
		throw ft_exception("Closing the server !");
	if (signal == SIGQUIT)
		throw ft_exception("Closing the server !");
	(void)signal;
}

void User::readSocket(void)
{
	std::vector<char> tmp(512, 0);

	if (recv(this->_socket.fd, tmp.data(), tmp.size() - 1, 0) == 0)
		this->_buffer.append("QUIT\r\n");
	this->_buffer.append(tmp.data());
}

int main(int ac, char** av)
{
	if (ac != 3)
	{
		std::cout << "Please use like this: " << av[0] << " <port> <password>" << std::endl;
		return (0);
	}

	signal(SIGINT, handler);
	signal(SIGQUIT, handler);

	try
	{
		Server serv(atoi(av[1]), av[2]);
		while (1)
		{
			poll(&serv._fds[0], serv._fds.size(), -1);

			if (serv._fds[0].revents & POLLIN)
			{
				User* newUser = new User(accept(serv._socket, NULL, NULL));
				if (newUser->_socket.fd == -1)
				{
					std::cerr << "accept error" << std::endl;
					delete (newUser);
					continue;
				}
				serv._fds.push_back(newUser->_socket);
				serv._users.push_back(newUser);
			}
			for (size_t i = 1; i < serv._fds.size(); i++)
			{
				if (serv._fds[i].revents & POLLIN)
				{
					try
					{
						serv._users[i - 1]->readSocket();
						while (serv._users[i - 1]->_buffer.find("\r\n") != std::string::npos) //ptet mettre ca dans readSocket ?
							serv.execCommand(ft_split(serv._users[i - 1]->_buffer), serv._users[i - 1]);
					}
					catch(std::exception const& e)
					{
						std::cerr << e.what() << std::endl;
					}
				}
			}
		}
	}
	catch(std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
