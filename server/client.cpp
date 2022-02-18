/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 19:04:16 by nschumac          #+#    #+#             */
/*   Updated: 2022/02/18 18:44:02 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <unistd.h>


Client::Client(t_socket client_socket, struct sockaddr_in addr, Connection *connection) : _client_socket(client_socket), _addr(addr), _requests(), _status(), _connection(connection)
{

}

Client::~Client()
{
	close(this->_client_socket);
}