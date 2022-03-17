#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#pragma once
# include <vector>
# include <sys/select.h>
# include <map>
# include <climits>
# include <cstring>
# include "connection.hpp"
# include "client.hpp"
# include "../parsing/ConfigParser.hpp"
# include "../main.hpp"

class webserv
{
	private:

		std::vector<Connection *>	_connections;
		std::vector<Client *>		_clients;

		void	_initializeConnections(ConfigParser const& p);
		void	_removeClient(std::vector<Client *>::iterator &pos);

		fd_set						_readfds;
		fd_set						_writefds;
		int							_maxfds;

		void	_initSets();
		

	public:

		void	run();

		webserv(ConfigParser const& p);
		~webserv();

};



#endif