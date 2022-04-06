#include "server/webserv.hpp"
#include "parsing/Lexer.hpp"
#include "parsing/ConfigParser.hpp"
#include "main.hpp"
#include <signal.h>

int	main(int argc, char **argv) {
	signal(SIGPIPE, SIG_IGN);
	if (argc > 2) { //define PATH all the time just for testing purpose.
		std::cerr << "usage: " << argv[0] << " [PATHNAME]" << std::endl;
		return (1);
	}
	try
	{
		std::string path = "default/default.conf";
		if (argc == 2)
			path = argv[1];
		Lexer<ConfigToken> l(path);
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