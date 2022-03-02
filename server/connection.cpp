/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:05:03 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/18 18:50:30 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "connection.hpp"
#include <fcntl.h>
#include <arpa/inet.h>
#include "client.hpp"


Connection::Connection(const unsigned short &port, const std::string &address) :  _servers(), _port(port), _address(address)
{
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		CONNECTION_ERROR("Socket Creation Error");
	
	struct sockaddr_in addr = (struct sockaddr_in) { };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->_port);
	addr.sin_addr.s_addr = inet_addr(this->_address.c_str());

	if (bind(this->_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		CONNECTION_ERROR("Connection Bind: you cannot bind this address");

	if (listen(this->_socket, -1) == -1)
		CONNECTION_ERROR("Connection Listen: could'nt do it bitch");
}

Client *Connection::newAccept()
{
	t_socket			client_socket;
	struct sockaddr_in	client_addr;
	socklen_t			len = sizeof(client_addr);
	
	client_socket = accept(this->_socket, (sockaddr *)&client_addr, &len);
	
	if (client_socket == -1)
		CONNECTION_ERROR("Connection Accept: returned an invalid Socket!");

	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	return new Client(client_socket, (struct sockaddr_in)client_addr, this);
}

void	Connection::addServer(Server const& in)
{
	this->_servers.push_back(in);
}

Connection::~Connection() {
	close(this->_socket);
}