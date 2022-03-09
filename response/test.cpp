#include "response.hpp"
#include <fstream>
#include <vector>
#include <istream>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

int	main(void) {
	response	r(200);
	r.add_header("Connection", "keep-alive");
	r.add_header("Server", "webserv");
	
	long size = GetFileSize("index.html");

	r.add_header("Content-length", std::to_string(size));
	std::vector<char> numbers(size);
	int fd = open("index.html", O_RDONLY);
	read(fd, numbers.begin().base(), size);
	r.add_body(numbers);
	r.write_response(1);
}