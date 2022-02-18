#include "ConfigParser.hpp"
#include <arpa/inet.h>

static bool	is_ip(const std::string &ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
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

void	ConfigParser::_checkServer(std::vector<ConfigToken>::iterator &it) {
	
	size_t scope = it->scope();
	it++;
	while (it != _tokens.end() && it->scope() > scope) {
		it++;
	}
	it++;
}

void	ConfigParser::_checkConnection(std::vector<ConfigToken>::iterator &it) {
	size_t	scope = it->scope();
	connection	c;
	// LOGN(it->content());
	it ++;
	while (it != _tokens.end() && it->scope() > scope) {

		if (it->type() == ConfigToken::LISTEN) {
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
			// LOGN(it->content());
			_checkServer(it);
			// LOGN(it->scope());
			continue ;
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