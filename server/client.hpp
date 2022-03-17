/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:42:26 by nschumac          #+#    #+#             */
/*   Updated: 2022/03/17 17:22:08 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <vector>
# include <string>
# include <arpa/inet.h>
# include <unistd.h>
# include "connection.hpp"
# include "../request/Request.hpp"
# include "../main.hpp"

typedef int t_socket;

#define MAX_RECV_SIZE 4096

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
		clientstatus				_status;
		Connection					*_connection;
		Request 					_req;

	public:

		Client(t_socket client_socket, struct sockaddr_in _addr, Connection *connection);
		~Client();

		t_socket		getSocket() { return this->_client_socket; }

		clientstatus	getClientStatus() { return static_cast<clientstatus>(this->_status); }
		void			setClientStatus(clientstatus cs) { this->_status = (cs); }

		int				readRequest();
		int				sendResponse();

		// Client &operator=(const Client &in)
		// {
		// 	this->_addr = in._addr;
		// 	this->_client_socket = in._client_socket;
		// 	// this->_request = in._request;
		// 	this->_status = in._status;
		// 	return *this;
		// }

};