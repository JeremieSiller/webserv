#include "Lexer.hpp"
#include "ConfigToken.hpp"
#include "ConfigParser.hpp"

template<typename T>
	void	print(const T &a) {
		std::cout << a << " ";
	}

template<typename T, typename F>
	void	print_pair(const std::pair<T, F> &a) {
		std::cout "\t\t" << a.first << " " << a.second << std::endl;
	}

void	print_servers(const std::vector<server> &servers) {
	std::vector<server>::const_iterator	it = servers.begin();
	while (it != servers.end()) {
		LOGN("\tServer: ")
		LOGN("\t-----------------------------")
		LOG("\tRoot: ") << it->_root << std::endl;
		LOG("\tClient_Max_Body_Size: ") << it->_client_max_body_size << std::endl;
		LOG("\tAutoindex: ") << it->_autoindex << std::endl;
		LOG("\tServer_Names: ");
		std::for_each(it->_server_names.begin(), it->_server_names.end(), print<std::string>);
		LOG(std::endl);
		LOGN("\tError_Pages: ");
		std::for_each(it->_error_pages.begin(), it->_error_pages.end(), print_pair<int, std::string>);
	}

}

void print_connections(const std::vector<connection> &connections ) {
	std::vector<connection>::const_iterator	it = connections.begin();
	while (it != connections.end()) {
		LOGN("Connection: ")
		LOGN("-----------------------------");
		LOG("Address: ") << it->_address << std::endl;
		LOG("Port: ") << it->_port << std::endl;

		it++;
	}

}

int	main(int argc, char **argv)
{
	if (argc != 2) { //define PATH all the time just for testing purpose.
		std::cerr << "usage: " << argv[0] << " [PATHNAME]" << std::endl;
		return (1);
	}
	try
	{
		Lexer<ConfigToken> l(argv[1]);
		ConfigParser  p(l.getToken());
		std::vector<ConfigToken>::const_iterator begin = l.getToken().begin();
		std::vector<ConfigToken>::const_iterator end = l.getToken().end();
		print_connections(p._connections);
		// while (begin != end)
		// {
		// 	std::cout << "|";
		// 	if (begin->content() != "\n")
		// 			std::cout << begin->content();
		// 		else
		// 		std::cout << "\\n";
		// 	std::cout << "| "; 
		// 	if (begin->type() == ConfigToken::IP_ADDRESS)
		// 		std::cout << "IP";
		// 	else if (begin->type() == ConfigToken::INTEGER)
		// 		std::cout << "int";
		// 	else if (begin->type() == ConfigToken::DIRECTORY)
		// 		std::cout << "dir";
		// 	else if (begin->type() == ConfigToken::STRING)
		// 		std::cout << "str";
		// 	else if (begin->type() == ConfigToken::NEW_LINE)
		// 		std::cout << "\\n";
		// 	else if (begin->type() == ConfigToken::PATH)
		// 		std::cout << "path";
		// 	else
		// 		std::cout << begin->identity[begin->type()];
		// 	std::cout << " |" << begin->scope() << std::endl;
		// 	begin++;
		// }
	}
	catch (std::exception const &e)
	{
		std::cerr << "Error" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}