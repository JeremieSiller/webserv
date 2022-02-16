#include "Lexer.hpp"
#include "ConfigToken.hpp"

int	main(int argc, char **argv)
{
	if (argc != 2) { //define PATH all the time just for testing purpose.
		std::cerr << "usage: " << argv[0] << " [PATHNAME]" << std::endl;
		return (1);
	}
	try
	{
		Lexer<ConfigToken> l(argv[1]);
		std::vector<ConfigToken>::const_iterator begin = l.getToken().begin();
		std::vector<ConfigToken>::const_iterator end = l.getToken().end();
		while (begin != end)
		{
			std::cout << "|";
			if (begin->content() != "\n")
					std::cout << begin->content();
				else
				std::cout << "\\n";
			std::cout << "| "; 
			if (begin->type() == ConfigToken::IP_ADDRESS)
				std::cout << "IP";
			else if (begin->type() == ConfigToken::INTEGER)
				std::cout << "int";
			else if (begin->type() == ConfigToken::DIRECTORY)
				std::cout << "dir";
			else if (begin->type() == ConfigToken::STRING)
				std::cout << "str";
			else if (begin->type() == ConfigToken::NEW_LINE)
				std::cout << "\\n";
			else if (begin->type() == ConfigToken::PATH)
				std::cout << "path";
			else
				std::cout << begin->identity[begin->type()];
			std::cout << " |" << begin->scope() << std::endl;
			begin++;
		}
	}
	catch (std::exception const &e)
	{
		std::cerr << e.what() << std::endl;
	}
}