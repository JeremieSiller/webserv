/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/08 16:04:49 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/09 15:35:00 by nschumac         ###   ########.fr       */
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

int Request::_iscrlf(const char *str, int &idx)
{
	if (str[idx] == '\r')
	{
		if (str[idx + 1] == '\n')
			return 2;
		return -1;
	}
	return 0;
}

int Request::_isalpha(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int Request::_isSpecial(char c, bool type)
{
	if (type)
		return (c == '_' || c == '-');
	return (strchr("_ :;.,\\/\"'?!(){}[]@<>=-+*#$&`|~^%", c) != NULL);
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
				else
					this->_method += str[i];
				break;
			case RequestLineURI:
				if (str[i] == ' ')
					this->_ps = RequestLineHTTPH;
				else
					this->_path += str[i];
				break;
			case RequestLineHTTPH:
				if (str[i] != 'H')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHT;
				break;
			case RequestLineHTTPHT:
				if (str[i] == 'T')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTT;
				break;
			case RequestLineHTTPHTT:
				if (str[i] == 'T')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP;
				break;
			case RequestLineHTTPHTTP:
				if (str[i] == 'P')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_;
				break;
			case RequestLineHTTPHTTP_:
				if (str[i] == '/')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_x;
				break;
			case RequestLineHTTPHTTP_x:
				if (str[i] == '1')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_x_;
				break;
			case RequestLineHTTPHTTP_x_:
				if (str[i] == '.')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_x_x;
				break;
			case RequestLineHTTPHTTP_x_x:
				if (str[i] == '1')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineCRLF;
				break;
			case RequestLineCRLF:
				if (!this->_iscrlf(str, i))
					return BAD_REQUEST;
				if (str[++i] != '\n')
					return BAD_REQUEST;
				this->_ps = RequestHeaderStart;
				break;
			case RequestHeaderStart:
				if (this->_iscrlf(str, i))
				{
					if (str[++i] == '\n')
						this->_ps = RequestBodyStart;
					return BAD_REQUEST;
				}
				else if (!this->_isalpha(str[i]) && !this->_isSpecial(str[i], true))
					return BAD_REQUEST;
				headerName = str[i];
				this->_ps = RequestHeaderName;
			case RequestHeaderName:
				if (str[i] == ':')
				{
					if (this->_headers.count(headerName)) // header already in map invalid shit
						return BAD_REQUEST;
					this->_headers.insert(std::pair<std::string, std::string>(headerName, ""));
					this->_ps = RequestHeaderSpace;
				}
				else if (!this->_isalpha(str[i]) && !this->_isSpecial(str[i], true))
					return BAD_REQUEST;
				else if (str[i] == '\r')
					this->_ps = RequestBodyStart;
				else
					headerName += str[i];
				break;
			case RequestHeaderSpace:
				if (str[i] != ' ')
					return BAD_REQUEST;
				this->_ps = RequestHeaderOptions;
				break;
			case RequestHeaderOptions:
				if (this->_iscrlf(str, i))
				{
					if (str[++i] == '\n')
						this->_ps = RequestHeaderStart;
					return BAD_REQUEST;
				}
				else if (!this->_isalpha(str[i]) && !this->_isSpecial(str[i], false))
					return BAD_REQUEST;
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