#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <unistd.h>
# include <stdio.h>
# include <map>
# include "../request/Request.hpp"
# include "../parsing/ConfigParser.hpp"
# include <sstream>

class cgi
{
private:
	std::map<std::string, std::string>	_env;
	const Request	&_req;
	location	&_loc;
	std::string		_path;
	FILE			*_input;
	FILE			*_output;
	void	_setEnv();
	void	_runCgi();
public:
	cgi(const Request &req, location &loc, const std::string &path);
	FILE	*getOutput();
	~cgi();
};

template<class T>
std::string	to_string(const T &t) {
	std::stringstream	ss;
	ss << t;
	return ss.str();
}


#endif