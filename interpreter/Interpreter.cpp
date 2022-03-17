#include "Interpreter.hpp"

Interpreter::Interpreter(const Request &request) : _request(request) {

}

Interpreter::~Interpreter() {

}

void	Interpreter::findHostname(const std::vector<Server> &servers) {
	std::vector<Server>::const_iterator it = servers.begin();
	while (it != servers.end())
	{
		if (*it == _request.getHost()) {
			_server = *it;
			return ;
		}
	}
	_server = *servers.begin();
}