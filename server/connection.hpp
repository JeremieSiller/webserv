/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:00:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/18 17:46:35 by nschumac         ###   ########.fr       */
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

# define CONNECTION_ERROR(msg) throw Connection::Connection_exception(msg)


class Connection
{
	public:

		class Connection_exception : public std::exception 
		{
			public:

				Connection_exception(const char * msg) : _msg(msg) {}
				Connection_exception(std::string msg) : _msg(msg) {}
				virtual ~Connection_exception() _NOEXCEPT {}
				virtual const char *what() const _NOEXCEPT { return _msg.c_str(); }
				
			private:
			
				std::string _msg;

		};

	private:
	
		std::vector<Server>		_servers;
		const unsigned short	_port;
		const std::string		_address;
		t_socket				_socket;

	public:
	
		t_socket	getSocket() { return this->_socket; }

		Client	*newAccept();

		void	addServer(Server const& in);
	
		Connection(const unsigned short &port = STDPORT, const std::string &adress = STDADDRESS);
		~Connection();
};



#endif