/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nschumac <nschumac@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/14 20:18:18 by jsiller           #+#    #+#             */
/*   Updated: 2022/02/17 21:57:40 by nschumac         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"

# define PROTOCOL AF_INET
# define SOCKET_TYPE SOCK_STREAM //SOCK_STREAM - two way connection - tcp



int	main(int argc, char **argv) {

	connectionshit con1;

	con1.addr = "127.0.0.1";
	con1.port = 8080;

	std::vector<servershit> one;

	servershit serv1;

	serv1.servername.push_back("localhost");
	serv1.errorpages.push_back("yeet");
	serv1.root = "/";

	servershit serv2;

	serv2.servername.push_back("127.0.0.1");
	serv2.errorpages.push_back("yeet");
	serv2.root = "/";


	one.push_back(serv1);
	one.push_back(serv2);


	connectionshit con2;

	con2.addr = "169.254.15.104";
	con2.port = 8080;

	servershit serv3;

	serv3.servername.push_back("169.254.15.104");
	serv3.errorpages.push_back("yeet");
	serv3.root = "/";	
	
	configshit cs;

	std::vector<servershit> two;

	two.push_back(serv3);

	cs.connections.push_back(std::pair<connectionshit, std::vector<servershit> >(con1, one));
	cs.connections.push_back(std::pair<connectionshit, std::vector<servershit> >(con2, two));
	
	try
	{
		webserv webserver(cs);
		webserver.run();
	}
	catch (std::exception & e)
	{
		std::cout << e.what() << std::endl;
	}

}