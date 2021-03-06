/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/03/16 13:46:44 by nschumac         ###   ########.fr       */
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
# include "../request/Request.hpp"
# include "../main.hpp"

class Server {
	private:

		std::vector<std::string>		_server_name;	//server_names the server listens to.
		std::map<int, std::string>		_error_pages;
		std::vector<location>			_locations;
		size_t							_max_body_size;	// max upload file size. Size identifier needs to be handled
		std::string						_root; 			//root where to finde files -> will be passed on to location which will than append its own root (I guess xD)

	
	public:

		Server() : _server_name(), _error_pages(), _locations(), _max_body_size(), _root() { }
		Server(std::vector<std::string> const &server_name, std::map<int, std::string> const &error_pages, 
			std::vector<location> const & locations, size_t const & max_body_size, std::string const & root)
			: _server_name(server_name), _error_pages(error_pages), _locations(locations), _max_body_size(max_body_size), _root(root)
		{
		}

		Server&operator=(Server const &in)
		{
			this->_server_name = in._server_name;
			this->_error_pages = in._error_pages;
			this->_locations = in._locations;
			this->_max_body_size = in._max_body_size;
			this->_root = in._root;
			return *this;
		}

		// std::string custommessage() { return this->_server_name[0]; }
		
		// should return op codes
		// depending on this server will formulate response
		int	doRequest(Request const &req);
		
		bool operator==(std::string const &in ) const
		{
			for (unsigned int i = 0; i < _server_name.size(); i++)
				if (in == _server_name[i])
					return true;
			return false;
		}

		~Server() {}
		const std::vector<std::string>	&getServerName() const { return _server_name; }
		const std::map<int, std::string> &getErrorPages() const { return _error_pages; }
		const std::vector<location>		&getLocations() const { return _locations; }
		const size_t					&getMaxBodySize() const { return _max_body_size; }
		const std::string				&getRoot() const { return _root; }
};


#endif
