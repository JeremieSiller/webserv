/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:00:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/17 22:10:07 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONS_HPP
# define CONNECTIONS_HPP

#pragma once

# include "client.hpp"
# include "server.hpp"
# include "location.hpp"
# include <map>
# include <exception>
# include <string>

# define STDPORT 80
# define STDADDRESS "0.0.0.0"

# define CONNECTION_ERROR(msg) throw connection::connection_exception(msg)


class connection
{
	public:

		class connection_exception : public std::exception 
		{
			public:

				connection_exception(const char * msg) : _msg(msg) {}
				connection_exception(std::string msg) : _msg(msg) {}
				virtual ~connection_exception() _NOEXCEPT {}
				virtual const char *what() const _NOEXCEPT { return _msg.c_str(); }
				
			private:
			
				std::string _msg;

		};

	private:
	
		std::vector<server>		_servers;
		std::vector<location>	_locations;
		const unsigned short	_port;
		const std::string		_address;
		t_socket				_socket;

	public:
	
		t_socket	getSocket() { return this->_socket; }

		client	*newAccept();

		void	addServer(server const& in);
	
		connection(const unsigned short &port = STDPORT, const std::string &adress = STDADDRESS);
		~connection();
};



#endif