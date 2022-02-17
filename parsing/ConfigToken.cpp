#include "ConfigToken.hpp"
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>

const std::string	ConfigToken::_seperators = "#;:{}";

const std::string	ConfigToken::identity[] = {
	"{",
	"}",
	"#",
	"\n",
	"server",
	"location",
	"root",
	"error_page",
	"client_max_body_size",
	"method",
	"GET",
	"POST",
	"DELETE",
	"index",
	"upload_enable",
	"on",
	"off",
	"listen",
	"cgi_extension",
	"cgi_path",
	"server_name",
	"autoindex",
	":",
	";",
	""
};

ConfigToken::ConfigToken(std::string const &content) : AToken(content), _scope() { }

static bool	is_number(std::string const &str) {
	std::string::const_iterator s = str.begin();
	while (s != str.end())
	{
		if (!std::isdigit(*s))
			return (false);
		s++;
	}
	return (true);
}

size_t	ConfigToken::getPos() const {
	for (size_t i = 0; identity[i] != ""; i++)
	{
		if (identity[i] == _content)
			return (i);
	}
	return (std::string::npos);
}

static bool	is_ip(const std::string &ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}

void	ConfigToken::classify() {
	size_t	pos = getPos();

	if (pos != std::string::npos)
		_type = pos;
	else if (is_number(_content))
		_type = INTEGER;
	else if (_content[0] == '.' || _content[0] == '/')
	{
		if (*_content.rbegin() == '/')
			_type = DIRECTORY;
		else
			_type = PATH; //can still be directory 
	}
	else if (is_ip(_content))
		_type = IP_ADDRESS;
	else
		_type = STRING;
}

/* returns 1 because special ConfigTokens are never longer than 1 */
size_t	ConfigToken::getSpecialLength(std::string const &content) const {
	for (size_t i = 0; _seperators[i]; i++)
	{
		if (content.find(_seperators[i]) != std::string::npos)
			return 1;
	}
	return (std::string::npos);
}

size_t	ConfigToken::isSeperator(std::string const &content) const {
	size_t	safe = std::string::npos;
	size_t	tmp = 0;
	for (size_t i = 0; _seperators[i]; i++)
	{
		tmp = content.find(_seperators[i]);
		if (tmp != std::string::npos && (tmp < safe || safe == std::string::npos))
			safe = tmp;
	}
	return (safe);
}

void	ConfigToken::setType(instrutions const &val) {
	_type = val;
}

size_t const	&ConfigToken::scope() const {
	return _scope;
}


void	ConfigToken::setScope(size_t const &scope) {
	_scope = scope;
}