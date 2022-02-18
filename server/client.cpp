/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/17 19:04:16 by nschumac          #+#    #+#             */
/*   Updated: 2022/02/17 22:10:32 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <unistd.h>


client::client(t_socket client_socket, struct sockaddr_in addr) : _client_socket(client_socket), _addr(addr), _requests(), _status()
{

}

client::~client()
{
	close(this->_client_socket);
}