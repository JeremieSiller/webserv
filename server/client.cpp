/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 19:04:16 by nschumac          #+#    #+#             */
/*   Updated: 2022/03/08 14:37:05 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <unistd.h>


Client::Client(t_socket client_socket, struct sockaddr_in addr, Connection *connection) : _client_socket(client_socket), _addr(addr),  _status(), _connection(connection)
{
}

Client::~Client()
{
	close(this->_client_socket);
}

int	Client::readRequest()
{
	char	buffer[4098] = {0};
	int		ret;

	ret = recv(this->_client_socket, buffer, 4096 - 1, 0);
	if (ret <= 0)
		return (ret);
		
	std::cout << buffer << std::endl;
	
	if (this->_request.getType() == Request::COMPLETE || this->_request.getType() == Request::INVALID) // gen new request
		this->_request = Request(buffer, ret);
	else
		this->_request.append(buffer, ret); // append newly read segment
	
	switch (this->_request.getType())
	{
		case Request::INCOMPLETE:
			this->_status = READING;
			break;
		case Request::INVALID:
			this->_status = DIE;
			break;
		default:
			this->_status = WRITING;
	}

	return ret;
}

int Client::sendResponse()
{
	try
	{
		Server &server = this->_connection->getServer(this->_request.getServerName());
		std::cout << "Server Name: " << this->_request.getServerName() << std::endl;
		if (this->_request.getType() == Request::INVALID)
		{
			// do invalid response;
		}
		write(this->_client_socket, "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 11\n\nHello world", strlen("HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 11\n\nHello world"));
		this->_status = READING;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		return 0;
	}
	return 1;
}