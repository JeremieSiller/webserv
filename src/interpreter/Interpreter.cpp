#include "Interpreter.hpp"


#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "cgi.hpp"
#include <fcntl.h>
#include "utils.hpp"

Interpreter::Interpreter() :
	_request(), _connection(), _response(), _server(), _location(), _state(), _full_path(), _file(), _iscgi() {
}

Interpreter::Interpreter(Request *request, Connection *connection) :
	_request(request), _connection(connection), _response(), _server(), _location(), _state(), _full_path(), _file(), _iscgi() {

}

Interpreter::~Interpreter() {

}

void Interpreter::execute() {
	if (_request->getStatus() == Request::INVALID) {
		_buildError(400);
		return ;
	}
	_findHostname();
	_findLocation(_server.getLocations());
	if (_state == 1)
		return ;
	_checkMethods();
	if (_state == 1)
		return ;
	_checkBodySize();
	if (_state == 1)
		return ;
	_checkRedirect();
	if (_state == 1)
		return ;
	_appendLocationToRoot();
	if (_request->getInterpreterInfo().abs_path.back() == '/') {
		_findDirectory();
	} else {
		_findFile();
	}
}

/**
 * @brief finds the server according to the hostname in the header
 */
void	Interpreter::_findHostname() {
	_server = _connection->getServer(_request->getHost());
}

/**
 * @brief finds the most accurate location according to the path in HTTP
 * e.g. request: GET /www/doc/ HTTP/1.1 would find /www/doc/ as a location or /www/ if /www/doc does not exist and / if /www doees not exist
 * if no locations matches 404 is build
 * @param l vector of locations
 */
void	Interpreter::_findLocation(std::vector<location> const &l) {
	std::vector<location>::const_iterator it = l.begin();
	int		max = -1;
	while (it != l.end()) {
		const std::vector<std::string>	sp = split_string(it->_path, '/');
		const std::vector<std::string>	&up = split_string(_request->getInterpreterInfo().abs_path, '/');
		int c = count_continues_matches(sp, up);
		if (c > max) {
			_location = *it;
			max = c;
		}
		it++;
	}
	if (max == -1) {
		_buildError(404);
		return ;
	}
}

/**
 * @brief builds error message
 * opens error_page and adds it as body content
 * @param error the error code to be send
 */
void	Interpreter::_buildError(int error) {
	_state = true;
	_response = response(error);
	_buildStandard();
	std::vector<char> vec;
	if (FILE *fp = fopen(_server.getErrorPages().find(error)->second.c_str(), "r"))
	{
		char buf[1024];
		size_t	c_length = 0;
		while (size_t len = fread(buf, 1, sizeof(buf), fp))
		{
			vec.insert(vec.end(), buf, buf + len);
			c_length += len;
		}
		fclose(fp);
		std::stringstream ss;
		ss << c_length;
		_response.add_header("Content-length", ss.str());
		_response.add_header("Content-Type", "text/html");
		_response.add_body(vec, 0);
	} else {
		vec.push_back('5');
		vec.push_back('0');
		vec.push_back('0');
		_response.add_header("Content-Type", "text/html");
		_response.add_header("Content-length", "3");
		_response.add_body(vec, 0);
	}
}

/**
 * @brief cheks the body size and returns 413 payload too big if 
 * body size is too big.
 */
void	Interpreter::_checkBodySize() {
	if (_location._client_max_body_size != 0 && _request->getBody().size() > _location._client_max_body_size) {
		_buildError(413);
	}
}

/**
 * @brief checks if the method is allowed in the current location scope
 * builds 405 Method not allowed if the method is not allowed
 */
void	Interpreter::_checkMethods() {
	if (_location._methods.find(_request->getMethod()) == _location._methods.end()) {
		_buildError(405);
	}
}

/**
 * @brief writes the full request to a file
 * 
 * @param fd the filedescriptor usually a socket
 * @return int 
 */
int	Interpreter::send(const int &fd) {
	return (_response.write_response(fd));
}

/**
 * @brief 
 * 
 */
void	Interpreter::_appendLocationToRoot() {
	if (_location._root != "") {
		_full_path = _location._root;
	} else if (_server.getRoot() != "") {
		_full_path = _server.getRoot();
	} else {
		_full_path = "";
	}
	if (_full_path.back() == '/') {
		_full_path.pop_back();
	}
	std::string	with_out_location;
	if (_location._path.back() == '/')
		with_out_location = _request->getInterpreterInfo().abs_path.substr(_location._path.length() - 1);
	else {
		with_out_location = _request->getInterpreterInfo().abs_path.substr(_location._path.length());
	}
	_full_path += with_out_location;
}

/**
 * @brief answers to all request ending on /
 * and finds the request directory
 */
void	Interpreter::_findDirectory() {
	struct stat s;
	if (stat(_full_path.c_str(), &s) == 0) {
		std::vector<std::string>::const_iterator it = _location._index.begin();
		while (it != _location._index.end())
		{
			std::string	index_path = _full_path + *it;
			if (stat(index_path.c_str(), &s) == 0) {
				if (s.st_mode & S_IFREG) {
					if (_location._cgi_extension != "" && ends_with(index_path, _location._cgi_extension)) {
						_cgi(index_path);
						_build(200);
					} else {
						_openFile(index_path);
						_build(200);
					}
					return ;
				} else {
					_buildError(403);
					return ;
				}
			}
			it++;
		}
		if (_location._autoindex == true) {
			std::string html = buildDirectoryListing(_full_path, _request->getInterpreterInfo().abs_path);
			if (html != "") {
				_buildText(200, html);
			} else {
				_buildError(404);
			}
		} else {
			_buildError(404);
		}
	} else {
		_buildError(404);
	}
}

/**
 * @brief finds the file
 * determines if cgi, upload or just a simple response is needed
 */
void	Interpreter::_findFile() {
	struct stat s;
	if (stat(_full_path.c_str(), &s) == 0) {
		if ( s.st_mode & S_IFREG) {
			if (_location._cgi_extension != "" && ends_with(_request->getInterpreterInfo().abs_path, _location._cgi_extension)) {
				_cgi(_full_path);
				_build(200);
			}
			else if (_location._upload == 1 && (_request->getMethod() == "PUT" || _request->getMethod() == "POST")) {
				_fileUpload(true);
			} else {
				_openFile(_full_path);
				_build(200);
			}
		} else {
			_full_path += '/';
			_findDirectory();
		}
	} else {
		if (_location._cgi_extension != "" && ends_with(_request->getInterpreterInfo().abs_path, _location._cgi_extension)) {
			_cgi(_full_path);
			_build(200);
		}
		else if (_location._upload == 1 && (_request->getMethod() == "PUT" || _request->getMethod() == "POST")) {
			_fileUpload(false);
		}
		else {
			_buildError(404);
		}
	}
}

/**
 * @brief opens _file and adds it to the body
 * if _iscgi == true it will parse the headers
 * TODO check the status code from the cgi and use it for the response status code
 * @param code 
 */
void	Interpreter::_build(int code) {
	if (_file)
	{
		size_t	offset = 0;
		_state = true;
		_response = response(code);
		_buildStandard();
		ssize_t	s = get_file_size(_file);
		std::vector<char> vec(s);
		fread(vec.begin().base(), 1, s, _file);
		if (_iscgi == true)
		{
			std::string	eoh = "\r\n\r\n";
			std::vector<char>::iterator pos = std::search(vec.begin(), vec.end(), eoh.begin(), eoh.end());
			if (pos != vec.end()) {
				pos += 4;
				std::string	header= "";
				std::string	value ="";
				bool state = 0;
				std::vector<char>::iterator it = vec.begin();
				while (it != pos)
				{
					if (*it == '\t' && state == 0)
						break;
					else if (*it == ':' && state == 0) {
						it+=2;
						state = 1;
						continue ;
					} else if (state == 1 && *it == '\r') {
						it += 2;
						state = 0;
						_response.add_header(header, value);
						value.clear();
						header.clear();
						continue ;
					}
					if (state == 0) {
						header += *it;
					} else {
						value += *it;
					}
					it++;
				}
				offset = pos - vec.begin();
			}
		}
		std::stringstream ss;
		ss << (vec.size() - offset);
		_response.add_header("Content-length", ss.str());
		_response.add_body(vec, offset);
		fclose(_file);
	} else
	{
		_buildError(403);
	}
}

/**
 * @brief builds extremly simple response with plain text
 * 
 * @param code status code
 * @param text the body
 */
void	Interpreter::_buildText(int code, std::string const &text) {
	_state = true;
	_response = response(code);
	_buildStandard();
	std::stringstream ss;
	ss << text.length();
	_response.add_header("Content-length", ss.str());
	_response.add_header("Content-Type", "text/html");
	std::vector<char> vec(text.begin(), text.end());
	_response.add_body(vec, 0);
}

void	Interpreter::_buildStandard() {
	_response.add_header("Server", "webvserv");
	if (_request->getConnection() == true)
		_response.add_header("Connection", "keep-alive");
	else
		_response.add_header("Connection", "close");
}

/**
 * @brief returns the response that was built
 * 
 * @return const response& 
 */
const response	&Interpreter::getResponse() const {
	return _response;
}

/**
 * @brief uploads file to given directory
 * TODO:
 * use cgi for upload. check in config file that upload_path is given if upload is enabled
 * @param exists determines if 201 (file created) or 204 (file updated) is returned
 */
void	Interpreter::_fileUpload(bool exists) {
	std::string	path = _full_path.substr(_full_path.find_last_of('/'));
	path = _location._upload_path + path;
	LOG_BLUE("file_path: " << path);
	FILE *fp = fopen(path.c_str(), "w");
	if (fp) {
		fwrite(_request->getBody().begin().base(), 1, _request->getBody().size(), fp);
		fclose(fp);
		if (exists == 1) {
			_buildText(204, "");
		}
		else  {
			_buildText(201, "");
		}
		return ;
	}
	_buildError(500);
}

/**
 * @brief calls the cgi class
 * with "file" as input
 * @param file 
 */
void	Interpreter::_cgi(const std::string &file) {
	cgi c(*_request, _location, file);
	_file = c.getOutput();
	_iscgi = true;
}

/**
 * @brief opens a file with fopen
 * 
 * @param file 
 */
void	Interpreter::_openFile(const std::string &file) {
	_file = fopen(file.c_str(), "r");
}

/**
 * @brief checks if the redirection directive is set for the chosen location
 * builds a 301 redirect if that is the case
 */
void	Interpreter::_checkRedirect() {
	if (_location._redirect_path != "") {
		_state = true;
		_response = response(301);
		_buildStandard();
		_response.add_header("Location", _location._redirect_path);
		_response.add_header("Content-length", "0");
	}
}