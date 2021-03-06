/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 17:26:50 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/14 12:35:24 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <map>
# include <vector>
# include <stdlib.h>
# include "../response/response.hpp"
# include "../server/server.hpp"
# include "../request/Request.hpp"
# include "../server/connection.hpp"

class Interpreter {
	
	private:
		Request			*_request;
		Connection		*_connection;
		response		_response;
		Server			_server;
		location		_location;
		bool			_state;
		std::string		_full_path;
		FILE			*_file;
		bool			_iscgi;

		void	_findHostname();
		void	_findLocation(const std::vector<location> &l);
		void	_buildError(int error);
		void	_buildStandard();
		void	_build(int code);
		void	_buildText(int code, std::string const &text);
		void	_checkMethods();
		void	_findFile();
		void	_findDirectory();
		void	_appendLocationToRoot();
		void	_checkBodySize();
		void	_fileUpload(bool existing);
		void	_cgi(const std::string &file);
		void	_openFile(const std::string &file);
		void	_checkRedirect();
		void	_delete();
	public:
		Interpreter();
		void	execute();
		const response	&getResponse() const;
		Interpreter(Request *Request, Connection *connection);
		ssize_t	send(const int &fd);
		~Interpreter();
};
