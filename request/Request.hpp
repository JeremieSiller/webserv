/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/08 16:30:33 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/08 16:31:13 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# pragma once

# include <iostream>
# include <map>
# include <vector>

class Request {
	
	public:

		typedef enum t_requestTypes
		{
			INCOMPLETE,
			COMPLETE,
			INVALID
			
		}requestTypes;
		

	private:

		//https://www.tutorialspoint.com/http/http_requests.htm
		typedef enum t_parseState
		{
			RequestLineMethod,

			/*
			** Absolute URI, Relative URI, * maybe
			** Absolute path cannot be emypty if none is given it is INVALID
			*/
			RequestLineURI,
			RequestLineHttpVersion,
			
			RequestHeaderStart,
			RequestHeaderName,
			RequestHeaderSpace,
			RequestHeaderOptions,

			RequestBodyStart,
			RequestBody
		} parseState;

	private:
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::vector<char>					_body;
		std::string							_serverName;
		requestTypes						_type;
		parseState							_ps;

	private:
		/* Parsing first line in request message */
		int		parseFirstLine(const std::string& line);
		int		readMethod(const std::string& line);
		int		readPath(const std::string& line, int i);
		int		readVersion(const std::string& line, int i);
		/*
		** parses everything
		*/
		int		_parse(const char str[], int size);
		
		/*
		** Integrity_check
		*/
		int		_integrityCheck();
		
		/*** Available HTTP methods ***/
		static	std::vector<std::string>	methods;

	public:
		Request(const char str[] = NULL, int size = 0);
		~Request() {};
		
		Request &operator=(Request const &in)
		{
			this->_method = in._method;
			this->_path= in._path;
			this->_version = in._version;
			this->_headers= in._headers;
			this->_body= in._body;
			this->_serverName = in._serverName;
			return *this;
		}
		
		/* Getter functions */
		std::string							getMethod() const;
		std::string							getPath() const;
		std::string							getVersion() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getBody() const;
		int									getRet() const;
		int									getPort() const;

		int									parse(const std::string &str);
		std::string							nextLine(const std::string &str, size_t &i);
		std::string							readKey(const std::string &str);
		std::string							readValue(const std::string &str);

		static std::vector<std::string>		initMethods();

		/*** Getter functions ***/
		std::string const &							getMethod() { return this->_method; }
		std::string const &							getPath() { return this->_path; }
		std::string const &							getVersion() { return this->_version; }
		std::map<std::string, std::string> const &	getHeaders() { return this->_headers; }
		std::string const &							getServerName() { return this->_serverName;}

		requestTypes						getType() { return this->_type; }
		void								append(const char str[], int size);
};
