#include "server/webserv.hpp"
#include "parsing/Lexer.hpp"
#include "parsing/ConfigParser.hpp"

int	main(int argc, char **argv) {
	
		if (argc != 2) { //define PATH all the time just for testing purpose.
		std::cerr << "usage: " << argv[0] << " [PATHNAME]" << std::endl;
		return (1);
	}
	try
	{
		Lexer<ConfigToken> l(argv[1]);
		ConfigParser  p(l.getToken());
		webserv webserver(p);

		webserver.run();
	}
	catch (std::exception const &e)
	{
		std::cerr << "Error" << std::endl;
		std::cerr << e.what() << std::endl;
	}

}