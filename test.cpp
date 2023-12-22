/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 14:33:03 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/22 16:47:08 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void User::readSocket(void)
{
	std::vector<char> buffer(512, 0);
	std::string res;
	int bytesRead = 0;

	bytesRead = recv(this->_socket.fd, buffer.data(), buffer.size() - 1, 0);
	if (bytesRead == 0)
		throw ft_exception("Connection closed");
	this->_buffer.append(buffer.data());
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
				if (newuser._socket.fd == -1)
				{
					std::cout << "accept error" << std::endl;
					continue;
				}
				send(newuser._socket.fd, "001 djanusz :Welcome\n", 21, 0);
				std::cout << "Someone is connected . . ." << std::endl;

				serv._fds.push_back(newuser._socket);
				serv._users.push_back(newuser);
				std::cout << "Number of users : " << serv._users.size() << std::endl;
			}
			for (size_t i = 1; i < serv._fds.size(); i++)
			{
				if (serv._fds[i].revents & POLLIN)
				{
					try
					{
						serv._users[i - 1].readSocket();
						while (serv._users[i - 1]._buffer.find("\n"))
						{
							std::cerr << "#DEBUG: " << serv._users[i - 1]._buffer.find("\n") << std::endl;
							std::cout << "[" << i << "]{" << serv._users[i - 1]._buffer.substr(0, serv._users[i - 1]._buffer.find("\n")) << "}" << std::endl;
							serv._users[i - 1]._buffer.erase(0, serv._users[i - 1 ]._buffer.find("\n"));
						}
					}
					catch(std::exception const& e)
					{
						std::cerr << e.what() << std::endl;
						close(serv._fds[i].fd);
						serv._fds.erase(serv._fds.begin() + i);
						serv._users.erase(serv._users.begin() + i - 1);
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
