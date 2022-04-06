#include "ConfigParser.hpp"

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

/**
 * @brief removes all comments and line breaks from token list
 */
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

/**
 * @brief sets the scope depth for all tokens 
 * throws an error if a scope is not closed or there are
 * too many closing scopes. 
 */
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

void	ConfigParser::_checkLocation(std::vector<ConfigToken>::iterator &it, location &l) {
	size_t scope = it->scope();
	l._upload = -1;
	l._autoindex = -1;
	it++;
	if (it->type() != ConfigToken::PATH) {
		throw unexpectedToken(it->content(), ", location needs to have a path");
	}
	l._path = it->content();
	it++;
	if (it->scope() == scope) {
		throw unexpectedToken(it->content(), ", location needs a scope, expected \"{\"");
	}
	while (it != _tokens.end() && it->scope() > scope) {
		if (it->type() == ConfigToken::ROOT) {
			if (l._root != "") {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", root can not be empty");
			}
			l._root = it->content();
			if (!is_directory(l._root)) {
				throw unexpectedToken(it->content(), ", root needs to be a valid directory");
			}
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", root can not contain more than one path");
			}
		} else if (it->type() == ConfigToken::INDEX) {
			if (!l._index.empty()) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", index need atleast one arguement");
			}
			while (it->type() != ConfigToken::EOF_INSTRUCT && (it->type() == ConfigToken::STRING || it->type() == ConfigToken::PATH)) {
				l._index.push_back(it->content());
				it++;
			}
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", index indetifier can not be a keyword, expected \";\"");
			}
		} else if (it->type() == ConfigToken::METHOD) {
			if (!l._methods.empty()) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", method need atleast one arguement");
			}
			while (it->type() != ConfigToken::EOF_INSTRUCT && (it->type() == ConfigToken::POST || it->type() == ConfigToken::DELTE || it->type() == ConfigToken::GET || it->type() == ConfigToken::PUT || it->type() == ConfigToken::HEAD)) { //TODO
				if (l._methods.insert(it->content()).second == false) {
					throw unexpectedToken(it->content(), ", specific method can not be set twice");
				}
				it++;
			}
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", method identifier has to be method, expected \";\"");
			}
		} else if (it->type() == ConfigToken::UPLOAD_ENABLE) {
			if (l._upload != -1) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() != ConfigToken::OFF && it->type() != ConfigToken::ON) {
				throw unexpectedToken(it->content(), ", upload needs to have paramter \'on\' or \'off\'");
			}
			if (it->type() == ConfigToken::ON) {
				l._upload = true;
			} else if (it->type() == ConfigToken::OFF) {
				l._upload = false;
			}
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", upload needs exaclty one arguement");
			}
		} else if (it->type() == ConfigToken::UPLOAD_PATH) {
			if (l._upload_path != "") {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", upload_path can not be empty");
			}
			l._upload_path = it->content();
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", upload_path can not contain more than one path");
			}
		} else if (it->type() == ConfigToken::REDIRECT) {
			if (l._redirect_path != "") {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			while(it->type() == ConfigToken::DOUBLE_DOT || it->type() == ConfigToken::STRING || it->type() == ConfigToken::PATH) {
				l._redirect_path += it->content();
				it++;
			}
			if (it->type() == ConfigToken::EOF_INSTRUCT && l._redirect_path == "") {
				throw unexpectedToken(it->content(), ", redirect can not be empty");
			}
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", redirect can not contain this token");
			}
		} else if (it->type() == ConfigToken::CGI_PATH) {
			if (l._cgi_path != "") {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", _cgi_path can not be empty");
			}
			l._cgi_path = it->content();
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", _cgi_path can not contain more than one path");
			}
		} else if (it->type() == ConfigToken::CGI_EXTENSION) {
			if (l._cgi_extension != "") {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", _cgi_extension can not be empty");
			}
			l._cgi_extension = it->content();
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", _cgi_extension can not contain more than one path");
			}
		} else if (it->type() == ConfigToken::AUTO_INDEX) {
			if (l._autoindex != -1) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() != ConfigToken::OFF && it->type() != ConfigToken::ON) {
				throw unexpectedToken(it->content(), ", autoindex needs to have paramter \'on\' or \'off\'");
			}
			if (it->type() == ConfigToken::ON) {
				l._autoindex = true;
			} else if (it->type() == ConfigToken::OFF) {
				l._autoindex = false;
			}
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", autoindex needs exaclty one arguement");
			}
		} else if (it->type() == ConfigToken::MAX_BODY_SIZE) {
			if (l._client_max_body_size != 0) {
				throw unexpectedToken(it->content(), ", can not be set twice");
			}
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", client_max_body_size can not be empty");
			}
			if (it->type() != ConfigToken::INTEGER) {
				throw unexpectedToken(it->content(), ", client_max_body_size must be size_t");
			}
			try
			{
				std::stringstream sstream(it->content());
				sstream >> l._client_max_body_size;
			} catch (std::exception const &e) {
					throw unexpectedToken(it->content(), ", not a valid PORT, needs to fit in a short");
			}
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", client_max_body_size can not contain more than one arguement");
			}
		} else if (it->type() != ConfigToken::SCOPE_END && it->type() != ConfigToken::SCOPE_START && it->type() != ConfigToken::EOF_INSTRUCT) {
			throw unexpectedToken(it->content(), ", can not be in location scope");
		} 
		
		it++;
	}
	if (l._autoindex == -1) {
		l._autoindex = false;
	}
	if (l._upload == -1) {
		l._upload = false;
	}
	if (l._upload == true && l._upload_path == "") {
		throw std::runtime_error("upload can not be enabled without upload_path");
	}
	if (l._cgi_extension != "" && l._cgi_path == "") {
		throw std::runtime_error("cgi can not be enabled without cgi_path");
	}
}

void	ConfigParser::_checkServer(std::vector<ConfigToken>::iterator &it, connection &c) {
	size_t scope = it->scope();
	server s;
	s._error_pages[301] = "standard-html/301.html";
	s._error_pages[400] = "standard-html/400.html";
	s._error_pages[403] = "standard-html/403.html";
	s._error_pages[404] = "standard-html/404.html";
	s._error_pages[405] = "standard-html/405.html";
	s._error_pages[413] = "standard-html/413.html";
	s._error_pages[500] = "standard-html/500.html";
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
		} else if (it->type() == ConfigToken::ERROR_PAGE) {
			it++;
			int	error_code;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", error_page needs exactly two arguements (error code & path to file)");
			}
			if (it->type() != ConfigToken::INTEGER || (it->content()[0] != '5' && it->content()[0] != '4') || it->content().length() != 3 ) {
				throw unexpectedToken(it->content(), ", error_page first arguement needs to be a valid error code (4XX or 5XX)");
			}
			error_code = std::atoi(it->content().c_str());
			it++;
			if (it->type() == ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", error_page needs exactly two arguements (error code & path to file)");
			}
			if (!is_file(it->content())) {
				throw unexpectedToken(it->content(), "could not find file");
			}
			s._error_pages[error_code] = it->content();
			it++;
			if (it->type() != ConfigToken::EOF_INSTRUCT) {
				throw unexpectedToken(it->content(), ", error_page needs exactly two arguements (error code & path to file)");
			}
		} else if (it->type() == ConfigToken::LOCATION) {
			location l;
			_checkLocation(it, l);
			s._locations.push_back(l);
		} else if (it->type() != ConfigToken::SCOPE_START && it->type() != ConfigToken::SCOPE_END && it->type() != ConfigToken::EOF_INSTRUCT) {
			throw unexpectedToken(it->content(), ", can not be in server scope");
		}
		it++;
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
						c._port = std::atoi(it->content().c_str());
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
					c._address = getAddressFromHost(it->content());
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