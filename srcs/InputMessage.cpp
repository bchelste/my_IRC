/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InputMessage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 17:27:23 by bchelste          #+#    #+#             */
/*   Updated: 2022/05/31 20:09:21 by lbenedar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "InputMessage.hpp"

InputMessage::InputMessage(const std::string &src)
{
	this->prfx = "";
	this->cmnd = "";
	this->initInputMessage(src);
}

InputMessage::InputMessage(const InputMessage &obj)
{
	*this = obj;
}

InputMessage	&InputMessage::operator=(const InputMessage &obj)
{
	if (this == &obj)
		return (*this);
	prfx = obj.prfx;
	cmnd = obj.cmnd;
	prmtrs = obj.prmtrs;
	return (*this);
}


InputMessage::~InputMessage()
{
	
}

const std::string &InputMessage::getPrfx() const
{
	return (this->prfx);
}

const std::string &InputMessage::getCmnd() const
{
	return (this->cmnd);
}

const std::vector<std::string> &InputMessage::getPrmtrs() const
{
	return (this->prmtrs);
}

int InputMessage::initInputMessage(const std::string &src)
{
	size_t prev = 0;
	size_t next;
	std::string separator = " ";
	size_t delta = separator.length();
	
	// check \r\n at the end of string
	// if ((next = src.find("\r\n", prev)) == std::string::npos)
	// 	return (1);
	
	// set prefix:

	Tracer::Instance()->inFunc("Entered initInputMessage");

	if (src[0] == ':')
	{
		if ((next = src.find(separator, prev)) != std::string::npos)
		{
			Tracer::Instance()->writeInFile("prefix: " + src.substr(prev, (next - prev)));
			this->prfx = src.substr(prev, (next - prev));
			prev = next + delta;
		}
	}
	
	// set command:
	if ((next = src.find(separator, prev)) != std::string::npos)
	{
		Tracer::Instance()->writeInFile("command: " + src.substr(prev, (next - prev)));
		this->cmnd = src.substr(prev, (next - prev));
		prev = next + delta;
	
		// set params:
		Tracer::Instance()->writeInFile("params: ");
		/*
		while((next = src.find(separator, prev)) != std::string::npos)
		{
			std::cout << src.substr(prev, (next - prev)) << std::endl;
			this->prmtrs.push_back(src.substr(prev, (next - prev)));
			prev = next + delta;
		}
		std::cout << src.substr(prev) << std::endl;
		this->prmtrs.push_back(src.substr(prev));
		*/
		
		
		
		while((next = src.find(separator, prev)) != std::string::npos)
		{
			//std::cout << src.substr(prev, (next - prev)) << std::endl;
			// check :
			if (src[prev] == ':')
			{
				Tracer::Instance()->outFunc(src.substr(prev + 1));
				this->prmtrs.push_back(src.substr(prev + 1));
				return (0);
			}
			Tracer::Instance()->writeInFile(src.substr(prev, (next - prev)));
			this->prmtrs.push_back(src.substr(prev, (next - prev)));
			prev = next + delta;
		}
		if (src[prev] == ':' && (prev + 1) != src.length())
			prmtrs.push_back(src.substr(prev + 1));
		else
			prmtrs.push_back(src.substr(prev));
		Tracer::Instance()->writeInFile(*(prmtrs.end() - 1));
	}
	else
		this->cmnd = src.substr(prev);
	Tracer::Instance()->outFunc("Exited initInputMessage");
	return (0);
}
