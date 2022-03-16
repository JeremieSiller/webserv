/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 19:04:16 by nschumac          #+#    #+#             */
/*   Updated: 2022/03/16 13:1 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <unistd.h>
#include <string.h>

Client::Client(t_socket client_socket, struct sockaddr_in addr, Connection *connection) : _client_socket(client_socket), _addr(addr),  _status(), _connection(connection)
{
}

Client::~Client()
{
	close(this->_client_socket);
}

int	Client::readRequest()
{
	std::vector<char> buffer(4096);
	int		ret;

	ret = recv(this->_client_socket, buffer.begin().base(), 4096, 0);
	if (ret <= 0)
		return (ret);

	this->_request.insert(this->_requests.end(), buffer.begin(), buffer.end());
	return 1;
}

int Client::sendResponse()
{
	
	return 1;
}