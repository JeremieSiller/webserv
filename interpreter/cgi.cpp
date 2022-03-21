#include "cgi.hpp"

static char	**to_c_array(std::map<std::string, std::string> const &m) {
	char	**ret;
	std::map<std::string, std::string>::const_iterator	it = m.begin();
	ret = new char*[m.size() + 1];
	size_t	i = 0;
	while (it != m.end())
	{
		std::string	var = it->first + '=' + it->second;
		ret[i] = new char[var.length() + 1];
		strcpy(ret[i], var.c_str());
		i++;
		it++;
	}
	ret[i] = NULL;
	return ret;
}

cgi::cgi(const Request &req,location &loc, const std::string &path) : _env(), _req(req), _loc(loc), _path(path), _input(), _output()
{
	_input = tmpfile();
	_output = tmpfile();
	_setEnv();
	_runCgi();
}

cgi::~cgi()
{
}

void	cgi::_setEnv() {
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	if (!_req.getBody().empty())
		_env["CONTENT_LENGTH"] = to_string(_req.getContentLength());
	_env["CONTENT_TYPE"] = _req.getParsedHeader().find("Content-Type")->second;
	LOG_RED("Content-tyepe: " << _env["CONTENT_TYPE"]);
	_env["PATH_INFO"] = _path;
	_env["PATH_TRANSLATED"] = _path;
	_env["QUERY_STRING"] = _req.getInterpreterInfo().query;
	_env["REMOTE_ADDR"] = ""; //should be set to clients IP
	_env["REMOTE_HOST"] = "";
	_env["REMOTE_IDENT"] = "";
	_env["REQUEST_METHOD"] = _req.getMethod();
	_env["SCRIPT_NAME"] = "";
	_env["SERVER_NAME"] = _req.getHost();
	_env["SERVER_PORT"] = "";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["SERVER_SOFTWARE"] = "webserv";
	_env["REDIRECT_STATUS"] = "200";
	_env["SCRIPT_NAME"] = _path;
	_env["REQUEST_URI"] = _path;
	_env["AUTH_TYPE"] = "";
	_env["HTTP_X_SECRET_HEADER_FOR_TEST"] = "";
}

void	cgi::_runCgi() {
	int	fdin, fdout;
	fdin = fileno(_input);
	fdout = fileno(_output);
	write(fdin, _req.getBody().begin().base(), _req.getBody().size());
	write(1, _req.getHeader().begin().base(), _req.getHeader().size());
	lseek(fdin, 0, SEEK_SET);
	int	pid = fork();
	if (pid == -1) {
		LOG_RED("Fork failed");
		throw "internal server error";
	}
	else if (pid == 0) {
		if (dup2(fdin, 0) == -1) {
			exit(99);
		}
		if (dup2(fdout, 1) == -1) {
			exit(99);
		}
		// close(fdin);
		// close(fdout);
		char **arr = (char **)calloc(sizeof(char *), 3);
		arr[0] = &(_loc._cgi_path[0]); 
		arr[1] = &(_path[0]);
		extern char **environ;
		char **env = to_c_array(_env);
		if (execve(_loc._cgi_path.c_str(), &(arr[0]), env) == -1) {
			perror("lol");
			exit(99);
		}
	} else {
		close(fdin);
		int exit_status;
		wait(&exit_status);
		if (WIFEXITED(exit_status)) {
			if (WEXITSTATUS(exit_status) == 99)
				throw "internal server error";
		}
	}
	lseek(fdout, 0, SEEK_SET);
}

FILE	*cgi::getOutput() {
	return _output;
}