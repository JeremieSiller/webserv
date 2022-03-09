#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#pragma once
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#define VERSION "HTTP/1.1"

class response
{
private:
	const int					&_statsusCode;
	const std::string			&_version;
	std::vector<char>			_bytes;
	void	_build();
	void	_buildFirstLine();
	void	_pushEndOfLine();
public:
	response(const int &status, const std::string &verison = VERSION);
	~response();
	int		write_response(const int &fd); //writes the full response to filedescriptor
	void	add_header(const std::string &attribute, const std::string &value); 
	void	add_header(const std::string &full_header);
};

#endif