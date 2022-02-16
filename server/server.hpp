/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsiller <jsiller@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/16 12:18:41 by jsiller          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#pragma once

# include <iostream>
# include <vector>

class server {
	const	std::vector<std::string>	_server_name;	//server_names the server listens to.
	const	std::vector<std::string>	_error_pages;
	const	size_t						_max_body_size;	// max upload file size. Size identifier needs to be handled
	const	std::string					_root; 			//root where to finde files -> will be passed on to location which will than append its own root (I guess xD)
};

/* UNIX includes */
# include <sys/socket.h>
# include <unistd.h>
#endif