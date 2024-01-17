/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:39:01 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/16 15:24:00 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "User.hpp"

class Channel
{
	public:
		Channel(void);
		Channel(User* owner, std::string name);
		Channel(Channel const& src);
		Channel& operator=(Channel const& src);
		~Channel(void);

		void _JOIN(User* user);
		void leaveChannel(User* user, std::string mod, std::string msg);
		std::string userList(void);
		std::string modsList(void);
		void ft_sendAll(std::string msg);
		void ft_sendAll(User* user, std::string msg);
		int findUser(std::string name);
	// private:
		std::string _name;
		bool _invitationOnly;
		bool _topicCanBeChange;
		std::string _password;
		size_t _maxUsers;
		std::string _topic;
		User* _owner;
		std::vector<User*> _invited;
		std::vector<User*> _users;
		std::vector<User*> _admins;
};

#endif
