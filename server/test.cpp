/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsiller <jsiller@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:18 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/16 11:09:31 by jsiller          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

# define PROTOCOL AF_INET
# define SOCKET_TYPE SOCK_STREAM //SOCK_STREAM - two way connection - tcp



int	main(int argc, char **argv) {
	int	socket_fd = 0;

	socket_fd = socket(PROTOCOL, SOCKET_TYPE, 0);
	if (socket_fd == -1) {
		perror("socket");
		return (1);
	}
	unlink(socket_fd);
}