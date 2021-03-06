#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# pragma once
# include <string>
# include <vector>
# include <map>
# include <unistd.h>
# define VERSION "HTTP/1.1"
# include "../main.hpp"

class response
{
public:
	typedef enum e_state {
		START_WRITING,
		WRITING,
	} t_state;
private:
	int					_statusCode;
	std::string			_version;
	std::vector<char>	_header;
	std::vector<char>	_body;
	t_state				_state;
	void	_build();
	void	_buildFirstLine();
	void	_pushEndOfLine();
public:
	response();
	response(const int &status, const std::string &verison = VERSION);
	~response();
	int		write_response(const int &fd); //writes the full response to filedescriptor
	void	add_header(const std::string &attribute, const std::string &value);
	void	add_body(const std::vector<char> &body, const size_t &offset);
	const	t_state	&getState() const { return  _state; }
};

#endif