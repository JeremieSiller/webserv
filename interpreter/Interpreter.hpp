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
# include "response/response.hpp"
# include "server/server.hpp"
# include "request/request.hpp"
# include "location/location.hpp"

class Interpreter {
	
	private:
		const Request	&_request;
		response		_response;
		Server			_server;

	public:
		Interpreter(const Request &Request);
		~Interpreter() {};

		void	findHostname(const std::vector<Server> &servers);
		void	findLocation();

};