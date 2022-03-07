/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 19:04:16 by nschumac          #+#    #+#             */
/*   Updated: 2022/03/07 16:43:10 by nschumac         ###   ########.fr       */
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
	char	buffer[4096] = {0};
	int		ret;

	ret = recv(this->_client_socket, buffer, 4096 - 1, NULL);
	if (ret <= 0)
		return (ret);
		
	if (this->_request.getType() == Request::COMPLETE) // gen new request
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
	
	



	return 0;
}