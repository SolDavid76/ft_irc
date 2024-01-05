/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:32:33 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/05 11:08:25 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::vector<std::string> ft_split(const std::string& input, char separator)
{
	std::vector<std::string> res;
	std::istringstream iss(input);
	std::string tmp;

	while (std::getline(iss, tmp, separator))
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
