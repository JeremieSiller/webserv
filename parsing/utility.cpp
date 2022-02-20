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