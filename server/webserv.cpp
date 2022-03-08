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
#include <sys/time.h>


webserv::webserv(ConfigParser const & p) : _connections(), _clients(), _readfds(), _writefds(), _maxfds()
{
	_initializeConnections(p);
}

webserv::~webserv() {}

void	webserv::_initializeConnections(ConfigParser const& p)
{
	for (std::vector<connection>::const_iterator itr = p._connections.begin(); itr != p._connections.end(); itr++)
	{
		this->_connections.push_back(new Connection(itr->_port, itr->_address));
		for (std::vector<server>::const_iterator serv = itr->_servers.begin(); serv != itr->_servers.end(); serv++)
		{
			// fill in max_client_size here still
			this->_connections.back()->addServer(Server(serv->_server_names, serv->_error_pages, serv->_locations, INT_MAX, serv->_root, serv->_autoindex));
		}
	}
}

void	webserv::_removeClient(std::vector<Client *>::iterator &pos)
{
	delete *pos;
	this->_clients.erase(pos);
	pos--;
}

void	webserv::_initSets()
{
	// for select, Largest/Highset Filedescriptor
	this->_maxfds = 0;

	FD_ZERO(&this->_readfds);
	FD_ZERO(&this->_writefds);

	// add server to reading fds
	for (std::vector<Connection*>::iterator itr = this->_connections.begin(); itr != this->_connections.end(); itr++)
	{
		FD_SET((*itr)->getSocket(), &this->_readfds);
		if ((*itr)->getSocket() > this->_maxfds)
			this->_maxfds = (*itr)->getSocket();
	}
	
	// add clients to read fds
	for (std::vector<Client*>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
	{
		if ((*itr)->getClientStatus() == Client::READING)
			FD_SET((*itr)->getSocket(), &this->_readfds);
		else if ((*itr)->getClientStatus() == Client::WRITING || (*itr)->getClientStatus() == Client::DIE)
			FD_SET((*itr)->getSocket(), &this->_writefds);
		else
			this->_removeClient(itr);
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
		struct timeval timeout;

		timeout.tv_sec = 10;
		timeout.tv_usec = 0;
		select_ret = select(this->_maxfds + 1, &this->_readfds, &this->_writefds, NULL, &timeout);
		if (select_ret == -1)
		{
			perror("select");
			CONNECTION_ERROR("Select Error");
		}
		else if (select_ret >= 1) // successfull
		{
			for (std::vector<Connection*>::iterator itr = this->_connections.begin(); itr != this->_connections.end(); itr++)
			{
				if (FD_ISSET((*itr)->getSocket(), &this->_readfds)) // new client on this connection
				{
					try
					{
						this->_clients.push_back((*itr)->newAccept());
						this->_clients.back()->setClientStatus(Client::READING);

					} catch(std::exception &e)
					{
						std::cout << e.what() << std::endl;
					}
				}
			}

			for (std::vector<Client*>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
			{
				if (FD_ISSET((*itr)->getSocket(), &this->_readfds)) // we can read from client
				{
					// Function call sets correct status
					// if Client is DIE check what needs to be written
					if ((*itr)->readRequest() <= 0) // if it returns 0 or -1 | close socket
					{
						this->_removeClient(itr);
						continue;
					}
				}
				else if (FD_ISSET((*itr)->getSocket(), &this->_writefds))
				{
					// either client status is DIE or WRITE which is the same
					(*itr)->sendResponse();
				}
			}
		}
		else // TIMOUT
		{
			std::cout << "wiping" << std::endl;
			// Clean all Clients
			for (std::vector<Client *>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
				this->_removeClient(itr);
		}
	}
}