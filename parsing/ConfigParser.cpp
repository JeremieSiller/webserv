#include "ConfigParser.hpp"

ConfigParser::ConfigParser(std::vector<ConfigToken> &tokens) :
    _tokens(tokens) {
	_removeCommentsAndLineBreaks();
	_setScopes();
	_checkScopes();
}

ConfigParser::~ConfigParser() {

}

class   ConfigParser::ScopeNotClosed : public std::exception {
public:
	const char *what() const throw () {
		return "Error: Scope not closed\n";
	}
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
		it->setScope(scope);
		if (it->type() == ConfigToken::SCOPE_END)
			scope--;
		it++;
	}
	if (scope != 0)
		throw ScopeNotClosed();
}

void	ConfigParser::_checkScopes() const {
	std::vector<ConfigToken>::const_iterator	it = _tokens.begin();
	while (it != _tokens.end()) {
		if (it->scope() == 0 && it->type() != ConfigToken::SERVER) {
			throw std::runtime_error("found unexpected token: \"" + it->content() + "\" in outer scope, expected: \"server\"");
		}
		if (it->scope() > 0 && it->type() == ConfigToken::SERVER) {
			throw std::runtime_error("found unexpected token: \"" + it->content() + "\" in inner scope");
		}
		it++;
	}
}