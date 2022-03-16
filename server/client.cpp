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

int strHexDec(std::string str)
{
	int bruh = 0;
	for (int i = str.length() - 1; i >= 0; --i)
	{
		bruh += ((str[i] >= 'A' ? str[i] - 'A' + 10 : str[i] - '0') << (i - str.length() - 1) * 4);
	}
	return bruh;
}

int	Client::readRequest()
{
	char	buffer[MAX_RECV_SIZE] = {0};
	int		ret;
	size_t		len = 0;

	ret = recv(this->_client_socket, buffer, MAX_RECV_SIZE, 0);
	if (ret <= 0)
		return (ret);
	this->_request +=  std::string(buffer, ret);
	for (int i = 0; i < this->_request.size(); ++i)
		std::cout << this->_request[i];
	size_t body = this->_request.find("\r\n\r\n");
	if (body != std::string::npos)
	{
		if (this->_request.find("Transfer-Encoding: chunked") != std::string::npos)
		{
			int i = this->_request.find("\r\n", body + 4);
			len = strHexDec(this->_request.substr(body + 4, i - (body + 4)));
		}
		else
		{
			// Content-Length is omited in case of chunked encoding
			// needs to exist i think xd
			if (this->_request.find("Content-Length: ") != std::string::npos)
				len = ::atoi(this->_request.substr(this->_request.find("Content-Length: ") + 16, 11).c_str());
			else
				return -1;
		}
	}
	else
		return -1;
	std::cout << "{" << len << ", " << this->_request.size() - body - 4 << "}";
	if (this->_request.size() >= len + body + 4)
		this->_status = WRITING;
	else
		this->_status = READING;
	return 1;
}

int Client::sendResponse()
{
	this->_request.clear();
	return 0;
}