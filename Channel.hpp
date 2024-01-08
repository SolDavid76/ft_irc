/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ennollet <ennollet@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:39:01 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/08 16:02:54 by ennollet         ###   ########.fr       */
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
	// private:
		size_t _maxUsers;
		bool _invitationOnly;
		std::string _name;
		std::string _password;
		std::string _topic;
		std::vector<User*> _invited;
		std::vector<User*> _users;
		std::vector<User*> _admins;


		int findUser(std::string name);
};

#endif
