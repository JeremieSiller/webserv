/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 15:07:31 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/07 16:45:26 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request(const char str[], int size) : _method(), _path(), _version(), _headers(), _body(), _ret(), _port()
{
	if (!parse)

}

int	Request::parse()
{

}