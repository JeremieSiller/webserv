#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <vector>
#include <exception>
#include <map>
#include <set>
#include <iostream>
#include <limits>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <cstdlib>
#include "ConfigToken.hpp"
#include "utility.hpp"


#define LOG(x) std::cout << x
#define LOGN(x) std::cout << x << std::endl; 



typedef struct s_location {
	std::string						_path;
	std::string						_root;
	std::set<std::string>			_methods;
	std::vector<std::string>		_index;
	char							_upload;
	std::string						_upload_path;
	std::string						_cgi_extension;
	std::string						_cgi_path;
	char								_autoindex;
}	location;

typedef struct s_server {
	std::vector<location>				_locations;
	std::vector<std::string>			_server_names; //compare with host
	std::map<int, std::string>			_error_pages;
	std::string							_root;
	std::string							_client_max_body_size;
} server;

typedef struct s_connection {
	std::string		 _address;
	int				 _port;
	std::vector<server> _servers;
	s_connection() : _address(""), _port(-1), _servers() { };
} connection;

class ConfigParser
{
private:
	std::vector<ConfigToken>	&_tokens;
	void	_removeCommentsAndLineBreaks();
	void	_setScopes();
	void	_iterate();
	void	_checkConnection(std::vector<ConfigToken>::iterator &it);
	void	_checkServer(std::vector<ConfigToken>::iterator &it, connection &c);
	void	_checkLocation(std::vector<ConfigToken>::iterator &it, location &l);
public:
	std::vector<connection>	 _connections;
private:
	class   ScopeNotClosed;
	class   unexpectedToken;
public:
	ConfigParser(std::vector<ConfigToken> &tokens);
	~ConfigParser();
};
  

#endif