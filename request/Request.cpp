/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/09 16:53:29 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/15 19:07:44 by jhagedor         ###   ########.fr       */
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
	// if (this->_version != "HTTP/1.1")
	// 	return -2;
	if (this->_path == "")
		this->_path = "/";
	if (this->_body.size() == 0) // invalid structure missing \r\n\r\n
		return 1;
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

int Request::_iscrlf(const char *str, const int &idx)
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
				if (str[i] != 'T')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTT;
				break;
			case RequestLineHTTPHTT:
				if (str[i] != 'T')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP;
				break;
			case RequestLineHTTPHTTP:
				if (str[i] != 'P')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_;
				break;
			case RequestLineHTTPHTTP_:
				if (str[i] != '/')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_x;
				break;
			case RequestLineHTTPHTTP_x:
				if (str[i] != '1')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_x_;
				break;
			case RequestLineHTTPHTTP_x_:
				if (str[i] != '.')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineHTTPHTTP_x_x;
				break;
			case RequestLineHTTPHTTP_x_x:
				if (str[i] != '1')
					return HTTP_VERSION_NOT_SUPPORTED;
				this->_version += str[i];
				this->_ps = RequestLineCRLF;
				break;
			case RequestLineCRLF:
				if (!this->_iscrlf(str, i))
					return BAD_REQUEST;
				// if (str[++i] != '\n')
				// 	return BAD_REQUEST;
				i++;
				this->_ps = RequestHeaderStart;
				break;
			case RequestHeaderStart:
				/* if (this->_iscrlf(str, i))
				{
					// if (str[++i] == '\n')
					// 	this->_ps = RequestBodyStart;
					return BAD_REQUEST;
				}
				else  */
				if (!this->_isalpha(str[i]) && !this->_isSpecial(str[i], true))
					return BAD_REQUEST;
				// why assign char to string? +=?
				headerName += str[i];
				this->_ps = RequestHeaderName;
			case RequestHeaderName:
				if (str[i] == ':')
				{
					// need to check if this is coorect (Jerome point of contact)
					if (this->_headers.count(headerName)) // header already in map invalid shit
						return BAD_REQUEST;
					this->_headers[headerName] = "";
					this->_ps = RequestHeaderSpace;
				}
				else if (!this->_isalpha(str[i]) && !this->_isSpecial(str[i], true))
					return BAD_REQUEST;
				// Do we need this here?
				// else if (str[i] == '\r')
				// 	this->_ps = RequestBodyStart;
				else
					headerName += str[i];
				break;
			case RequestHeaderSpace:
				// Could there be more spaces?
				if (str[i] != ' ')
					return BAD_REQUEST;
				this->_ps = RequestHeaderOptions;
				break;
			case RequestHeaderOptions:
				if (this->_iscrlf(str, i))
				{
					// if (str[++i] == '\n')
					this->_ps = RequestHeaderStart;
					// return BAD_REQUEST;
					break;
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

std::string	Request::uriDecode(std::string value)
{
	std::string result;
	result.reserve(value.size());

	for (std::size_t i = 0; i < value.size(); ++i)
	{
		char ch = value[i];
		
		if (ch == '%' && (i + 2) < value.size())
		{
			std::string hex = value.substr(i + 1, 2);
			// copy and paste, need to figure out how this line works
			char dec = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
			result.push_back(dec);
			i += 2;
		}
		else if (ch == '+')
			result.push_back(' ');
		else
			result.push_back(ch);
	}
	return result;
}

int					Request::prepareInterpreter()
{
	// check what kind of Request-URI we have
	// Request-URI    = "*" | absoluteURI | abs_path | authority
	// 1) "*"				: We do not check this for now
	// 2) absoluteURI		: GET http://www.w3.org/pub/WWW/TheProject.html HTTP/1.1
	// 3) abs_path			: GET /pub/WWW/TheProject.html HTTP/1.1
	// 4) authoriy			: No idea what this is

	int	i = 0;

	if (_path[i] == 'h' && _path[i + 1] == 't' && _path[i + 2] == 't' && _path[i + 3] == 'p' 
		 && ((_path[i + 4] == 's' && _path[i + 5] == ':') || _path[i + 4] == ':'))
	{
		// absoluteURI => we cut the beginning, the information is in the Host and Authorize header as well
		std::size_t _path_pos = _path.find_first_of("//");
		std::size_t abs_path_pos = _path.substr(_path_pos + 2).find_first_of("/");
		_path = _path.substr(abs_path_pos);
	}
	std::size_t query_pos = _path.find_first_of("?");
	std::size_t fragment_pos = _path.find_first_of("#");
	if (_path == "/")
		_interpreter_info.abs_path = "/";
	else if (query_pos == std::string::npos && fragment_pos == std::string::npos)
		_interpreter_info.abs_path = uriDecode(_path);
	else if (fragment_pos == std::string::npos  && query_pos != std::string::npos)
	{
		// There is a query in _path
		_interpreter_info.abs_path = uriDecode(_path.substr(0, query_pos));
		_interpreter_info.query = uriDecode(_path.substr(query_pos));
	}
	else if (fragment_pos != std::string::npos  && query_pos == std::string::npos)
	{
		// There is a fragment in _path
		_interpreter_info.abs_path = uriDecode(_path.substr(0, fragment_pos));
		_interpreter_info.fragment = uriDecode(_path.substr(fragment_pos));
	}
	else
	{
		// There is a query and fragment in _path
		_interpreter_info.abs_path = uriDecode(_path.substr(0, query_pos));
		_interpreter_info.query = uriDecode(_path.substr(query_pos, fragment_pos - query_pos));
		_interpreter_info.fragment = uriDecode(_path.substr(fragment_pos));
	}
}