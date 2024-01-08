/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:39:01 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/08 16:22:40 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "User.hpp"

class Channel
{
	public:
		Channel(void);
		Channel(User& creator, std::string name);
		Channel(Channel const& src);
		Channel& operator=(Channel const& src);
		~Channel(void);

		void _JOIN(User& user);
		std::string userList(void);
	// private:
		std::string _name;
		bool _invitationOnly;
		std::string _password;
		size_t _maxUsers;
		std::string _topic;
		User* _owner;
		std::vector<User*> _invited;
		std::vector<User*> _users;
		std::vector<User*> _admins;


		int findUser(std::string name);
};

#endif
