/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/18 18:50:54 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#pragma once

# include <iostream>
# include <vector>
# include <map>
# include <sys/socket.h>
# include <unistd.h>
# include "../parsing/ConfigParser.hpp"

class Server {
	private:

		const std::vector<std::string>		_server_name;	//server_names the server listens to.
		const std::map<int, std::string>	_error_pages;
		const std::vector<location>			_locations;
		const size_t						_max_body_size;	// max upload file size. Size identifier needs to be handled
		const std::string					_root; 			//root where to finde files -> will be passed on to location which will than append its own root (I guess xD)
		const char							_autoindex;

	
	public:

		Server(std::vector<std::string> const &server_name, std::map<int, std::string> const &error_pages, 
			std::vector<location> const & locations, size_t const & max_body_size, std::string const & root, char const & autoindex)
			: _server_name(server_name), _error_pages(error_pages), _locations(locations), _max_body_size(max_body_size), _root(root), _autoindex(autoindex)
		{
		}

		std::string custommessage() { return this->_server_name[0]; }

		~Server() {};
};


#endif