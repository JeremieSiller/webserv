#include "ConfigParser.hpp"
#include <arpa/inet.h>

/**
 * @brief returns if string is a valid ip_address
 * 
 * @param ipAddress 
 * @return true 
 * @return false 
 */
static bool	is_ip(const std::string &ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}

static bool is_directory(const std::string &path)
{
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0)
		return (0);
	return !S_ISREG(buf.st_mode);
}

static bool is_file(const std::string &path)
{
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0)
		return (0);
	return S_ISREG(buf.st_mode);
}


ConfigParser::ConfigParser(std::vector<ConfigToken> &tokens) :
    _tokens(tokens), _connections() {
	_removeCommentsAndLineBreaks();
	_setScopes();
	_iterate();
}

ConfigParser::~ConfigParser() {

}

class   ConfigParser::ScopeNotClosed : public std::exception {
public:
	const char *what() const throw () {
		return "Scope not closed\n";
	}
};

class	ConfigParser::unexpectedToken : public std::runtime_error {
public:
	unexpectedToken(std::string const &content, std::string const &specific = std::string("")) 
		: std::runtime_error("unexpected token: \"" + content + "\"" + specific) { }
};

void	ConfigParser::_removeCommentsAndLineBreaks() {
	std::vector<ConfigToken>::iterator	it = _tokens.begin();
	while (it != _tokens.end()) {
		if (it->type() == ConfigToken::NEW_LINE || it->type() == ConfigToken::COMMENT) {
			_tokens.erase(it);
		} else {
			it++;
		}
	}
}

void    ConfigParser::_setScopes() {
	std::vector<ConfigToken>::iterator	it = _tokens.begin();
	int											scope = 0;
	while (it != _tokens.end())
	{
		if (it->type() == ConfigToken::SCOPE_START)
			scope++;
		if (it->type() == ConfigToken::SCOPE_END)
			scope--;
		it->setScope(scope);
		it++;
	}
	if (scope != 0)
		throw ScopeNotClosed();
}

/**
 * @brief reads file /etc/hosts and searches for @param host 
 * @return std::string corresponding ip address
 */
std::string ConfigParser::_getAddressFromHost(std::string const &host) {
	std::ifstream						file("/etc/hosts");
	std::string							r = "";
	std::stringstream 					stream;
	std::stringstream					tmp;
	std::string							start_line;
	std::istream_iterator<std::string>	start;
	std::string							line;
	if (!file)
		return r;
	stream << file.rdbuf();
	while (std::getline(stream, line)) {
		std::istream_iterator<std::string>	end;
		tmp << line;
		start = tmp;
		start_line = *start;
		while (start != end) {
			if (*start == host) {
				if (start != end) {
					if (r != "" && is_ip(start_line)) {
						throw std::runtime_error("duplicate symbols for hostname: " + host + " in /etc/hosts");
					}
					if (is_ip(start_line))
						r = start_line;
				}
				tmp.clear();
				break ;
			}
			start++;
		}
		tmp.clear();
	}
	return r;
}

void	ConfigParser::_checkServer(std::vector<ConfigToken>::iterator &it, connection &c) {
	size_t scope = it->scope();
	server s;
	s._autoindex = -1;
	it++;
	while (it != _tokens.end() && it->scope() > scope) {
		if (it->type() == ConfigToken::ROOT) {
			if (s._root != "") {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", root can not be empty");
			}
			s._root = it->content();
			if (!is_directory(s._root)) {
				throw unexpectedToken(it->content(), ", root needs to be a valid directory");
			}
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", root can not contain more than one path");
			}
		} else if (it->type() == ConfigToken::SERVER_NAME) {
			if (!s._server_names.empty()) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", server_name needs at least one arguement");
			}
			while (it->type() != ConfigToken::EOF_INSTRUCT) {
				s._server_names.push_back(it->content());
				it++;
			}
		} else if (it->type() == ConfigToken::AUTO_INDEX) {
			if (s._autoindex != -1) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() != ConfigToken::OFF && it->type() != ConfigToken::ON) {
				throw unexpectedToken(it->content(), ", autoindex needs to have paramter \'on\' or \'off\'");
			}
			if (it->type() == ConfigToken::ON) {
				s._autoindex = true;
			} else if (it->type() == ConfigToken::OFF) {
				s._autoindex = false;
			}
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", autoindex needs exaclty one arguement");
			}
		} else if (it->type() == ConfigToken::ERROR_PAGE) {
			it++;
			int	error_code;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", error_page needs exactly two arguements (error code & path to file)");
			}
			if (it->type() != ConfigToken::INTEGER || (it->content()[0] != '5' && it->content()[0] != '4') || it->content().length() != 3 ) {
				throw unexpectedToken(it->content(), ", error_page first arguement needs to be a valid error code (4XX or 5XX)");
			}
			error_code = std::stoi(it->content());
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", error_page needs exactly two arguements (error code & path to file)");
			}
			if (!is_file(it->content())) {
				throw unexpectedToken(it->content(), "could not find file");
			}
			if (s._error_pages[error_code] != "") {
				throw unexpectedToken(std::to_string(error_code), ", can not set error_page twice");
			}
			s._error_pages[error_code] = it->content();
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", error_page needs exactly two arguements (error code & path to file)");
			}
		} else if (it->type() == ConfigToken::LOCATION) {
			location l;
			s._locations.push_back(l);
			// s._locations[0]._locations.push_back(location());
		}
		it++;
	}
	if (s._autoindex == -1) {
		s._autoindex = 0;
	}
	c._servers.push_back(s);
	it++;
}

void	ConfigParser::_checkConnection(std::vector<ConfigToken>::iterator &it) {
	size_t	scope = it->scope();
	connection	c;
	it ++;
	while (it != _tokens.end() && it->scope() > scope) {

		if (it->type() == ConfigToken::LISTEN) {
			if (c._port != -1 && c._address != "") {
				throw unexpectedToken(it->content(), ", listen directive can not be set twice");
			}
			it++;
			while (it != _tokens.end() && it->type() != ConfigToken::EOF_INSTRUCT) {
				if (it->type() == ConfigToken::INTEGER) {
					if (c._port != -1) {
						throw unexpectedToken(it->content(), ", PORT can not be set twice for one connection");
					}
					try
					{
						c._port = std::stoi(it->content());
						if (c._port < 0 || c._port > 65535){
							throw unexpectedToken(it->content(), ", not a valid PORT, needs to fit in a short");
						}
					} catch (std::exception const &e) {
							throw unexpectedToken(it->content(), ", not a valid PORT, needs to fit in a short");
					}
				} else if (it->type() == ConfigToken::IP_ADDRESS) {
					if (c._address != "") {
						throw unexpectedToken(it->content(), ", HOSTNAME can not be set twice for one connection");
					}
					c._address = it->content();
				} else {
					if (c._address != "") {
						std::cerr << "addr: " << c._address << std::endl;
						throw unexpectedToken(it->content(), ", HOSTNAME can not be set twice for one connection");
					}
					c._address = _getAddressFromHost(it->content());
					if (c._address == "" || !is_ip(c._address)) {
						throw unexpectedToken(it->content(), ", not a valid HOSTNAME, format: X.X.X.X or in /etc/hosts");
					}
				}
				it++;
			}
			if (c._address == "" && c._port == -1) {
				throw std::runtime_error("LISTEN keyword atleast needs a PORT or a HOSTNAME");
			}
		}
		else if (it->type() == ConfigToken::SERVER) {
			_checkServer(it, c);
			continue ;
		}
		else if (it->type() != ConfigToken::SCOPE_START && it->type() != ConfigToken::SCOPE_END && it->type() != ConfigToken::EOF_INSTRUCT) {
			throw unexpectedToken(it->content(), " connection block can only contain listen directive and server(s)");
		}
		it++;
	}
	it++;
	if (c._address == "")
		c._address = "0.0.0.0";
	if (c._port == -1)
		c._port = 80;
	_connections.push_back(c);
	// LOGN(it->content());
}

void	ConfigParser::_iterate() {
	std::vector<ConfigToken>::iterator	it = _tokens.begin();
	while (it != _tokens.end()) {
		if (it->type() == ConfigToken::CONNECTION) {
			_checkConnection(it);
			continue ;
		}
		it++;
	}
}