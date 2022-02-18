/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:18 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/18 17:58:53 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "../parsing/Lexer.hpp"
#include "../parsing/ConfigToken.hpp"
#include "../parsing/ConfigParser.hpp"

# define PROTOCOL AF_INET
# define SOCKET_TYPE SOCK_STREAM //SOCK_STREAM - two way connection - tcp



int	main(int argc, char **argv) {
	
		if (argc != 2) { //define PATH all the time just for testing purpose.
		std::cerr << "usage: " << argv[0] << " [PATHNAME]" << std::endl;
		return (1);
	}
	try
	{
		Lexer<ConfigToken> l(argv[1]);
		ConfigParser  p(l.getToken());
		webserv webserver(p);

		webserver.run();
	}
	catch (std::exception const &e)
	{
		std::cerr << "Error" << std::endl;
		std::cerr << e.what() << std::endl;
	}

}