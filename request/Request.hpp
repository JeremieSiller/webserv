/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 13:16:53 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/07 16:45:43 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <map>

class Request {
	
	public:

		typedef enum t_requestTypes
		{
			INCOMPLETE,
			COMPLETE,
			INVALID
			
		}requestTypes;


	private:
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_ret;
		int									_port;
		req									_type;

	private:
		/*** Parsing first line in request message ***/
		int		readFirstLine(const std::string& line);
		int		readMethod(const std::string& line, int i);
		int		readPath(const std::string& line, int i);
		int		readVersion(const std::string& line, int i);
		int		parse(const std::string& str);
		
		/*** Available HTTP methods ***/
		static	std::vector<std::string>	methods;

	public:
		Request(const char str[] = NULL, int size = 0) {};
		~Request() {};
		
		Request &operator=(Request const &in)
		{
			this->_method = in._method;
			this->_path= in._path;
			this->_version = in._version;
			this->_headers= in._headers;
			this->_body= in._body;
			this->_ret = in._ret;
			this->_port = in._port;
		}
		
		/*** Getter functions ***/
		std::string							getMethod();
		std::string							getPath();
		std::string							getVersion();
		std::map<std::string, std::string>	getHeaders();
		std::string							getBody();
		int									getRet();
		int									getPort();


		static std::vector<std::string>		initMethods();
		
		requestTypes						getType();
		void								append(const char *str, int size);
};