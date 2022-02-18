/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:05:03 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/17 22:15:09 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "connection.hpp"
#include <fcntl.h>
#include <arpa/inet.h>

connection::connection(const unsigned short &port, const std::string &address) :  _servers(), _locations(), _address(address), _port(port)
{
	if ((this->_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
		CONNECTION_ERROR("Socket Creation Error");
	
	struct sockaddr_in addr;
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->_port);
	addr.sin_addr.s_addr = inet_addr(this->_address.c_str());

	if (bind(this->_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		CONNECTION_ERROR("Server Binding: ");

	if (listen(this->_socket, -1) == -1)
		CONNECTION_ERROR("Server Listen: ");
}

client *connection::newAccept()
{
	t_socket			client_socket;
	struct sockaddr_in	client_addr;
	socklen_t			len = sizeof(client_addr);
	
	client_socket = accept(this->_socket, (sockaddr *)&client_addr, &len);
	
	if (client_socket == -1)
		CONNECTION_ERROR("Server Acccept: ");

	fcntl(client_socket, F_SETFL, O_NONBLOCK);

	return new client(client_socket, (struct sockaddr_in)client_addr);
}

void	connection::addServer(server const& in)
{
	this->_servers.push_back(in);
}

connection::~connection() {
	close(this->_socket);
}