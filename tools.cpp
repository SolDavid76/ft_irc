/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:32:33 by djanusz           #+#    #+#             */
/*   Updated: 2023/12/23 20:49:28 by djanusz          ###   ########.fr       */
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
		if (pos != std::string::npos && pos == tmp.length() - 2)
			tmp.erase(pos);
		res.push_back(tmp);
	}
	return (res);
}
