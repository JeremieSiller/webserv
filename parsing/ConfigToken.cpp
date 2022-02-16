#include "ConfigToken.hpp"
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>

const std::string	ConfigToken::special_toekens = "#;:{}";

const std::string	ConfigToken::identity[] = {
	"{",
	"}",
	"#",
	"\n",
	"server",
	"location",
	"root", // directory ??
	"error_page",
	"client_max_body_size",
	"method",
	"GET",
	"POST",
	"DELETE",
	"index", // can this differ?
	"upload_enable",
	"on",
	"off",
	"listen",
	"cgi_extension",
	"cgi_path",
	"server_name",
	"autoindex", //needed??
	":",
	";",
	"localhost", // gonna be classified as ip
	""
};

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
	{
		_type = pos;
		if (_content == "localhost")
			_content = "127.0.0.1";
	}
	else if (is_number(_content))
		_type = INTEGER;
	else if (_content[0] == '/')
		_type = DIRECTORY;
	else if (_content.substr(0, 2) == "./")
		_type = PATH;
	else if (is_ip(_content))
		_type = IP_ADDRESS;
	else
		_type = STRING;
}

/* returns 1 because special ConfigTokens are never longer than 1 */
size_t	ConfigToken::getSpecialLength(std::string const &content) const {
	for (size_t i = 0; special_toekens[i]; i++)
	{
		if (content.find(special_toekens[i]) != std::string::npos)
			return 1;
	}
	return (std::string::npos);
}

size_t	ConfigToken::isSeperator(std::string const &content) const {
	size_t	safe = std::string::npos;
	size_t	tmp = 0;
	for (size_t i = 0; special_toekens[i]; i++)
	{
		tmp = content.find(special_toekens[i]);
		if (tmp != std::string::npos && (tmp < safe || safe == std::string::npos))
			safe = tmp;
	}
	return (safe);
}

// bool	ConfigToken::isAllowed_toFollow(AToken const &in) const {
// 	if (_type == EOF_INSTRUCT)
// 		return true;
// 	return false;
// }

// bool	ConfigToken::isAllowed_toPreceed(AToken const &in) const {
// 	if (_type == EOF_INSTRUCT)
// 		return true;
// 	return false;
// }

bool	ConfigToken::validate_syntax(std::vector<AToken>::const_iterator tokens, std::vector<AToken>::const_iterator const &end) const {
	// int	const &instruction = tokens->type();
	// if (instruction == SERVER)
	// {

	// }
	// while (tokens != end)
	// {
	// 	tokens++;
	// }
	return true;
}

void	ConfigToken::setType(instrutions const &val) {
	_type = val;
}
