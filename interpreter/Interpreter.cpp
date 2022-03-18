#include "Interpreter.hpp"


#include <string>
#include <vector>
#include <sstream>

// std::stringstream test("this_is_a_test_string");
// std::string segment;
// std::vector<std::string> seglist;

// while(std::getline(test, segment, '_'))
// {
//    seglist.push_back(segment);
// }

std::vector<std::string>	split_string(const std::string &s, const char &c) {
	std::stringstream ss;
	std::string	segment;

	ss << s;
	std::vector<std::string> seglist;
	while  (std::getline(ss, segment, c)) {
		if (segment != "")
			seglist.push_back(segment);
	}
	return seglist;
}

int		count_continues_matches(const std::vector<std::string> &path, const std::vector<std::string> &uri) {
	std::vector<std::string>::const_iterator	pit = path.begin();
	std::vector<std::string>::const_iterator	uit = uri.begin();
	size_t	count = 0;
	while (pit != path.end() && uit != uri.end()) {
		if (*pit != *uit)
			break;
		pit++;
		uit++;
		count++;
	}
	return count;
}

Interpreter::Interpreter(const Request &request, Connection *connection) : _request(request), _connection(connection) {
	_findHostname();
	std::vector<std::string>::const_iterator it = _server.getServerName().begin();
	while (it != _server.getServerName().end()) {
		LOG_BLUE("server-name: " << *it);
		it++;
	}
	LOG_RED("Finding location...");
	_findLocation(_server.getLocations());
}

Interpreter::~Interpreter() {

}

void	Interpreter::_findHostname() {
	_server = _connection->getServer(_request.getHost());
}

void	Interpreter::_findLocation(std::vector<location> const &l) {
	std::vector<location>::const_iterator it = l.begin();
	int		max = -1;
	while (it != l.end()) {

		std::cout << "Path: " << _request.getInterpreterInfo().abs_path << std::endl;
		std::cout << "Query: " << _request.getInterpreterInfo().query << std::endl;
		std::cout << "Frag: " << _request.getInterpreterInfo().fragment << std::endl;

		const std::vector<std::string>	sp = split_string(it->_path, '/');
		const std::vector<std::string>	&up = split_string(_request.getInterpreterInfo().abs_path, '/');
		int c = count_continues_matches(sp, up);
		LOG_RED("DBEUG: " << c);
		if (c > max) {
			LOG_BLUE("DEBUG2");
			_location = *it;
			max = c;
		}
		it++;
	}
	LOG_GREEN(_location._path << " is the most exact match for: " << _request.getInterpreterInfo().abs_path);
}
