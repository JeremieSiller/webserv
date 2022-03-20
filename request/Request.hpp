/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 13:16:53 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/20 21:13:36 by nschumac         ###   ########.fr       */
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

# define CRLF "\r\n"

struct s_multipart
{
	std::map<std::string, std::string> header;
	std::map<std::string, std::vector<char> > content_Disposition;
	std::vector<char> data;
};

class Request
{
	public:

		typedef enum t_headerstaus
		{
			HEADER,
			BODY,
			COMPLETE,
			INVALID
		} headerstatus;

		typedef struct interpreter_info
		{
			std::string				abs_path;
			std::string				query;
			std::string				fragment;
		}			t_interpreter_info;


	private:
	
		std::string _header;
		std::vector<char> _body;

		std::map<std::string, std::string> _parsedHeader;
		
		std::string			_version;
		std::string			_path;
		std::string			_method;
		t_interpreter_info	_interpreter_info;
		
		headerstatus	_headerStatus;
		
		int			_chunksize; // auch bodysize


		std::string _host;
		size_t		_contentLength;
		std::list<std::string> _transferEncoding;
		
		// false -> CLOSE;
		// true -> keep-alive;
		bool		_connection;
		

		// only 100-Continue laut https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Expect
		// 417 on fail and 100 continue on success
		bool		_expect;

		// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Type
		std::string _contenttype;

		
		std::string _location;
		
		std::string _server;

		std::list<std::string> _accept;

		std::vector<char> _boundary;

		std::vector<s_multipart> boundary_body;
		
		std::map<std::string, std::vector<char> > _url_encode;

	private:

		
		std::vector<char> _parseChunked(std::vector<char>::const_iterator start, std::vector<char>::const_iterator end);
		
		void	_fillboundary();
		void	_fillUrlEncode();
		int											prepareInterpreter();

	public:
	
		Request();

		int 	parseHeader();
		
		void	setHeader(std::string const &header);
		void	setStatus(headerstatus status) { this->_headerStatus = status; }
		void	addBody(std::vector<char>::const_iterator start, std::vector<char>::const_iterator end);
		
		void	clear();
		
		const headerstatus							&getStatus() const { return this->_headerStatus; }
		const std::list<std::string>				&getAccept() const { return _accept; }
		const std::string							&getServer() const { return _server; }
		const std::string							&getContentType() const {return _contenttype; }
		const std::string							&getLocation() const { return _location; }
		const std::vector<char>						&getBody() const { return _body; }
		const std::map<std::string, std::string>	&getParsedHeader() const { return _parsedHeader; }
		const std::string							&getHeader() const { return _header; }
		const std::string							&getVersion() const { return _version; }
		const std::string							&getPath() const { return _path; }
		const std::string							&getMethod() const { return _method; }
		const int									&getChunkSize() const { return _chunksize; }
		const size_t								&getContentLength() const { return _contentLength; }
		const std::string 							&getHost() const { return _host; }
		const std::list<std::string>				&getTransferEncoding() const { return _transferEncoding; }
		const bool									&getConnection() const { return _connection; }
		const bool									&getExpect() const { return _expect; }
		const std::map<std::string, std::vector<char> > &getUrlEncode() const { return this->_url_encode; }

		std::string 								uriDecode(std::string value);
		const t_interpreter_info					&getInterpreterInfo() const { return _interpreter_info; }
};
