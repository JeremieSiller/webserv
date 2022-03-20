#include "Interpreter.hpp"


#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include "cgi.hpp"
#include <fcntl.h>

std::string	buildDirectoryListing(std::string const &dir, std::string const &abs_path) {
	std::string ret = "";
	ret += "<html>\n";
	ret += "<head><title>Index of "+ abs_path + "</title></head>\n";
	ret += "<body>\n";
	ret += "<h1>Index of " + abs_path + "</h1><hr><pre><a href=\"../\">../</a>\n";
	DIR *d;
	struct dirent *dd;
	d = opendir(dir.c_str());
	if (d) {
		while ((dd = readdir(d)) != NULL)
		{
			if (dd->d_name[0] != '.') {
				ret += "<a href=\"" + abs_path + dd->d_name;
				if (dd->d_type == DT_DIR)
					ret += "/";
				ret += "\">" + std::string(dd->d_name);
				if (dd->d_type == DT_DIR)
					ret += "/";
				ret += "</a>\n";
			}
		}
	}
	else {
		return "";
	}
	ret += "</body>\n";
	ret += "</html>";
	return ret;
}

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
	if (pit == path.begin() && path.empty() == false)
		return -1;
	return count;
}

inline bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

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
				// write(1, _request.getBody().begin().base(), _request.getBody().size());
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
	if (_location._cgi_extension == ".bla") { //TODO
		LOG_YELLOW("CGI");
		cgi c(_request, _location, _file);
		fp = c.getOutput();
	} else {
		fp = fopen(_file.c_str(), "r");
	}
	if (fp)
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
		if (ends_with(_full_path, ".html") || _location._cgi_extension != "") {
			_response.add_header("Content-Type", "text/html");
		} else {
			_response.add_header("Content-Type", "media-type");
		}
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
