#include "utility.hpp"
/**
 * @brief returns if string is a valid ip_address
 * @param ipAddress 
 */
bool    is_ip(const std::string &ipAddress)
{
	struct sockaddr_in sa;
	int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
	return result != 0;
}

/**
 * @brief returns if @param path is a valid directory
 * @param path 
 */
bool    is_directory(const std::string &path)
{
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0)
		return (0);
	return !S_ISREG(buf.st_mode);
}

/**
 * @brief returns if @param path is a valid file
 * @param path 
 */
bool    is_file(const std::string &path)
{
	struct stat buf;
	if (stat(path.c_str(), &buf) != 0)
		return (0);
	return S_ISREG(buf.st_mode);
}

/**
 * @brief reads file /etc/hosts and searches for @param host 
 * @return std::string corresponding ip address
 */
std::string getAddressFromHost(std::string const &host) {
	std::ifstream						file("/etc/hosts");
	std::string							r = "";
	std::stringstream 					stream;
	std::stringstream					tmp;
	std::string							start_line;
	std::istream_iterator<std::string>	start;
	std::string							line;
	if (!file)
		return r;
	stream << file.rdbuf();
	while (std::getline(stream, line)) {
		std::istream_iterator<std::string>	end;
		tmp << line;
		start = tmp;
		start_line = *start;
		while (start != end) {
			if (*start == host) {
				if (start != end) {
					if (r != "" && is_ip(start_line)) {
						throw std::runtime_error("duplicate symbols for hostname: " + host + " in /etc/hosts");
					}
					if (is_ip(start_line))
						r = start_line;
				}
				tmp.clear();
				break ;
			}
			start++;
		}
		tmp.clear();
	}
	return r;
}