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

void	webserv::_removeClient(std::vector<client *>::iterator &pos)
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
	for (std::vector<connection*>::iterator itr = this->_connections.begin(); itr != this->_connections.end(); itr++)
	{
		FD_SET((*itr)->getSocket(), &this->_readfds);
		if ((*itr)->getSocket() > this->_maxfds)
			this->_maxfds = (*itr)->getSocket();
	}
	
	// add clients to read fds
	for (std::vector<client*>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
	{
		if ((*itr)->getClientStatus() == client::READING)
			FD_SET((*itr)->getSocket(), &this->_readfds);
		else if ((*itr)->getClientStatus() == client::WRITING)
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
		if ((select_ret = select(this->_maxfds + 1, &this->_readfds, &this->_writefds, NULL, &timeout)) == -1)
		{
			perror("select");
			CONNECTION_ERROR("Select Error");
		}
		else if (select_ret >= 1) // successfull
		{
			for (std::vector<connection*>::iterator itr = this->_connections.begin(); itr != this->_connections.end(); itr++)
			{
				if (FD_ISSET((*itr)->getSocket(), &this->_readfds)) // new client on this connection
				{
					try
					{
						this->_clients.push_back((*itr)->newAccept());
						this->_clients.back()->setClientStatus(client::READING);

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
					// HIER MUSS EINGELESEN WERDEN, KANNST DU AUCH IN CLIENT NE FUNKTION MACHEN
					// @JONAS
					char buffer[4096];
					memset(buffer, 0, 4096);
					int chars = recv((*itr)->getSocket(), buffer, 4096, 0);
					std::cout << "Request: \n" <<  buffer << std::endl << std::endl;
					
					// set writing state to make sure we can answer
					(*itr)->setClientStatus(client::WRITING);

				}
				else if (FD_ISSET((*itr)->getSocket(), &this->_writefds))
				{
					// HIER WERDEN DIE ANTOWRTEN GESCHICKT

					// @JONAS
					std::cout << "response sent" << std::endl;

					write((*itr)->getSocket(), "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 11\n\nHello world", strlen("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 11\n\nHello world"));
					// Set reading after response
					(*itr)->setClientStatus(client::READING);
				}
			}
		}
		else // TIMOUT
		{
			std::cout << "wiping" << std::endl;
			// Clean all Clients
			for (std::vector<client *>::iterator itr = this->_clients.begin(); itr != this->_clients.end(); itr++)
				this->_removeClient(itr);
		}
	}
}