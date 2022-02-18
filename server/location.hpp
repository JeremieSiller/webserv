#ifndef LOCATION_HPP
# define LOCATION_HPP

#pragma once

# include <string>

class Location
{
private:
	const	std::string	_path;
	const	std::string	_root; //not 100% sure how to handle this
public:
	Location(/* args */);
	~Location() {};
};

#endif
