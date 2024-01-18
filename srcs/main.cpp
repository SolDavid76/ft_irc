/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/22 14:33:03 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/18 14:00:58 by djanusz          ###   ########.fr       */
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
			poll(&(serv.getFds()[0]), serv.getFds().size(), -1);

			if (serv.getFds()[0].revents & POLLIN)
			{
				User* newUser = new User(accept(serv.getSocket(), NULL, NULL));
				if (newUser->getSocket().fd == -1)
				{
					std::cerr << "accept error" << std::endl;
					delete (newUser);
					continue;
				}
				serv.addFds(newUser->getSocket());
				serv.addUsers(newUser);
			}
			for (size_t i = 1; i < serv.getFds().size(); i++)
			{
				if (serv.getFds()[i].revents & POLLIN)
				{
					try
					{
						serv.getUsers()[i - 1]->readSocket();
						while (serv.getUsers()[i - 1]->getBuffer().find("\r\n") != std::string::npos)
							serv.execCommand(ft_split(serv.getUsers()[i - 1]->getBuffer()), serv.getUsers()[i - 1]);
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
