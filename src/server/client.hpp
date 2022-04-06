/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:42:26 by nschumac          #+#    #+#             */
/*   Updated: 2022/04/01 17:05:06 by nschumac         ###   ########.fr       */
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
# include "../interpreter/Interpreter.hpp"
# define INTERNAL_SERVER_ERROR "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\nContent-Type: text/html\r\nServer: webserv\r\nContent-Length: 12\r\n\r\n<h1>500</h1>"

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

		static std::vector<char>::const_iterator	find_pattern(const std::vector<char> &data, const std::vector<char> &pattern);

		
	private:

		t_socket					_client_socket;
		clientstatus				_status;
		Connection					*_connection;
		Request 					_req;
		Interpreter					_interpreter;
		std::vector<char>			_subBuffer;
		size_t						_offset;
		std::vector<char>			_buf;

	public:

		Client(t_socket client_socket, Connection *connection);
		~Client();

		t_socket		getSocket() { return this->_client_socket; }

		clientstatus	getClientStatus() { return static_cast<clientstatus>(this->_status); }
		void			setClientStatus(clientstatus cs) { this->_status = (cs); }

		int				readRequest();
		int				sendResponse();

		int				internalServerError();
		
		Request			&getRequest() { return this->_req; }
};