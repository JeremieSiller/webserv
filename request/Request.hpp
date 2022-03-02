/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 13:16:53 by jhagedor          #+#    #+#             */
/*   Updated: 2022/02/21 15:07:10 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <map>

class Request {
	private:
		std::string							_method;
		std::string							_path;
		std::string							_version;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_ret;
		int									_port;

	private:
		/*** Parsing first line in request message ***/
		int		readFirstLine(const std::string& line);
		int		readMethod(const std::string& line, int i);
		int		readPath(const std::string& line, int i);
		int		readVersion(const std::string& line, int i);
		
		/*** Available HTTP methods ***/
		static	std::vector<std::string>	methods;

	public:
		Request(const std::string& str);
		~Request();
		
		/*** Getter functions ***/
		std::string							getMethod();
		std::string							getPath();
		std::string							getVersion();
		std::map<std::string, std::string>	getHeaders();
		std::string							getBody();
		int									getRet();
		int									getPort();

		int									parse(const std::string& str);

		static std::vector<std::string>		initMethods();
};