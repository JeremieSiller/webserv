#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <unistd.h>
# include <stdio.h>
# include <map>
# include "../request/Request.hpp"
# include "../parsing/ConfigParser.hpp"

class cgi
{
private:
	char			**_env;
	FILE			*_input;
	FILE			*_output;
	const Request	&_req;
	location	&_loc;
	std::string		_path;
	void	_setEnv();
	void	_runCgi();
public:
	cgi(const Request &req, location &loc, const std::string &path);
	FILE	*getOutput();
	~cgi();
};



#endif