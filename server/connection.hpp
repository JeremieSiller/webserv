/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:00:46 by jsiller           #+#    #+#             */
/*   Updated: 2022/03/16 13:45:12 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONNECTIONS_HPP
# define CONNECTIONS_HPP

#pragma once

# include "server.hpp"
# include <map>
# include <exception>
# include <string>

# define STDPORT 80
# define STDADDRESS "0.0.0.0"

# define CONNECTION_ERROR(msg) throw Connection::Connection_exception(msg)

//Forward declarations
class Client;
typedef int t_socket;

class Connection
{
	public:

		class Connection_exception : public std::runtime_error 
		{
			public:
				Connection_exception(std::string const &msg) : std::runtime_error(msg) { }
		};

	private:
	
		std::vector<Server>		_servers;
		unsigned short			_port;
		std::string				_address;
		t_socket				_socket;

	public:
	
		Connection&operator=(Connection const &in);
	
		Server & getServer(std::string const &serverName);
		t_socket	getSocket() { return this->_socket; }

		Client	*newAccept();

		void	addServer(Server const& in);
	
		Connection(const unsigned short &port = STDPORT, const std::string &adress = STDADDRESS);
		~Connection();
};



#endif