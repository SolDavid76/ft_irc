/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/20 15:33:33 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/21 10:14:07 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

void User::new_connection(int socket, std::string info)
{
	std::string buf;
	this->_socket = socket;
	this->_nick = info.substr(info.find("NICK") + 5, info.find("\n"));
}

int main(int ac, char** av)
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	char buffer[1024];

	// config serv (ipv4/port)
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(atoi(av[1]));

	if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		std::cout << "bind error" << std::endl;
		close(sock);
		return (-1);
	}
	if (listen(sock, 5) == -1)
	{
		std::cout << "listen error" << std::endl;
		close(sock);
		return (-2);
	}

	std::cerr << "#DEBUG = " << inet_ntoa(server_addr.sin_addr) << std::endl;
	std::cout << "Please wait . . ." << std::endl;

	std::vector<struct pollfd> fds;
	struct pollfd x;
	x.fd = sock;
	x.events = POLLIN;
	fds.push_back(x);
	std::cout << "Number of users : " << fds.size() - 1 << std::endl;

	while (1)
	{
		poll(&fds[0], fds.size(), -1);

		if (fds[0].revents & POLLIN)
		{
			x.fd = accept(sock, (struct sockaddr*)&client_addr, &client_len);
			std::cerr << "#DEBUG = " << inet_ntoa(client_addr.sin_addr) << std::endl;
			x.events = POLLIN;
			if (x.fd == -1)
			{
				std::cout << "accept error" << std::endl;
				continue;
			}
			recv(x.fd, buffer, 1024 - 1, 0);
			// User admin;
			// admin.new_connection(x.fd, buffer);
			send(x.fd, ":10.33.10.2 001 djanusz :Welcome\r\n", 34, 0);
			std::cout << "Someone is connected . . ." << std::endl;
			fds.push_back(x);
			std::cout << "Number of users : " << fds.size() - 1 << std::endl;
		}
		for (int i = 1; i < fds.size(); i++)
		{
			if (fds[i].revents & POLLIN)
			{
				int n = recv(fds[i].fd, buffer, 1024 - 1, 0);
				if (n <= 0)
				{
					if (n == 0)
						std::cout << "Connection closed" << std::endl;
					else
						std::cout << "Connection losed" << std::endl;
					close(fds[i].fd);
					fds.erase(fds.begin() + i);
					i--;
					continue;
				}
				buffer[n] = '\0';
				std::cout << "[" << i << "]: " << buffer << std::endl;
			}
		}
	}
}
