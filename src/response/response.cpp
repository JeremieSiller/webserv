#include "response.hpp"
#include <iostream>

/**
 * @brief get the reason message according to a status code
 * 
 * @param code http status code
 * @return std::string const 
 */
static std::string const getReason(const int &code) {
	switch (code)
	{
	case 101:
		return "101 Continue";
	case 102:
		return "102 Switching Protocols";
	case 103:
		return "103 Processing";
	case 104:
		return "104 Early Hints";
	case 200:
		return "200 OK";
	case 201:
		return "201 Created";
	case 202:
		return "202 Accepted";
	case 203:
		return "203 Non-Authoritative Information";
	case 204:
		return "204 No Content";
	case 205:
		return "205 Reset Content";
	case 206:
		return "206 Partial Content";
	case 207:
		return "207 Multi-Status";
	case 208:
		return "208 Already Reported";
	case 226:
		return "226 IM Used";
	case 300:
		return "300 Multiple Choices";
	case 301:
		return "301 Moved Permanently";
	case 302:
		return "302 Found";
	case 303:
		return "303 See Other";
	case 304:
		return "304 Not Modified";
	case 305:
		return "305 Use Proxy";
	case 306:
		return "306 unused";
	case 307:
		return "307 Temporary Redirect";
	case 308:
		return "308 Permanent Redirect";
	case 400:
		return "400 Bad Request";
	case 401:
		return "401 Unauthorized";
	case 402:
		return "402 Payment Required";
	case 403:
		return "403 Forbidden";
	case 404:
		return "404 Not Found";
	case 405:
		return "405 Method Not Allowed";
	case 406:
		return "406 Not Acceptable";
	case 407:
		return "407 Proxy Authentication Required";
	case 408:
		return "408 Request Timeout";
	case 409:
		return "409 Conflict";
	case 410:
		return "410 Gone";
	case 411:
		return "411 Length Required";
	case 412:
		return "412 Precondition Failed";
	case 413:
		return "413 Payload Too Large";
	case 414:
		return "414 URI Too Long";
	case 415:
		return "415 Unsupported Media Type";
	case 416:
		return "416 Range Not Satisfiable";
	case 417:
		return "417 Expectation Failed";
	case 418:
		return "418 I'm a teapot";
	case 421:
		return "421 Misdirected Request";
	case 422:
		return "422 Unprocessable Entity";
	case 423:
		return "423 Locked";
	case 424:
		return "424 Failed Dependency";
	case 425:
		return "425 Too Early";
	case 426:
		return "426 Upgrade Required";
	case 428:
		return "428 Precondition Required";
	case 429:
		return "429 Too Many Requests";
	case 431:
		return "431 Request Header Fields Too Large";
	case 451:
		return "451 Unavailable For Legal Reasons";
	case 500:
		return "500 Internal Server Error";
	case 501:
		return "501 Not Implemented";
	case 502:
		return "502 Bad Gateway";
	case 503:
		return "503 Service Unavailable";
	case 504:
		return "504 Gateway Timeout";
	case 505:
		return "505 HTTP Version Not Supported";
	case 506:
		return "506 Variant Also Negotiates";
	case 507:
		return "507 Insufficient Storage";
	case 508:
		return "508 Loop Detected";
	case 509:
		return "509 Not Extended";
	case 511:
		return "511 Network Authentication Required";
	default:
		return "500 Unknown Server Error";
	}
}

/**
 * @brief Construct a new response::response object
 * 
 * @param status  sets the status code of the reponse, e.g. 200
 * @param version sets the http version, standard value is HTTP/1.1
 */
response::response(const int &status, const std::string &version) : _statusCode(status), _version(version), _header(), _body(), _state(START_WRITING) {
	_build();
}

/**
 * @brief Destroy the response::response object
 * 
 */
response::~response() { }

/**
 * @brief writes the full response to the passed file descriptor
 * 
 * @param fd a file descriptor (should be a socket)
 * @return int returns the value of the write-call
 */
int	response::write_response(const int &fd) {
	if (_state == START_WRITING) {
		_pushEndOfLine();
		LOG_BLUE("Response: ");
		for (size_t i = 0; i < _header.size(); i++)
		{
			std::cout << _header[i];
		}
		LOG_BLUE("------");
		_header.insert(_header.end(), _body.begin(), _body.end());
	}
	ssize_t	tmp = write(fd, _header.begin().base(), _header.size());
	if (tmp < static_cast<ssize_t>(_header.size())) {
		if (tmp == -1) {
			_state = START_WRITING;
			return -1;
		} else {
			_header.erase(_header.begin(), _header.begin() + tmp);
			_state = WRITING;
			return 1;
		}
	} else {
		_state = START_WRITING;
		return 0;
	}
}

/**
 * @brief builds the response header and body
 * 
 */
void	response::_build() {
	_buildFirstLine();
}

/**
 * @brief	pushes the eond of line initializer of http (\r\n) 
 * to the end of the vector 
 */
void	response::_pushEndOfLine() {
	_header.push_back('\r');
	_header.push_back('\n');
}

/**
 * @brief	build the first line of the response 
 * looking like e.g. this: "HTTP/1.1 200 ok\r\n"
 */
void	response::_buildFirstLine() {
	_header.insert(_header.end(), _version.begin(), _version.end());
	_header.push_back(' ');
	std::string reason = getReason(_statusCode);
	_header.insert(_header.end(), reason.begin(), reason.end());
	_pushEndOfLine();
}

/**
 * @brief adds header to response
 * e.g. "Server: webserv\r\n" or "Connection: keep-alive"
 * 
 * @param attribute the directive keyword
 * @param value the vaue to be put after the keyword
 */
void	response::add_header(const std::string &attribute, const std::string &value) {
	_header.insert(_header.end(), attribute.begin(), attribute.end());
	_header.push_back(':');
	_header.push_back(' ');
	_header.insert(_header.end(), value.begin(), value.end());
	_pushEndOfLine();
}

/**
 * @brief adds the body to the end of the reponse
 * automatically inserts a EOL so the headers end is defined with two EOL's (\r\n\r\n)
 * @param _body a vector that contains all bytes to add to the body
 */
void	response::add_body(const std::vector<char> &body) {
	_body.insert(_body.end(), body.begin(), body.end());
}


response::response() : _statusCode(), _version(), _header(), _body(), _state(START_WRITING) {}
