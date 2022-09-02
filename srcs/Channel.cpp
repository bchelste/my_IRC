/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/14 17:09:20 by bchelste          #+#    #+#             */
/*   Updated: 2022/05/28 17:04:30 by lbenedar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string newName)
{
	name = newName;
}

Channel::Channel(const Channel& obj)
{
	*this = obj;
}

Channel	&Channel::operator=(const Channel& obj)
{
	if (this == &obj)
		return (*this);
	name = obj.name;
	topic = obj.topic;
	operators = obj.operators;
	users = obj.users;
	return (*this);
}

Channel::~Channel()
{
	
}

void Channel::setName(std::string newName)
{
	name = newName;
}

void Channel::setTopic(std::string newTopic)
{
	topic = newTopic;
}

void Channel::setKey(std::string newKey,std::string nickName)
{
	if (this->isOper(nickName))
		key = newKey;
}

std::string Channel::getName() const
{
	return (name);
}

std::string Channel::getTopic() const
{
	return (topic);
}

std::map<const std::string*, User*> &Channel::getUsers()
{
	return (users);
}


std::map<const std::string*, User*>::iterator Channel::findUser(const std::string& nickName)
{
	std::map<const std::string*, User*>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
		if (*(it->first) == nickName)
			break;
	return (it);
}

std::vector<const std::string*>::iterator Channel::findOperator(const std::string& nickName)
{
	std::vector<const std::string*>::iterator it;
	for (it = operators.begin(); it != operators.end(); ++it)
		if (**it == nickName)
			break;
	return (it);
}

bool Channel::isUser(const std::string& nickName)
{
	if (findUser(nickName) == users.end())
		return (0);
	return (1);
}

bool Channel::isOper(const std::string& nickName)
{
	if (findOperator(nickName) == operators.end())
		return (0);
	return (1);
}

void Channel::addUser(User &user)
{
	if (findUser(user.getNickName()) != users.end())
		return ;
	users.insert(std::make_pair(&user.getNickName(), &user));
	user.addChannel(this);
}

void Channel::addOperator(User &user)
{
	if (findOperator(user.getNickName()) != operators.end())
		return ;
	operators.push_back(&user.getNickName());
}

void Channel::deleteUser(const std::string &nickName)
{
	std::vector<const std::string*>::iterator oit;
	std::map<const std::string*, User*>::iterator uit;

	if ((uit = findUser(nickName)) != users.end())
	{
		Tracer::Instance()->inFunc("Entered deleteUser");
		Tracer::Instance()->writeInFile("Remove channel from user");
		uit->second->deleteChannel(this);
		Tracer::Instance()->writeInFile("Remove user from channel");
		users.erase(uit);
	}
	else
		return ;
	
	if ((oit = findOperator(nickName)) != operators.end())
	{
		Tracer::Instance()->writeInFile("Remove operator from channel");
		operators.erase(oit);
	}
	Tracer::Instance()->outFunc("Entered deleteUser");
}

void Channel::broadcast(const std::string &message)
{
	broadcast_except(message, NULL);
}

void Channel::broadcast_except(const std::string &message, User *user)
{
	std::map<const std::string*, User*>::iterator	ite = users.end();
	ssize_t											res;

	Tracer::Instance()->inFunc("Entered Channel::broadcast");
	for (std::map<const std::string*, User*>::iterator it = users.begin(); it != ite; ++it)
	{
		if (user && (user->getNickName() == it->second->getNickName()))
			continue;
		Tracer::Instance()->writeInFile("Try send partMessage to " + it->second->getNickName());
		res = send(it->second->getFd(), message.c_str(), message.size(), 0);
		if (res < 0)
		{
			Tracer::Instance()->writeInFile(std::string("!!!ERROR: couldn't send: ") + strerror(errno) + "!!!");
			break;
		}
	}
	Tracer::Instance()->outFunc("Exited Channel::broadcast");
}

bool Channel::checkKey(std::string &key)
{
	if (this->key == key)
		return (true);
	return (false);
}

std::vector<Channel *>::iterator Channel::findChanInVec(std::vector<Channel *> &vec, const std::string &name)
{
	for(std::vector<Channel *>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		if ((*it)->getName() == name)
		{
			return (it);
		}
	}
	return(vec.end());
}