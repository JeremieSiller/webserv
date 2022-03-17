/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 13:16:53 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/15 18:50:04 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# pragma once

# include <iostream>
# include <map>
# include <vector>
# include <stdlib.h>

# include "../response/errorcodes.hpp"
# include "../main.hpp"

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
			/*
			** goes through each character checks if its there
			** x is representation of number, so in our case 1.1
			*/
			RequestLineHTTPH,
			RequestLineHTTPHT,
			RequestLineHTTPHTT,
			RequestLineHTTPHTTP,
			RequestLineHTTPHTTP_,
			RequestLineHTTPHTTP_x,
			RequestLineHTTPHTTP_x_,
			RequestLineHTTPHTTP_x_x,
			RequestLineCRLF,
			
			RequestHeaderStart,
			RequestHeaderName,
			RequestHeaderSpace,
			RequestHeaderOptions,

			RequestBodyStart,
			RequestBody
		} parseState;

		typedef struct interpreter_info
		{
			std::string				host;
			std::string				port;
			std::string				abs_path;
			std::string				query;
			std::string				fragment;
		}			t_interpreter_info;
		

	private:
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::vector<char>					_body;
		std::string							_serverName;
		requestTypes						_type;
		parseState							_ps;
		t_interpreter_info					_interpreter_info;

	private:
	
		int		_iscrlf(const char *str, int &idx);
		int		_isalpha(char c);
		
		/*
		** type is boolean true for headerName
		** false for headerValue
		*/
		int		_isSpecial(char c, bool type);

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

		/*** Getter functions ***/
		std::string const &							getMethod() { return this->_method; }
		std::string const &							getPath() { return this->_path; }
		std::string const &							getVersion() { return this->_version; }
		std::map<std::string, std::string> const &	getHeaders() { return this->_headers; }
		std::string const &							getServerName() { return this->_serverName;}
		std::vector<char> const &					getBody() { return this->_body;}

		requestTypes						getType() { return this->_type; }
		void								append(const char str[], int size);
		// new function to interprete requests
		int									findHostname();
		int									findLocation();
		int									prepareInterpreter();
		std::string 						uriDecode(std::string value);
};
