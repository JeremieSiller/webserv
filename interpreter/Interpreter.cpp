#include "Interpreter.hpp"

Interpreter::Interpreter(const Request &request, Connection *connection) : _request(request), _connection(connection) {
	_findHostname();
	std::vector<std::string>::const_iterator it = _server.getServerName().begin();
	while (it != _server.getServerName().end()) {
		LOG_BLUE("server-name: " << *it);
		it++;
	}
	// _findLocation();
}

Interpreter::~Interpreter() {

}

void	Interpreter::_findHostname() {
	_server = _connection->getServer(_request.getHost());
}
