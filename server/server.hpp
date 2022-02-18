/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/18 17:45:14 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#pragma once

# include <iostream>
# include <vector>

class Server {
	private:

		const	std::vector<std::string>	_server_name;	//server_names the server listens to.
		const	std::vector<std::string>	_error_pages;
		const	size_t						_max_body_size;	// max upload file size. Size identifier needs to be handled
		const	std::string					_root; 			//root where to finde files -> will be passed on to location which will than append its own root (I guess xD)
	
	public:

		Server(std::vector<std::string> server_name, std::vector<std::string> error_pages, size_t max_body_size, std::string root) : _server_name(server_name), _error_pages(error_pages), _max_body_size(max_body_size), _root(root) {}
		Server(Server const & in): _server_name(in._server_name), _error_pages(in._error_pages), _max_body_size(in._max_body_size), _root(in._root){}

		std::string custommessage() { return this->_server_name[0]; }
		~Server() {};
};

/* UNIX includes */
# include <sys/socket.h>
# include <unistd.h>
#endif