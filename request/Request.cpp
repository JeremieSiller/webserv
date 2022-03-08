/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 15:07:31 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/08 15:48:27 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

// class Request {
// 	private:
// 		std::string							_method;
// 		std::string							_path;
// 		std::string							_version;
// 		std::map<std::string, std::string>	_headers;
// 		std::string							_body;
// 		int									_ret;
// 		int									_port;

// 	private:
// 		/*** Parsing first line in request message ***/
// 		int		readFirstLine(const std::string& line);
// 		int		readMethod(const std::string& line, int i);
// 		int		readPath(const std::string& line, int i);
// 		int		readVersion(const std::string& line, int i);
		
// 		/*** Available HTTP methods ***/
// 		static	std::vector<std::string>	methods;

// 	public:
// 		Request(const std::string& str);
// 		~Request();
		


// 		int									parse(const std::string& str);

// 		static std::vector<std::string>		initMethods();
// };

////////////////////////////////////////////////////////////////////////////////

std::vector<std::string>	Request::initMethods()
{
	std::vector<std::string>	methods;

	methods.push_back("GET");
	methods.push_back("POST");
	methods.push_back("DELETE");

	return (methods);
}

std::vector<std::string>	Request::methods = Request::initMethods();

/* Getters */

std::string							Request::getMethod() const
{
	return (_method);
}

std::string							Request::getPath() const
{
	return (_path);
}

std::string							Request::getVersion() const
{
	return (_version);
}

std::map<std::string, std::string>	Request::getHeaders() const
{
	return (_headers);
}

std::string							Request::getBody() const
{
	return (_body);
}

int									Request::getRet() const
{
	return (_ret);
}

int									Request::getPort() const
{
	return (_port);
}

/* Parse Request */

int									Request::parseFirstLine(const std::string& request)
{
	size_t			i;
	std::string	first_line;

	i = request.find_first_of('\n');
	first_line = request.substr(0, i);
	
	// returns position of first whitespace
	i = readMethod(first_line);
	// return position of whitespace after path
	i = readPath(first_line, i);
	// 
	i = readVersion(first_line, i);
	

}

int								Request::readMethod(const std::string& first_line)
{
	size_t			j;

	if ((j = first_line.find_first_of(' ')) == std::string::npos)
	{
		//error
	}
	_method.assign(first_line, 0, j);
	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] == _method)
			return (j);
	}
	// return error
	return (j);
}

int								Request::readPath(const std::string& first_line, int i)
{
	size_t			j;

	//checking for multiple whitespaces
	if ((j = first_line.find_first_not_of(' ', i)) == std::string::npos)
	{
		//error 
	}
	if ((i = first_line.find_first_of(' ', j)) == std::string::npos)
	{
		//error
	}
	_path.assign(first_line, j, i - j);
	// check for wrong path
	return (i);
}

int								Request::readVersion(const std::string& first_line, int i)
{
	if ((i = first_line.find_first_not_of(' ', i)) == std::string::npos)
	{
		//error
	}
	if (first_line[i] == 'H' && first_line[i + 1] == 'T' && first_line[i + 2] == 'T' &&
			first_line[i + 3] == 'P' && first_line[i + 4] == '/')
		this->_version.assign(first_line, i + 5, 3);
	if (this->_version != "1.0" && this->_version != "1.1")
	{
		//error
	}
	
}

std::string			Request::nextLine(const std::string &str, size_t& i)
{
	std::string		ret;
	size_t			j;

	if (i == std::string::npos)
		return "";
	j = str.find_first_of('\n', i);
	ret = str.substr(i, j - i);
	i = (j == std::string::npos ? j : j + 1);
	return ret;	
}

std::string			Request::readKey(const std::string &str)
{
	size_t			j;

	j = str.find_first_of(':', 0);
}

std::string			Request::readValue(const std::string &str)
{
	
}

int					Request::parse(const std::string& str)
{
	std::string	key;
	std::string	value;
	std::string	line;
	size_t		i = 0;

	parseFirstLine(nextLine(str, i));
	while (line != "" && this->_ret != 400)
	{
		// get Key
		key = readKey(line);
		// get Value
		value = readValue(line);
		// check for existing key
		if (this->_headers.count(key))
				this->_headers[key] = value;
		// if (key.find("Secret") != std::string::npos)
		// 	this->_env_for_cgi[formatHeaderForCGI(key)] = value;
	}
	
}