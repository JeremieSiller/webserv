#ifndef LOCATION_HPP
# define LOCATION_HPP

#pragma once

# include <string>

class location
{
private:
	const	std::string	_path;
	const	std::string	_root; //not 100% sure how to handle this
public:
	location(/* args */);
	~location();
};

#endif
