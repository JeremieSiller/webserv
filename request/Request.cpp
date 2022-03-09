/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/08 16:04:49 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/09 14:44:27 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const char str[], int size) : _method(""), _path(""), _version(""), _headers(), _body(), _serverName(""), _type(COMPLETE), _ps(RequestLineMethod)
{
	if (!str)
		return ;
	int parseStatus = _parse(str, size);
	if (parseStatus <= 0)
		this->_type = INVALID;
	else if (parseStatus == 1)
		this->_type = INCOMPLETE;
	else
		this->_type = COMPLETE;
}

int Request::_integrityCheck()
{
	if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE")
		return -1;
	if (this->_version != "HTTP/1.1")
		return -2;
	if (this->_path == "") // cannot be empty has to have root at least
		return -3;
	if (this->_body.size() == 0) // invalid structure missing \r\n\r\n
		return -4;
	if (this->_headers.count("Content-Length")) // check for body size
		if (atoi(this->_headers["Content-Length"].c_str()) != (int)this->_body.size() - 1) // need to read again
			return 1;
	if (!this->_headers.count("Host")) // invalid always need host, ITHINK
		return 0;
	if (this->_headers["Host"].find(":")) // i think its always Host: SERVERNAME:PORT
		this->_serverName = std::string(this->_headers["Host"].substr(0, this->_headers["Host"].find(":")));
	else
		this->_serverName = std::string(this->_headers["Host"]);
	return 2;
}


void Request::append(const char str[], int size)
{
	this->_ps = RequestHeaderName;
	int parseStatus = _parse(str, size);
	if (parseStatus <= 0)
		this->_type = INVALID;
	else if (parseStatus == 1)
		this->_type = INCOMPLETE;
	else
		this->_type = COMPLETE;	
}

// returns 0 if invalid
int	Request::_parse(const char str[], int size)
{
	int i = 0;
	std::string headerName = "";
	// Parses everything into Map Method adn stuff
	while (i <= size)
	{
		switch(this->_ps)
		{
			case RequestLineMethod:
				if (str[i] == ' ')
					this->_ps = RequestLineURI;
				else if (str[i] == '\r')
					return 0;
				else
					this->_method += str[i];
				break;
			case RequestLineURI:
				if (str[i] == ' ')
					this->_ps = RequestLineHttpVersion;
				else if (str[i] == '\r')
					return 0;
				else
					this->_path += str[i];
				break;
			case RequestLineHttpVersion:
				if (str[i] == '\r')
					this->_ps = RequestHeaderStart;
				else
					this->_version += str[i];
				break;
			case RequestHeaderStart:
				if (str[i] == '\n')
				{
					if (headerName == "" && str[i + 1] == '\r' && str[i + 2] == '\n') 
						this->_ps = RequestBodyStart;
					else
					{
						headerName = "";
						this->_ps = RequestHeaderName;
					}
				}
				else
					return -1; //INVALID
				break;
			case RequestHeaderName:
				if (str[i] == ':')
				{
					if (this->_headers.count(headerName)) // header already in map invalid shit
						return -1;
					this->_headers.insert(std::pair<std::string, std::string>(headerName, ""));
					this->_ps = RequestHeaderSpace;
				}
				else if (str[i] == '\r')
					this->_ps = RequestBodyStart;
				else
					headerName += str[i];
				break;
			case RequestHeaderSpace:
				if (str[i] != ' ')
					return -1;
				this->_ps = RequestHeaderOptions;
				break;
			case RequestHeaderOptions:
				if (str[i] == '\r')
					this->_ps = RequestHeaderStart;
				else
					this->_headers[headerName] += str[i];
				break;
			case RequestBodyStart:
				if (str[i] == '\n')
					this->_ps = RequestBody;
				break;
			case RequestBody:
				this->_body.push_back(str[i]);
				break;
		}
		i++;
	}
	return _integrityCheck();
}