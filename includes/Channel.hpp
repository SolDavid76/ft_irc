/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:39:01 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/18 13:50:12 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "User.hpp"

class Channel
{
	public:
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

		/* GETTERS */
		std::string const& getName() const { return _name; }
		bool getInvitationOnly() const { return _invitationOnly; }
		bool getTopicChangeable() const { return _topicCanBeChange; }
		std::string const& getPassword() const { return _password; }
		size_t getMaxUsers() const { return _maxUsers; }
		std::string const& getTopic() const { return _topic; }
		User* getOwner() const { return _owner; }
		std::vector<User*> getInvitedUsers() const { return _invited; }
		std::vector<User*> getUsers() const { return _users; }
		std::vector<User*> getAdmins() const { return _admins; }

		/* SETTERS */
		void setName(std::string const& name) { _name = name; }
		void setInvitationOnly(bool invitationOnly) { _invitationOnly = invitationOnly; }
		void setTopicChangeable(bool topicChangeable) { _topicCanBeChange = topicChangeable; }
		void setPassword(std::string const& password) { _password = password; }
		void setMaxUsers(size_t maxUsers) { _maxUsers = maxUsers; }
		void setTopic(std::string const& topic) { _topic = topic; }
		void setOwner(User* owner) { _owner = owner; }
		void addInvitedUsers(User* invited) { _invited.push_back(invited); }
		void addUsers(User* users) { _users.push_back(users); }
		void addAdmins(User* admins) { _admins.push_back(admins); }

	private:
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
