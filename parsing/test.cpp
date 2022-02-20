#include "Lexer.hpp"
#include "ConfigToken.hpp"
#include "ConfigParser.hpp"

template<typename T>
	void	print(const T &a) {
		std::cout << a << " ";
	}

template<typename T, typename F>
	void	print_pair(const std::pair<T, F> &a) {
		std::cout << "\t\t" << a.first << " " << a.second << std::endl;
	}

void	print_locations(const std::vector<location> &locations, int tabs) {
	std::vector<location>::const_iterator it = locations.begin();
	while (it != locations.end())
	{
		for (size_t i = 0; i < tabs - 1; i++) { LOG("\t"); }
		LOG("Location: \"") << it->_path << "\"" << std::endl;
		for (size_t i = 0; i < tabs - 1; i++) { LOG("\t"); }
		LOGN("-----------------------------");
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("Root: ") << it->_root << std::endl;
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("Upload: ") << (int)it->_upload << std::endl;
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("upload_path: ") << it->_upload_path << std::endl;
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("Cgi_extension: ") << it->_cgi_extension << std::endl;
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("Cgi_path: ") << it->_cgi_path << std::endl;
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("Methods: ");
		std::for_each(it->_methods.begin(), it->_methods.end(), print<std::string>);
		LOG(std::endl);
		for (size_t i = 0; i < tabs; i++) { LOG("\t"); }
		LOG("Index: ");
		std::for_each(it->_index.begin(), it->_index.end(), print<std::string>);
		LOG(std::endl);
		print_locations(it->_locations, tabs + 1);
		it++;
	}
	
}

void	print_servers(const std::vector<server> &servers) {
	std::vector<server>::const_iterator	it = servers.begin();
	while (it != servers.end()) {
		LOGN("Server: ")
		LOGN("-----------------------------")
		LOG("\tRoot: ") << it->_root << std::endl;
		LOG("\tClient_Max_Body_Size: ") << it->_client_max_body_size << std::endl;
		LOG("\tAutoindex: ") << (int)it->_autoindex << std::endl;
		LOG("\tServer_Names: ");
		std::for_each(it->_server_names.begin(), it->_server_names.end(), print<std::string>);
		LOG(std::endl);
		LOGN("\tError_Pages: ");
		std::for_each(it->_error_pages.begin(), it->_error_pages.end(), print_pair<int, std::string>);
		print_locations(it->_locations, 2);
		it++;
	}

}

void print_connections(const std::vector<connection> &connections ) {
	std::vector<connection>::const_iterator	it = connections.begin();
	while (it != connections.end()) {
		LOGN("Connection: ")
		LOGN("-----------------------------");
		LOG("Address: ") << it->_address << std::endl;
		LOG("Port: ") << it->_port << std::endl;
		print_servers(it->_servers);
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
		print_connections(p._connections);
	}
	catch (std::exception const &e)
	{
		std::cerr << "Error" << std::endl;
		std::cerr << e.what() << std::endl;
	}
}