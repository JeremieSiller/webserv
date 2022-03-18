#include "Interpreter.hpp"

Interpreter::Interpreter(const Request &request, Connection *connection) : _request(request), _connection(connection) {
	_findHostname();
	_findLocation();
}

Interpreter::~Interpreter() {

}

void	Interpreter::_findHostname() {
	_server = _connection->getServer(_request.getHost());
}
