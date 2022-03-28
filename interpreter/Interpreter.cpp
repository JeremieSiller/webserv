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

Interpreter::Interpreter(const Request &request, Connection *connection) : _request(request), _connection(connection) {
	_state = 0;
	if (_request.getStatus() == Request::INVALID) {
		_buildError(400);
		return ;
	}
	_findHostname();
	std::vector<std::string>::const_iterator it = _server.getServerName().begin();
	while (it != _server.getServerName().end()) {
		LOG_BLUE("server-name: " << *it);
		it++;
	}
	_findLocation(_server.getLocations());
	if (_state == 1)
		return ;
	_checkMethods();
	if (_state == 1)
		return ;
	_appendLocationToRoot();
	if (_request.getInterpreterInfo().abs_path.back() == '/') {
		_findDirectory();
	} else {
		_findFile();
	}
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
		const std::vector<std::string>	sp = split_string(it->_path, '/');
		const std::vector<std::string>	&up = split_string(_request.getInterpreterInfo().abs_path, '/');
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
	LOG_GREEN(_location._path << " is the most exact match for: " << _request.getInterpreterInfo().abs_path);
}

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
		_response.add_body(vec);
	} else {
		vec.push_back('5');
		vec.push_back('0');
		vec.push_back('0');
		_response.add_body(vec);
		_response.add_header("Content-Type", "text/html");
		_response.add_header("Content-length", "3");
	}
}

void	Interpreter::_checkMethods() {
	if (_location._methods.find(_request.getMethod()) == _location._methods.end()) {
		_buildError(405);
	}
}

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
		with_out_location = _request.getInterpreterInfo().abs_path.substr(_location._path.length() - 1);
	else {
		with_out_location = _request.getInterpreterInfo().abs_path.substr(_location._path.length());
	}
	_full_path += with_out_location;
}

void	Interpreter::_findDirectory() {
	struct stat s;
	if (stat(_full_path.c_str(), &s) == 0) {
		std::vector<std::string>::const_iterator it = _location._index.begin();
		while (it != _location._index.end())
		{
			std::string	index_path = _full_path + *it;
			if (stat(index_path.c_str(), &s) == 0) {
				if (s.st_mode & S_IFREG) {
					_build(200, index_path);
					return ;
				} else {
					_buildError(403);
					return ;
				}
			}
			it++;
		}
		if (_location._autoindex == true) {
			std::string html = buildDirectoryListing(_full_path, _request.getInterpreterInfo().abs_path);
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

void	Interpreter::_findFile() {
	struct stat s;
	if (stat(_full_path.c_str(), &s) == 0) {
		if ( s.st_mode & S_IFREG) {
			if (_request.getMethod() == "PUT") {
				LOG_YELLOW("file already exists");
				FILE  *fp = fopen(_full_path.c_str(), "w");
				if (fp) {
					fwrite(_request.getBody().begin().base(), 1, _request.getBody().size(), fp);
					fclose(fp);
					_buildText(204, "");
					return ;
				}
			}
			_build(200, _full_path);
		} else {
			_build(301, "standard-html/301.html");
			_response.add_header("Location", _request.getInterpreterInfo().abs_path + "/");
		}
	} else {
		if (_request.getMethod() == "PUT") {
			FILE *fp = fopen(_full_path.c_str(), "w");
			if (fp) {
				fwrite(_request.getBody().begin().base(), 1, _request.getBody().size(), fp);
				_buildText(201, "");
				fclose(fp);
				return ;
			}
		}

		_buildError(404);
	}
}

void	Interpreter::_build(int code, std::string const &_file) {
	_state = true;
	_response = response(code);
	_buildStandard();
	FILE *fp;
	std::vector<char> vec;
	if (ends_with(_request.getInterpreterInfo().abs_path, _location._cgi_extension) && 
		_location._cgi_method.find(_request.getMethod()) != _location._cgi_method.end()) {
		LOG_YELLOW("CGI");
		LOG_GREEN("body-size: " << _request.getBody().size());
		cgi c(_request, _location, _file);
		fp = c.getOutput();
	} else {
		fp = fopen(_file.c_str(), "r");
	}
	if (fp)
	{
		std::vector<char> eoh;
		eoh.push_back('\r');
		eoh.push_back('\n');
		eoh.push_back('\r');
		eoh.push_back('\n');
		char buf[1024];
		while (size_t len = fread(buf, 1, sizeof(buf), fp))
		{
			vec.insert(vec.end(), buf, buf + len);
		}
		fclose(fp);
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
					LOG_GREEN(header << " : " << value << "|");
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
			vec.erase(vec.begin(), pos);
		}
		LOGN("Printing first 100 bytes of body");
		for	(size_t i = 0; i < 100 && i < vec.size(); i++) {
			if (vec[i] == 10 || vec[i] == 13) {
				std::cout << ".";
			} else {
				std::cout << vec[i];
			}
			std::cout << " ";
		}
		std::cout << std::endl;
		LOG_RED("Printing first 100 bytes of body as int");
		for	(size_t i = 0; i < 100 && i < vec.size(); i++) {
			std::cerr << (int)vec[i] << ' ';
		}
		std::cout << std::endl;
		LOG_RED("------");
		LOG_BLUE("vec-size: " << vec.size()); 
		std::cout << std::endl;
		std::stringstream ss;
		ss << vec.size();
		if (_request.getMethod() == "POST") {
			_response.add_header("Content-length", "0");
		} else {
			_response.add_header("Content-length", ss.str());
		}
		// if (ends_with(_full_path, ".html") || _location._cgi_extension != "") {
		// 	_response.add_header("Content-Type", "text/html");
		// } else {
		// 	_response.add_header("Content-Type", "media-type");
		// }
		int fd = open("test.txt", O_WRONLY | O_TRUNC | O_CREAT);
		if (fd == -1)
			perror("open");
		if (write(fd, vec.begin().base(), vec.size()) == -1)
			perror("write");
		close(fd);
		if (_request.getMethod() == "GET")
			_response.add_body(vec);
	} else
	{
		_buildError(403);
	}
}

void	Interpreter::_buildText(int code, std::string const &text) {
	_state = true;
	_response = response(code);
	_buildStandard();
	std::stringstream ss;
	ss << text.length();
	_response.add_header("Content-length", ss.str());
	_response.add_header("Content-Type", "text/html");
	std::vector<char> vec(text.begin(), text.end());
	_response.add_body(vec);
}

void	Interpreter::_buildStandard() {
	_response.add_header("Server", "webvserv");
	if (_request.getConnection() == true)
		_response.add_header("Connection", "keep-alive");
	else
		_response.add_header("Connection", "close");
}
