#ifndef UTILITY_HPP
# define UTILITY_HPP

# pragma once
# include <arpa/inet.h>
# include <sys/stat.h>
# include <string>

bool	is_ip(const std::string &ipAddress);
bool    is_directory(const std::string &path);
bool    is_file(const std::string &path);
#endif
