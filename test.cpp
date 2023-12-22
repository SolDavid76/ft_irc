/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 14:33:03 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/22 15:28:02 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void User::readSocket(void)
{
	std::vector<char> buffer(512, 0);
	std::string res;
	int bytesRead = 0;

	bytesRead = recv(this->_socket, buffer.data(), buffer.size() - 1, 0);
	if (bytesRead == 0)
		throw ft_exception("Connection lost");
	this->_buffer += buffer.data();
	std::cerr << "#DEBUG: AFTER READ" << __LINE__ << std::endl;
}

int main(int ac, char** av)
{
	if (ac != 2)
	{
		std::cout << "Please use like this: " << av[0] << " <port>" << std::endl;
		return (0);
	}

	try
	{
		Server serv(atoi(av[1]));
		while (1)
		{
			poll(&serv._fds[0], serv._fds.size(), -1);

			if (serv._fds[0].revents & POLLIN)
			{
				User newuser(accept(serv._socket, NULL, NULL));
				if (newuser._socket == -1)
				{
					std::cout << "accept error" << std::endl;
					continue;
				}
				send(newuser._socket, "001 djanusz :Welcome\n", 21, 0);
				std::cout << "Someone is connected . . ." << std::endl;

				serv._fds.push_back(newuser._socket);
				serv._users.push_back(newuser);
				std::cerr << "#DEBUG IN MAIN: " << serv._users.back()._socket.fd << std::endl;
				std::cout << "Number of users : " << serv._fds.size() - 1 << std::endl;
			}
			for (size_t i = 1; i < serv._fds.size(); i++)
			{
				if (serv._fds[i].revents & POLLIN)
				{
					try
					{
						std::cerr << "#DEBUG IN MAIN: " << serv._users[i]._socket.fd << std::endl;
						serv._users[i].readSocket();
						if (serv._users[i]._buffer.find("\r\n"))
							std::cout << "[" << i << "]: \"" << serv._users[i]._buffer << "\"" << std::endl;
					}
					catch(std::exception const& e)
					{
						std::cerr << e.what() << std::endl;
						close(serv._fds[i].fd);
						serv._fds.erase(serv._fds.begin() + i);
						i--;
						continue;
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
