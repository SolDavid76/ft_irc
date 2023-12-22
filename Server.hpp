/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:00:02 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/21 14:46:58 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Channel.hpp"

class Server
{
	public:
		Server(int port);
		Server(Server const& src);
		Server& operator=(Server const& src);
		~Server(void);
	// private:
		int _socket;
		std::vector<pollfd> _fds;
		std::vector<User> _users;
		std::vector<Channel> _channels;
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
