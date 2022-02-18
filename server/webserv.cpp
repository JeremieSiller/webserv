/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 20:48:52 by nschumac          #+#    #+#             */
/*   Updated: 2022/02/17 21:28:04by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"


webserv::webserv(configshit cs) : _connections(), _clients()
{
	_initializeConnections(cs);
}

webserv::~webserv() {}

void	webserv::_initializeConnections(configshit cs)
{
	// Initialize Connections
	for (unsigned int i = 0; i < cs.connections.size(); i++)
	{
		this->_connections.push_back(new connection(cs.connections[i].first.port, cs.connections[i].first.addr));
		// Initialize Servers running on Connections
		for (unsigned int c = 0; c < cs.connections[i].second.size(); c++)
		{
			this->_connections.back()->addServer(server(cs.connections[i].second[c].servername, cs.connections[i].second[c].errorpages, INT_MAX, cs.connections[i].second[c].root));
		}
	}
}
void	webserv::_initSets()
{
	// for select, Largest/Highset Filedescriptor
	this->_maxfds = 0;

	FD_ZERO(&this->_readfds);

	// add server to reading fds
	for (std::vector<connection*>::iterator itr = this->_connections.begin(); itr != this->_connections.end(); itr++)
	{
		FD_SET((*itr)->getSocket(), &this->_readfds);
		if ((*itr)->getSocket() > this->_maxfds)
			this->_maxfds = (*itr)->getSocket();
	}
	
	// add clients to read fds
	for (std::vector<client*>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
	{
		FD_SET((*itr)->getSocket(), &this->_readfds);
		if ((*itr)->getSocket() > this->_maxfds)
			this->_maxfds = (*itr)->getSocket();
	}

}

void webserv::run()
{
	while (true)
	{
		this->_initSets();

		// select readfds
		int select_ret;
		if ((select_ret = select(this->_maxfds + 1, &this->_readfds, NULL, NULL, NULL)) == -1)
		{
			std::cout << this->_maxfds << select_ret << std::endl;
			perror("select");
			CONNECTION_ERROR("Select Error");
		}
		else if (select_ret == 1) // successfull
		{
			for (std::vector<connection*>::iterator itr = this->_connections.begin(); itr != this->_connections.end(); itr++)
			{
				if (FD_ISSET((*itr)->getSocket(), &this->_readfds)) // new client on this connection
				{
					try
					{
						this->_clients.push_back((*itr)->newAccept());
					} catch(std::exception &e)
					{
						std::cout << e.what() << std::endl;
					}
				}
			}
			for (std::vector<client*>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
			{
				if (FD_ISSET((*itr)->getSocket(), &this->_readfds)) // we can read from client
				{
					std::cout << (*itr)->getSocket() << std::endl;
					char buffer[4096];
					memset(buffer, 0, 4096);
					int chars = recv((*itr)->getSocket(), buffer, 4096, 0);
					std::cout << "empty buffer" << buffer << std::endl;
					write((*itr)->getSocket(), "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 25\n\nHello world", strlen("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 11\n\nHello world"));	
				}
			}
		}
		else // TIMOUT
		{

		}
	}
}