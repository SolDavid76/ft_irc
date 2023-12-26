/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:32:33 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/26 13:09:32 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::vector<std::string> ft_split(const std::string& input)
{
	std::vector<std::string> res;
	std::istringstream iss(input);
	std::string tmp;

	while (std::getline(iss, tmp, ' '))
	{
		size_t pos = tmp.find("\r\n");
		if (pos != std::string::npos)
		{
			res.push_back(tmp.substr(0, pos));
			return (res);
		}
		res.push_back(tmp);
    }
	return (res);
}
