#include "response.hpp"

int	main(void) {
	response	r(200);
	r.add_header("Connection", "keep-alive");
	r.add_header("Server", "webserv");
	r.write_response(1);
}