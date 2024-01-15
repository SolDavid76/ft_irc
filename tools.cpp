/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: djanusz <djanusz@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:32:33 by djanusz           #+#    #+#             */
/*   Updated: 2024/01/12 16:14:17 by djanusz          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

std::vector<std::string> ft_split(const std::string& input)
{
	std::vector<std::string> res;
	std::istringstream iss(input);
	std::string info;

	std::getline(iss, info);
	info.erase(info.size() - 1, info.size());
	while (!info.empty())
	{
		std::string tmp = info.substr(0, info.find(' '));
		if (tmp[0] != ':' && tmp.size() != info.size())
		{
			res.push_back(tmp);
			info.erase(0, info.find(' ') + 1);
		}
		else
		{
			res.push_back(info.substr(0, info.size()));
			info.erase(0, info.size());
		}
	}
	return (res);
}

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
