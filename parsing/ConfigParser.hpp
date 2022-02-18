#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include <vector>
#include <exception>
#include <map>
#include <iostream>
#include <limits>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include "ConfigToken.hpp"

#define LOG(x) std::cout << x
#define LOGN(x) std::cout << x << std::endl; 



typedef struct s_location {
    std::string                         _root;
    std::vector<std::string>            _methods;
    std::vector<std::string>            _index;
    bool                                _upload;
    std::string                         _upload_path;
    std::string                         _cgi_extension;
    std::string                         _cgi_path;
    std::map<std::string, s_location>   _locations;
} location;

typedef struct s_server {
    std::map<std::string, location>     _locations;
    std::vector<std::string>            _server_names;
    std::map<int, std::string>          _error_pages;
    std::string                         _root;
    std::string                         _client_max_body_size;
    char                                _autoindex;
    s_server() : _locations(), _server_names(), _error_pages(), _root(), _autoindex() { }
} server;

typedef struct s_connection {
    std::string         _address;
    int                 _port;
    std::vector<server> _servers;
    s_connection() : _address(""), _port(-1), _servers() { };
} connection;

class ConfigParser
{
public:
    std::vector<connection>     _connections;
private:
    std::vector<ConfigToken>    &_tokens;
    void    _removeCommentsAndLineBreaks();
    void    _setScopes();
    void    _iterate();
    void    _checkConnection(std::vector<ConfigToken>::iterator &it);
    void    _checkServer(std::vector<ConfigToken>::iterator &it, connection &c);
    void    _checkLocation(std::vector<ConfigToken>::iterator &it);
    std::string _getAddressFromHost(std::string const &host);
    // void    _checkScopes();
    // void    _checkSyntax();
    // void    _checkKeywords();
/*  execptions */
private:
    class   ScopeNotClosed;
    class   unexpectedToken;
public:
    ConfigParser(std::vector<ConfigToken> &tokens);
    ~ConfigParser();
};
  

#endif