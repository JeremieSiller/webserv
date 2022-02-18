#ifndef WEBSERV_HPP
# define WEBSERV_HPP

#pragma once
# include <vector>
# include <sys/select.h>
# include <map>
# include "connection.hpp"
# include "client.hpp"

// THIS SI THE PARSED SHIT
typedef struct t_connectionshit
{
	unsigned short	port;
	std::string 	addr;
}connectionshit;

typedef struct t_servershit
{
	std::vector<std::string> servername;
	std::vector<std::string> errorpages;
	std::string	root;
}servershit;

typedef struct t_configshit
{
	std::vector<std::pair<connectionshit, std::vector<servershit> > > connections;
}	configshit;
// IDK what you give me @JEREMY

class webserv
{
	private:

		std::vector<connection*>	_connections;
		std::vector<client*>		_clients;

		void	_initializeConnections(configshit cs);
		void	_removeClient(std::vector<client *>::iterator &pos);

		fd_set						_readfds;
		fd_set						_writefds;
		int							_maxfds;

		void	_initSets();
		

	public:

		void	run();

		webserv(configshit cs);
		~webserv();

};



#endif