/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 13:16:53 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/17 18:10:55 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# pragma once

# include <iostream>
# include <map>
# include <vector>
# include <stdlib.h>
# include <list>

# include "../response/errorcodes.hpp"
# include "../main.hpp"

// class Request {
	
// 	public:

// 		typedef enum t_requestTypes
// 		{
// 			INCOMPLETE,
// 			COMPLETE,
// 			INVALID
			
// 		}requestTypes;
// 	private:

// 		//https://www.tutorialspoint.com/http/http_requests.htm
// 		typedef enum t_parseState
// 		{
// 			RequestLineMethod,

// 			/*
// 			** Absolute URI, Relative URI, * maybe
// 			** Absolute path cannot be emypty if none is given it is INVALID
// 			*/
// 			RequestLineURI,
// 			/*
// 			** goes through each character checks if its there
// 			** x is representation of number, so in our case 1.1
// 			*/
// 			RequestLineHTTPH,
// 			RequestLineHTTPHT,
// 			RequestLineHTTPHTT,
// 			RequestLineHTTPHTTP,
// 			RequestLineHTTPHTTP_,
// 			RequestLineHTTPHTTP_x,
// 			RequestLineHTTPHTTP_x_,
// 			RequestLineHTTPHTTP_x_x,
// 			RequestLineCRLF,
			
// 			RequestHeaderStart,
// 			RequestHeaderName,
// 			RequestHeaderSpace,
// 			RequestHeaderOptions,

// 			RequestBodyStart,
// 			RequestBody
// 		} parseState;

// 		typedef struct interpreter_info
// 		{
// 			std::string				host;
// 			std::string				port;
// 			std::string				abs_path;
// 			std::string				query;
// 			std::string				fragment;
// 		}			t_interpreter_info;
		

// 	private:
// 		std::string							_method;
// 		std::string							_path;
// 		std::string							_version;
// 		std::map<std::string, std::string>	_headers;
// 		std::vector<char>					_body;
// 		std::string							_serverName;
// 		requestTypes						_type;
// 		parseState							_ps;
// 		t_interpreter_info					_interpreter_info;

// 	private:
	
// 		int		_iscrlf(const char *str, int &idx);
// 		int		_isalpha(char c);
		
// 		/*
// 		** type is boolean true for headerName
// 		** false for headerValue
// 		*/
// 		int		_isSpecial(char c, bool type);

// 		/*
// 		** parses everything
// 		*/
// 		int		_parse(const char str[], int size);
		
// 		/*
// 		** Integrity_check
// 		*/
// 		int		_integrityCheck();
		
// 		/*** Available HTTP methods ***/
// 		static	std::vector<std::string>	methods;

// 	public:
// 		Request(const char str[] = NULL, int size = 0);
// 		~Request() {};
		
// 		Request &operator=(Request const &in)
// 		{
// 			this->_method = in._method;
// 			this->_path= in._path;
// 			this->_version = in._version;
// 			this->_headers= in._headers;
// 			this->_body= in._body;
// 			this->_serverName = in._serverName;
// 			return *this;
// 		}

// 		/*** Getter functions ***/
// 		std::string const &							getMethod() { return this->_method; }
// 		std::string const &							getPath() { return this->_path; }
// 		std::string const &							getVersion() { return this->_version; }
// 		std::map<std::string, std::string> const &	getHeaders() { return this->_headers; }
// 		std::string const &							getServerName() { return this->_serverName;}
// 		std::vector<char> const &					getBody() { return this->_body;}

// 		requestTypes						getType() { return this->_type; }
// 		void								append(const char str[], int size);
// 		// new function to interprete requests
// 		int									findHostname();
// 		int									findLocation();
// 		int									prepareInterpreter();
// 		std::string 						uriDecode(std::string value);
// };

# define CRLF "\r\n"

class Request
{
	public:

		typedef enum t_headerstaus
		{
			HEADER,
			BODY,
			INVALID
		} headerstatus;


	private:

	
		std::string _header;
		std::vector<char> _body;

		std::map<std::string, std::string> _parsedHeader;
		
		std::string _version;
		std::string _path;
		std::string _method;
		
		headerstatus	_headerStatus;
		
		int			_chunksize; // auch bodysize


		std::string _host;
		size_t		_contentLength;
		std::vector<std::string> _transferEncoding;
		
		// false -> CLOSE;
		// true -> keep-alive;
		bool		_connection;
		

		// only 100-Continue laut https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Expect
		// 417 on fail and 100 continue on success
		bool		_expect;

		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type
		std::map<std::string, std::string> _contenttype;

		
		std::string _location;
		
		std::string _server;

		std::list<std::string> _accept;


	private:

		int _parseHeader();

	public:
	
		Request();

		void	setHeader(std::string const &header);
		void	addBody(std::vector<char>::const_iterator start, std::vector<char>::const_iterator end);
		
		void	clear();
		
		const headerstatus							&getStatus() const { return this->_headerStatus; }
		const std::list<std::string>				&getAccept() const { return _accept; }
		const std::string							&getServer() const { return _server; }
		const std::map<std::string, std::string>	&getContentType() const {return _contenttype; }
		const  std::string							&getLocation() const { return _location; }
		const std::vector<char>						&getBody() const { return _body; }
		const std::map<std::string, std::string>	&getParsedHeader() const { return _parsedHeader; }
		const std::string							&getHeader() const { return _header; }
		const std::string							&getVersion() const { return _version; }
		const std::string							&getPath() const { return _path; }
		const std::string							&getMethod() const { return _method; }
		const int									&getChunkSize() const { return _chunksize; }
		const size_t								&getContentLength() const { return _contentLength; }
		const std::string 							&getHost() const { return _host; }
		const std::vector<std::string>				&getTransferEncoding() const { return _transferEncoding; }
		const bool									&getConnection() const { return _connection; }
		const bool									&getExpect() const { return _expect; }

};
