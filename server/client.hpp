/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:42:26 by nschumac          #+#    #+#             */
/*   Updated: 2022/03/15 17:02:24 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <vector>
# include <string>
# include <arpa/inet.h>
# include <unistd.h>
# include "connection.hpp"
# include "../request/Request.hpp"

typedef int t_socket;

class Client
{

	public:

		typedef enum t_clientstatus
		{
			READING,
			WRITING,
			DIE
		}clientstatus;

		
	private:

		t_socket					_client_socket;
		struct sockaddr_in			_addr;
		Request						_request;
		int							_status;
		Connection					*_connection;

	public:

		Client(t_socket client_socket, struct sockaddr_in _addr, Connection *connection);
		~Client();

		t_socket		getSocket() { return this->_client_socket; }

		clientstatus	getClientStatus() { return static_cast<clientstatus>(this->_status); }
		void			setClientStatus(clientstatus cs) { this->_status = static_cast<int>(cs); }

		int				readRequest();
		int				sendResponse();
		Request &	getRequest() { return this->_request; }

		Client &operator=(const Client &in)
		{
			this->_addr = in._addr;
			this->_client_socket = in._client_socket;
			this->_request = in._request;
			this->_status = in._status;
			return *this;
		}

};