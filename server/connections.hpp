/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connections.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsiller <jsiller@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:00:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/16 12:08:16 by jsiller          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONS_HPP
# define CONNECTIONS_HPP

#pragma once

# include "server.hpp"
# include <vector>
# include <string>

# define STDPORT 80
# define STDADDRESS "0.0.0.0.0" //not 100% sure about this

class connections
{
private:
	std::vector<server>	_servers;
	const unsigned		_port;
	const std::string	_address;
public:
	connections(const unsigned &port = 80, const std::string &adress = "0.0.0.0.0"); 
	~connections();
};



#endif