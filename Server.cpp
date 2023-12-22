/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:01:51 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/21 14:47:51 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port)
{
	sockaddr_in server_addr;

	this->_socket = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(port);

	if (bind(this->_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
	{
		close(this->_socket);
		throw ft_exception("Port already used.");
	}

	pollfd fd;
	fd.fd = this->_socket;
	fd.events = POLLIN;
	fd.revents = 0;
	this->_fds.push_back(fd);
	listen(this->_socket, 5);
}

Server::~Server(void)
{
}
