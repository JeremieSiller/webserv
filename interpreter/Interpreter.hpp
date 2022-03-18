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
		const Request	&_request;
		Connection		*_connection;
		response		_response;
		Server			_server;
		location		_location;

		void	_findHostname();
		void	_findLocation(const std::vector<location> &l);
	public:
		Interpreter(const Request &Request, Connection *connection);
		~Interpreter();

};