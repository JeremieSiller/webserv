/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/21 13:16:53 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/08 15:41:48 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <map>
# include <vector>



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
		/* Parsing first line in request message */
		int		parseFirstLine(const std::string& line);
		int		readMethod(const std::string& line);
		int		readPath(const std::string& line, int i);
		int		readVersion(const std::string& line, int i);
		
		/*** Available HTTP methods ***/
		static	std::vector<std::string>	methods;

	public:
		Request(const std::string& str);
		~Request();
		
		/* Getter functions */
		std::string							getMethod() const;
		std::string							getPath() const;
		std::string							getVersion() const;
		std::map<std::string, std::string>	getHeaders() const;
		std::string							getBody() const;
		int									getRet() const;
		int									getPort() const;

		int									parse(const std::string &str);
		std::string							nextLine(const std::string &str, size_t &i);
		std::string							readKey(const std::string &str);
		std::string							readValue(const std::string &str);

		static std::vector<std::string>		initMethods();
};

