/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:00:02 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/18 13:22:26 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"

class Server
{
	public:
		Server(int port, std::string password);
		~Server(void);

		typedef void (Server::*cmdFunction)(std::vector<std::string>&, User*);

		void disconect(User* user, std::string msg);
		void broadcast(User* user, std::string msg);
		void initCommands(void);
		int findUser(User* user);
		int findUser(std::string user);
		int findChannel(std::string channel);
		void execCommand(std::vector<std::string> command, User* user);
	// private:
		int _socket;
		std::string _password;
		std::vector<pollfd> _fds;
		std::vector<User*> _users;
		std::vector<Channel> _channels;
		std::map<std::string, cmdFunction> _commands;

		void _CAP(std::vector<std::string>& command, User* user);
		void _PASS(std::vector<std::string>& command, User* user);
		void _NICK(std::vector<std::string>& command, User* user);
		void _USER(std::vector<std::string>& command, User* user);
		void _PING(std::vector<std::string>& command, User* user);
		void _JOIN(std::vector<std::string>& command, User* user);
		void _PART(std::vector<std::string>& command, User* user);
		void _QUIT(std::vector<std::string>& command, User* user);
		void _KICK(std::vector<std::string>& command, User* user);
		void _MODE(std::vector<std::string>& command, User* user);
		void _TOPIC(std::vector<std::string>& command, User* user);
		void _INVITE(std::vector<std::string>& command, User* user);
		void _PRIVMSG(std::vector<std::string>& command, User* user);
		void jarvis(std::vector<std::string>& command, User* user);
};

class ft_exception: public std::exception
{
	public:
		ft_exception(std::string errorMessage) throw(): _errorMessage(errorMessage) {}
		virtual const char* what() const throw() { return (this->_errorMessage.c_str()); }
		virtual ~ft_exception() throw() {}
	private:
		std::string _errorMessage;
};

#endif
