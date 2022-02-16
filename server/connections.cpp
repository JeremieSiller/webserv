/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   connections.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsiller <jsiller@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 12:05:03 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/16 12:08:20 by jsiller          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "connections.hpp"

connections::connections(const unsigned &port, const std::string &address) : _servers(), _address(address), _port(port) { }

connections::~connections() {
	
}