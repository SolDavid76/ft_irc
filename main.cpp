/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 14:33:03 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/26 10:18:49 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void handler(int signal)
{
	if (signal == SIGINT)
		exit(0);
	if (signal == SIGQUIT)
		exit(0);
}

void User::readSocket(void)
{
	std::vector<char> tmp(512, 0);

	if (recv(this->_socket.fd, tmp.data(), tmp.size() - 1, 0) == 0)
		throw ft_exception("Connection closed");
	this->_buffer += tmp.data();
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
				User newuser(accept(serv._socket, NULL, NULL));
				if (newuser._socket.fd == -1)
				{
					std::cout << "accept error" << std::endl;
					continue;
				}
				serv._fds.push_back(newuser._socket);
				serv._users.push_back(newuser);

				send(newuser._socket.fd, "001 djanusz :Welcome\n", 21, 0);
				std::cout << "Someone is connected . . ." << std::endl;
				std::cout << "Number of users : " << serv._users.size() << std::endl;
			}
			for (size_t i = 1; i < serv._fds.size(); i++)
			{
				if (serv._fds[i].revents & POLLIN)
				{
					try
					{
						serv._users[i - 1].readSocket();
						size_t pos;
						while ((pos = serv._users[i - 1]._buffer.find("\r\n")) != std::string::npos) //ptet mettre ca dans readSocket ?
						{
							// std::cout << "#PRINT{" << serv._users[i - 1]._buffer.substr(0, pos) << "}" << std::endl;
							serv.execCommand(ft_split(serv._users[i - 1]._buffer), serv._users[i - 1]);
							serv._users[i - 1]._buffer.erase(0, pos + 2);
						}
					}
					catch(std::exception const& e)
					{
						std::cerr << e.what() << std::endl;
						serv.disconect(serv._users[i - 1]);
						i--;
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
