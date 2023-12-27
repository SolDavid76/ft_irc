/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/21 14:44:35 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/27 12:15:02 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(void)
{
	this->_name = "";
}

Channel::Channel(std::string name)
{
	this->_name = name;
}

Channel::Channel(Channel const& src)
{
	*this = src;
}

Channel& Channel::operator=(Channel const& src)
{
	if (this != &src)
	{
		this->_name = src._name;
		this->_users = src._users;
	}
	return (*this);
}

Channel::~Channel(void)
{
}
