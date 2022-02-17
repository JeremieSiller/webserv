/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 18:42:26 by nschumac          #+#    #+#             */
/*   Updated: 2022/02/17 22:10:23 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <vector>
# include <string>
# include <arpa/inet.h>
# include <unistd.h>

typedef int t_socket;

class client
{
	private:

		t_socket					_client_socket;
		struct sockaddr_in			_addr;
		std::vector<std::string>	_requests;
		int							_status;

	public:

		client(t_socket client_socket, struct sockaddr_in _addr);
		~client();

		t_socket	getSocket() { return this->_client_socket; }

		client &operator=(const client &in)
		{
			this->_addr = in._addr;
			this->_client_socket = in._client_socket;
			this->_requests = in._requests;
			this->_status = in._status;
			return *this;
		}

};