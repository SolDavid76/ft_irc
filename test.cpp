#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>

#include <iostream>
#include <vector>

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

	std::cout << "Please wait . . ." << std::endl;

	// struct pollfd fds[1];
	// fds[0].fd = sock;
	// fds[0].events = POLLIN;

	std::vector<pollfd> poll_fds(5);
    poll_fds[0].fd = sock;
    poll_fds[0].events = POLLIN;
	int newsock = 0;
	while (1)
	{
		// int x = poll(fds, 1, -1);
		int x = poll(poll_fds.data(), poll_fds.size(), -1);

		if (poll_fds[0].revents & POLLIN)
		{
			if (newsock != 0)
				newsock = accept(sock, (struct sockaddr*)&client_addr, &client_len);
			if (newsock == -1)
			{
				std::cout << "err" << std::endl;
				continue;
			}
			poll_fds[1].fd = newsock;
			poll_fds[1].events = POLLIN;
			send(poll_fds.back().fd, "Welcome\n", 9, 0);
			std::cout << "Someone is connecting . . ." << std::endl;

			for (size_t i = 0; i < poll_fds.size(); i++)
			{
				int n = recv(poll_fds[i].fd, buffer, sizeof(buffer) - 1, 0);

				if (n <= 0)
				{
					if (n == 0)
						std::cout << "Connection closed" << std::endl;
					else
						std::cout << "idk" << std::endl;
					close(poll_fds[i].fd);
					poll_fds.erase(poll_fds.begin() + i);
					i--;
					continue;
				}

				buffer[n] = '\0';
				std::cout << "[" << buffer << "]" << std::endl;
			}
		}
	}
}


