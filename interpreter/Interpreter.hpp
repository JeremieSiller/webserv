/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Interpreter.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jhagedor <jhagedor@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/11 17:26:50 by jhagedor          #+#    #+#             */
/*   Updated: 2022/03/14 12:35:24 by jhagedor         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <map>
# include <vector>
# include <stdlib.h>


class Interpreter {
	
	private:
		

	public:
		Interpreter();
		~Interpreter() {};

		void	findHostname();
		void	findLocation();

};