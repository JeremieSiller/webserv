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
#include <algorithm>
#include <sstream>

char EO_HEADER[] = {'\r', '\n', '\r', '\n'};

/**
 * @brief finds a pattern of a vector in a vector and returns it position
 * use like this: find_pattern(v1, std::vector<char> {'\r'}{'\n'});
 * @param data the vector to search in
 * @param pattern the pattern to find
 * @return std::vector<char>::const_iterator the position of pattern in data (std::npos if not found);
 */
std::vector<char>::const_iterator	Client::find_pattern(const std::vector<char> &data, const std::vector<char> &pattern) {
	return std::search(data.begin(), data.end(), pattern.begin(), pattern.end());
}

Client::Client(t_socket client_socket, struct sockaddr_in addr, Connection *connection) : _client_socket(client_socket), _addr(addr),  _status(READING), _connection(connection)
{
}

Client::~Client()
{
	close(this->_client_socket);
}



int	Client::readRequest()
{
	std::vector<char>	buf(MAX_RECV_SIZE);
	// char	buffer[MAX_RECV_SIZE] = {0};
	int					ret;

	ret = recv(this->_client_socket, buf.begin().base(), MAX_RECV_SIZE, 0);
	// if (std::find(this->_req.getTransferEncoding().begin(), this->_req.getTransferEncoding().end(), "Chunked") != this->_req.getTransferEncoding().end())
	// {
	// 	_status = WRITING;
	// 	this->_req.setStatus(Request::INVALID);
	// 	return 1;
	// }
	if (ret <= 0)
		return (ret);
	
	// LOG_YELLOW("logging chars");
	// std::vector<char>::const_iterator it = buf.begin();
	// while (it != (buf.begin() + ret)) {
	// 	std::cout << (*it);
	//  	it++;
	// }
	// LOG_YELLOW("logging asciss");
	// it = buf.begin();
	// while (it != (buf.begin() + ret)) {
	// 	std::cout << (int)(*it) << ' ';
	//  	it++;
	// }
	// LOG_YELLOW("buf size: " << buf.size());
	std::cout << std::endl;
	if (this->_req.getStatus() == Request::HEADER) {
		LOG_YELLOW("Header_status = Header");
		std::vector<char>::const_iterator pos = find_pattern(buf, std::vector<char> (EO_HEADER, EO_HEADER + 4));
		if (pos == buf.end())
			this->_req.setHeader(std::string(static_cast<std::vector<char>::const_iterator>(buf.begin()), static_cast<std::vector<char>::const_iterator>(buf.begin()) + ret));
		else
			this->_req.setHeader(std::string(static_cast<std::vector<char>::const_iterator>(buf.begin()), pos + 4));
		if (this->_req.getHeader().find("\r\n\r\n") != std::string::npos || pos != buf.end())
		{
			this->_req.addBody(pos + 4, static_cast<std::vector<char>::const_iterator>((buf.begin() + ret)));
			if(!this->_req.parseHeader())
			{
				this->_status = WRITING;
				this->_req.setStatus(Request::INVALID);
			}
			this->_req.setStatus(Request::BODY);
		}
	}
	else {
		LOG_YELLOW("Header_status = Body");
		this->_req.addBody(buf.begin(), static_cast<std::vector<char>::const_iterator>((buf.begin() + ret)));
	}

	if (this->_req.getStatus() == Request::COMPLETE)
		_status = WRITING;
	return 1;
}

int Client::sendResponse()
{
	this->_status = READING;
	Interpreter i(this->_req, this->_connection);
	if (i.send(this->_client_socket) == -1) {
		this->_req.clear();
		LOG_RED("Could not write to socket");
		return 0;
	}
	if (_req.getConnection() == false) {
		this->_req.clear();
		LOG_BLUE("Client wished to close the connection\n");
		return 0;
	}
	this->_req.clear();
	return 1;
}