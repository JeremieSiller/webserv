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
#include "connection.hpp"

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

Client::Client(t_socket client_socket, Connection *connection) : _client_socket(client_socket), _status(READING), _connection(connection), _offset(), _buf(MAX_RECV_SIZE)
{
}

Client::~Client()
{
	close(this->_client_socket);
}



int	Client::readRequest()
{
	int					ret;

	ret = recv(this->_client_socket, _buf.begin().base(), MAX_RECV_SIZE, 0);
	if (ret <= 0)
		return (ret);
	if (this->_req.getStatus() == Request::HEADER) {
		this->_subBuffer.insert(this->_subBuffer.end(), _buf.begin(), _buf.begin() + ret);
		std::vector<char>::const_iterator pos = find_pattern(_subBuffer, std::vector<char> (EO_HEADER, EO_HEADER + 4));
		if (pos != this->_subBuffer.end())
		{
			this->_req.setHeader(std::string(static_cast<std::vector<char>::const_iterator>(_subBuffer.begin()), pos + 4));
			if (this->_req.getStatus() == Request::INVALID) {
				this->_status = WRITING;
			}
			LOG_RED(this->_req.getHeader());
			this->_req.addBody(pos + 4, static_cast<std::vector<char>::const_iterator>((_subBuffer.end())));
			this->_subBuffer.clear();
		}
	}
	else {
		this->_req.addBody(_buf.begin(), static_cast<std::vector<char>::const_iterator>((_buf.begin() + ret)));
	}

	if (this->_req.getStatus() == Request::COMPLETE) {
		LOG_BLUE("Request complete!");
		memset(&_buf[0], 0, _buf.size() * sizeof(_buf[0]));
		this->_status = WRITING;
	}
	return 1;
}

int Client::sendResponse()
{
	if (_interpreter.getResponse().getState() == response::START_WRITING) {
		_interpreter = Interpreter(&this->_req, this->_connection);
		_interpreter.execute();
	}
	ssize_t tmp = _interpreter.send(this->_client_socket);
	if (tmp == -1) {
		this->_status = READING;
		this->_req.clear();
		LOG_RED("Could not write to socket");
		return 0;
	}
	if (tmp == 0) {
		LOG_BLUE("Server sent Response");
		this->_status = READING;
		this->_req.clear();
		if (_req.getConnection() == false) {
			LOG_BLUE("Client wished to close the connection\n");
			return 0;
		}
	}
	return 1;
}

int	Client::internalServerError() {
	ssize_t	tmp;

	LOG_RED("Internal Server Error: sending 500");
	tmp = write(_client_socket, &(INTERNAL_SERVER_ERROR[_offset]), strlen(INTERNAL_SERVER_ERROR) - _offset);
	if (tmp == -1) {
		this->_status = READING;
		this->_req.clear();
		_offset = 0;
		LOG_RED("Could not write to socket");
		return (0);
	}
	if (tmp == strlen(INTERNAL_SERVER_ERROR)) {
		this->_status = READING;
		this->_req.clear();
		LOG_BLUE("Server sent 500 to client");
		_offset = 0;
		return 0;
	}
	_offset += tmp;
	return 1;
}