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
std::vector<char>::const_iterator	find_pattern(const std::vector<char> &data, const std::vector<char> &pattern) {
	return std::search(data.begin(), data.end(), pattern.begin(), pattern.end());
}

Client::Client(t_socket client_socket, struct sockaddr_in addr, Connection *connection) : _client_socket(client_socket), _addr(addr),  _status(READING), _connection(connection)
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
	std::vector<char>	buf(MAX_RECV_SIZE);
	// char	buffer[MAX_RECV_SIZE] = {0};
	int					ret;
	size_t				len = 0;

	ret = recv(this->_client_socket, buf.begin().base(), MAX_RECV_SIZE, 0);
	if (ret <= 0)
		return (ret);
	std::vector<char>::const_iterator it = buf.begin();
	while (it != buf.end()) {
		std::cout << (*it);
		it++;
	}
	std::cout << std::endl;
	if (this->_req.getStatus() == Request::HEADER) {
		LOG_YELLOW("Header_status = Header");
		std::vector<char>::const_iterator pos = find_pattern(buf, std::vector<char> (EO_HEADER, EO_HEADER + 4));
		if (pos == buf.end()) {
			LOG_RED("Not a valid HTTP header (in the first " << MAX_RECV_SIZE << " bytes");
			_status = DIE; //is the same as WRITING! but writes 400 -> BAD request.
			return 1;
		}
		else {
			this->_req.setHeader(std::string(static_cast<std::vector<char>::const_iterator>(buf.begin()), pos));
			this->_req.addBody(_body.begin(), pos, static_cast<std::vector<char>::const_iterator>(buf.end()));
			//parse here
			//parser header (and body?) -> only thing checked till here is that we have '\r\n\r\n'
			//for testing purpose we are setting status to WRITE but this can differ depending on the header.
			_status = WRITING;
		}
	}
	else {
		LOG_YELLOW("Header_status = Body");
		this->_req.addBody(_body.begin(), buf.begin(), static_cast<std::vector<char>::const_iterator>(buf.end()));
	}
	return 1;
}

int Client::sendResponse()
{
	this->_status = READING;
	//testing purpose:
	std::string response = "HTTP/1.1 200 Ok\r\nContent-length: 0\r\n\r\n";
	if (send(this->_client_socket, response.c_str(), response.length(), 0) == -1)
		return 0;
	LOG_RED("Set header status to HEADER");
	this->_req.clear();
	return 1;
}