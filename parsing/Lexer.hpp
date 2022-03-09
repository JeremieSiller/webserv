#ifndef LEXER_HPP
# define LEXER_HPP

# include <vector>
# include <cstdarg>
# include <iostream>
# include <sstream>
# include <string>
# include <iterator>
# include <fstream>

# define DEFAULT_PATH "./conf.conf" //TO-DO
# define FILE_NOT_FOUND "Could not find config file at: "

/**
 * Lexer splits stringstream into tokens which are saved in a vector
 * takes a Token as a template and classfies the tokens depedning on the Tokens classify function
 * Can also take a path to open a file and split it into tokens.
 * Throws excecption if file is not found
**/
template<typename Token>
class Lexer {
private:
/* execptions classes */
	class FileNotFound;
	class UnexpectedToken;
	class ScopeNotClosed;
private:
/* attributes */
	std::vector<Token>	_tokens;
	std::stringstream	_stream;
/* private member functions */
	void	_splitTokens();
	void	_validateTokens();
public:
/* public member functions */
	Lexer(std::stringstream const &stream);
	Lexer(std::string const &_path = DEFAULT_PATH);
	~Lexer() { };
	std::vector<Token> &getToken();
};

template<typename Token>
Lexer<Token>::Lexer(std::stringstream const &stream) : _tokens(), _stream()
{
	_stream << stream.rdbuf();
	_splitTokens();
	_validateTokens();
}

template<typename Token>
Lexer<Token>::Lexer(std::string const &_path)
		: _tokens(), _stream()
{
	std::ifstream file(_path.c_str());
	if (!file)
		throw FileNotFound(_path);
	_stream << file.rdbuf();
	_splitTokens();
	_validateTokens();
}

template<typename Token>
class Lexer<Token>::FileNotFound : public std::runtime_error {
public:
	FileNotFound(std::string const &path) : std::runtime_error(path) { }
};

template<typename Token>
void	Lexer<Token>::_splitTokens() {
	std::string							line;
	std::stringstream					tmp;
	std::istream_iterator<std::string>	end;
	std::istream_iterator<std::string>	start;
	while (std::getline(_stream, line))
	{
		tmp << line;
		start = tmp;
		while (start != end)
		{
			Token t(*start);
			if (t.isSeperator(*start) == std::string::npos)
			{
				_tokens.push_back(*start);
				start++;
				continue ;
			}
			line = *start;
			size_t i = t.isSeperator(line);
			while (i != std::string::npos && line.length() > 0)
			{
				if (i > 0)
					_tokens.push_back(line.substr(0, i));
				_tokens.push_back(line.substr(i, t.getSpecialLength(line)));
				line = line.substr(i + t.getSpecialLength(line));
				i = t.isSeperator(line);
			}
			if (line.length() > 0)
				_tokens.push_back(line);
			start++;
		}
		tmp.clear();
		_tokens.push_back(Token("\n"));
	}
}

template<class Token>
void	Lexer<Token>::_validateTokens() {
	typename std::vector<Token>::iterator	it = _tokens.begin();
	while (it != _tokens.end())
	{
		it->classify();
		if (it->type() == Token::COMMENT)
		{
			while (it != _tokens.end() && it->content() != "\n")
			{
				it->setType(Token::COMMENT);
				it++;
			}
			continue ;
		}
		it++;
	}
}



template<typename Token>
std::vector<Token> &Lexer<Token>::getToken() {
		return _tokens;
};


#endif