/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputMessage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 16:17:43 by bchelste          #+#    #+#             */
/*   Updated: 2022/05/28 22:49:29 by lbenedar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INPUTMESSAGE_HPP
# define INPUTMESSAGE_HPP

# include <string>
# include <vector>
# include "Tracer.hpp"

class InputMessage
{
	private:
		
		std::string 				prfx;
		std::string					cmnd;
		std::vector<std::string>	prmtrs;
		
	public:
		
		InputMessage(const std::string &src);
		InputMessage(const InputMessage&);
		~InputMessage();

		InputMessage	&operator=(const InputMessage&);
		
		const std::string				&getPrfx() const;
		const std::string				&getCmnd() const;
		const std::vector<std::string>	&getPrmtrs() const;

		int initInputMessage(const std::string &src);
};

#endif