/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/09 16:53:29 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/28 19:06:35 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Request.hpp"
#include <sstream>
#include "../server/client.hpp"



Request::Request() : _header(), _body(), _parsedHeader(),_version(), _path(), _method(), _headerStatus(HEADER), _chunksize() , _host(), _contentLength(), _transferEncoding(), _connection(true), _expect(false), _contenttype(), _location(), _server(), _accept() {this->_crlfvec = std::vector<char>(CRLF, CRLF + 2);}

void Request::setHeader(std::string const &header)
{
	this->_header += header;
	this->_headerStatus = BODY;
	if(!this->parseHeader())
		this->_headerStatus = INVALID;
}

int Request::parseHeader()
{
	size_t pos = 0;
	size_t begin = 0;

	// Get First line
	// METHOD PATH HTTPVER \r\n
	pos = this->_header.find(' ');
	if (pos == std::string::npos)
		return 0;
	this->_method = std::string(this->_header.substr(0, pos));

	begin = pos;
	pos = this->_header.find(' ', begin + 1);
	if (pos == std::string::npos)
		return 0;
	this->_path = std::string(this->_header.substr(begin + 1, pos - (begin + 1)));

	begin = pos;
	pos = this->_header.find(CRLF, begin + 1);
	if (pos == std::string::npos)
		return 0;
	this->_version = std::string(this->_header.substr(begin + 1, pos - (begin + 1)));

	begin = pos;
	// begin == \r
	std::string headerName = "";
	std::string options = "";
	while (true)
	{
		// HEADERNAME: OPTIONS, OPTIONS
		// store everything in map then in individual things

		pos = this->_header.find(": ", begin + 2);
		// if it cant find it we must be at the end of headers section
		if (pos == std::string::npos)
		{
			pos = this->_header.find(CRLF, begin + 2);
			if (pos == std::string::npos)
				return 0;
			break;
		}
		headerName = std::string(this->_header.substr(begin + 2, pos - (begin + 2)));
		if (this->_parsedHeader.count(headerName))
			this->_parsedHeader[headerName] += ',';
		else
			this->_parsedHeader.insert(std::pair<std::string, std::string>(headerName, ""));
		
		// begin == ':'
		// needs to find crlf after the ': '
		// 
		begin = pos;
		pos = this->_header.find(CRLF, begin + 2);
		if (pos == std::string::npos)
			return 0;
		this->_parsedHeader[headerName] += std::string(this->_header.substr(begin + 2, pos - (begin + 2)));
		begin = pos;
	}
	
	if (this->_parsedHeader.count("Host"))
	{
		// port is optional
		// HOST: <NAME>:<PORT>
		pos = this->_parsedHeader["Host"].find(':');
		if (pos == std::string::npos)
			this->_host = this->_parsedHeader["Host"];
		else
			this->_host = std::string(this->_parsedHeader["Host"].substr(0, pos));
	}
	else
		this->_headerStatus = INVALID;

	if (this->_parsedHeader.count("Transfer-Encoding"))
		this->_transferEncoding = this->_parsedHeader["Transfer-Encoding"];
	else if (this->_parsedHeader.count("Content-Length"))
		this->_contentLength = ::atoi(this->_parsedHeader["Content-Length"].c_str());
	
	if (this->_parsedHeader.count("Connection") && this->_parsedHeader["Connection"] == "Close")
		this->_connection = false;
	
	if (this->_parsedHeader.count("Expect") && this->_parsedHeader["Expect"] == "100-continue")
		this->_expect = true;

	if (this->_parsedHeader.count("Location"))
		this->_location = this->_parsedHeader["Location"];
	
	if (this->_parsedHeader.count("Server"))
		this->_server = this->_parsedHeader["Server"];

	this->prepareInterpreter();
	return 1;
}

int strHexDec(std::string str)
{
	std::cout << "size: 0x" << str;
	for (size_t i = 0; i < str.length(); ++i)
		str[i] = std::toupper(str[i]);
	int bruh = 0;
	for (size_t i = 0; i < str.length(); ++i)
		bruh += ((str[i] >= 'A') ? (str[i] - 'A' + 10) : (str[i] - '0')) * (1 << ((str.length() - 1 - i) * 4));
	std::cout << " : " << bruh << std::endl;
	return bruh;
}

std::vector<char> Request::_parseChunked(std::vector<char>::const_iterator start, std::vector<char>::const_iterator end)
{
	static std::vector<char>	rest;
	std::vector<char>			ret;
	std::vector<char>::const_iterator search;
	std::vector<char>			copy;
	
	copy.reserve(8000);
	if (rest.size() > 0)
		copy.insert(copy.end(), rest.begin(), rest.end());
	copy.insert(copy.end(), start, end);
	rest.clear();

	start = copy.begin();
	end = copy.end();
	
	while (start != end)
	{
		while (this->_chunksize > 0)
		{
			ret.push_back(*(start++));
			this->_chunksize--;
			if (start == end)
				return ret;
		}

		search = std::search(start, end, this->_crlfvec.begin(), this->_crlfvec.end());
		// incomplete \r\n 
		if (search == end)
		{
			rest.insert(rest.end(), start, end);
			return ret;
		}
		// we caught first \r\n not complete shit
		if (!isdigit(*start))
		{
			start += 2;
			search = std::search(start, end, this->_crlfvec.begin(), this->_crlfvec.end());
			if (search == end)
			{
				rest.insert(rest.end(), start, end);
				return ret;
			}
		}
		// now between start and search there is the number...
		this->_chunksize = strHexDec(std::string(start, search));
		this->_contentLength += this->_chunksize;
		if (this->_chunksize == 0)
		{
			this->_headerStatus = COMPLETE;
			return ret;
		}
		start = search + 2;
	}
	return ret;
}

void Request::addBody(std::vector<char>::const_iterator start, std::vector<char>::const_iterator end)
{
	LOG_RED(this->_header);
	LOG_RED(this->_contentLength);
	if (this->_parsedHeader["Transfer-Encoding"].find("chunked") != std::string::npos || this->_parsedHeader["Transfer-Encoding"] == "chunked")
	{
		std::vector<char> newbuf = this->_parseChunked(start, end);
		this->_body.insert(_body.end(), newbuf.begin(), newbuf.end());
	}
	else
	{
		this->_body.insert(_body.end(), start, end);
		if (this->_body.size() >= this->_contentLength)
			this->_headerStatus = COMPLETE;
	}
}

void Request::clear()
{
	this->_header = "";
	this->_body.clear();
	this->_parsedHeader = std::map<std::string, std::string>();
	this->_version = "";
	this->_path = "";
	this->_method = "";
	this->_headerStatus = HEADER;
	this->_chunksize = 0;
	this->_host = "";
	this->_contentLength = 0;
	this->_transferEncoding = "";
	this->_connection = true;
	this->_expect = false;
	this->_contenttype ="";
	this->_location = "";
	this->_server = "";
	this->_accept = std::list<std::string>();
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
		_interpreter_info.query = uriDecode(_path.substr(query_pos + 1));
	}
	else if (fragment_pos != std::string::npos  && query_pos == std::string::npos)
	{
		// There is a fragment in _path
		_interpreter_info.abs_path = uriDecode(_path.substr(0, fragment_pos));
		_interpreter_info.fragment = uriDecode(_path.substr(fragment_pos + 1));
	}
	else
	{
		// There is a query and fragment in _path
		_interpreter_info.abs_path = uriDecode(_path.substr(0, query_pos));
		_interpreter_info.query = uriDecode(_path.substr(query_pos + 1, fragment_pos));
		_interpreter_info.fragment = uriDecode(_path.substr(fragment_pos + 1));
	}
	return 0;
}