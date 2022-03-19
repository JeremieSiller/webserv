#include "cgi.hpp"

cgi::cgi(const Request &req,location &loc, const std::string &path) : _req(req), _loc(loc), _path(path)
{
	_input = tmpfile();
	_output = tmpfile();
	_runCgi();
}

cgi::~cgi()
{
}

void	cgi::_runCgi() {
	_env = NULL;

	int	fdin, fdout;
	fdin = fileno(_input);
	fdout = fileno(_output);
	write(fdin, _req.getBody().begin().base(), _req.getBody().size());

	int	pid = fork();
	if (pid == -1) {
		LOG_RED("Fork failed");
		throw "internal server error";
	}
	else if (pid == 0) {
		if (dup2(fdin, 0) == -1) {
			LOG_GREEN("dup2 1");
			exit(99);
		}
		if (dup2(fdout, 1) == -1) {
			LOG_GREEN("dup2 2");
			exit(99);
		}
		close(fdin);
		close(fdout);
		char **arr = (char **)calloc(sizeof(char *), 3);
		arr[0] = &(_loc._cgi_path[0]); 
		arr[1] = &(_path[0]);
		if (execve(_loc._cgi_path.c_str(), &(arr[0]), _env) == -1) {
			perror("lol");
			exit(99);
		}
	} else {
		int exit_status;
		wait(&exit_status);
		if (WIFEXITED(exit_status)) {
			if (WEXITSTATUS(exit_status) == 99)
				throw "internal server error";
		}
		fclose(_input);
	}
	lseek(fdout, 0, SEEK_SET);
}

FILE	*cgi::getOutput() {
	return _output;
}