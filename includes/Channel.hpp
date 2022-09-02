/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/06 16:19:13 by bchelste          #+#    #+#             */
/*   Updated: 2022/05/28 23:38:38 by lbenedar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "User.hpp"

#include <vector>
#include <string>

class User;

class Channel
{
	private:

		std::string	name;
		std::string topic;
		std::vector<const std::string*> operators;
		std::map<const std::string*, User*> users;

		std::string key;

	public:

		Channel(std::string newName);
		Channel(const Channel&);

		Channel	&operator=(const Channel&);

		~Channel();
		
		void setName(std::string newName);
		void setTopic(std::string newTopic);
		void setKey(std::string newKey, std::string nickName);
		
		std::string getName() const;
		std::string getTopic() const;

		void addUser(User &user);
		void addOperator(User &user);
		
		std::map<const std::string*, User*>::iterator	findUser(const std::string& nickName);
		std::vector<const std::string*>::iterator		findOperator(const std::string& nickName);
		bool isUser(const std::string& nickName);
		bool isOper(const std::string& nickName);


		void deleteUser(const std::string &nickName);
		void broadcast(const std::string &message);
		void broadcast_except(const std::string &message, User*);

		std::map<const std::string*, User*> &getUsers();

		bool checkKey(std::string &key);

		static std::vector<Channel *>::iterator findChanInVec(std::vector<Channel *>&, const std::string&);
		
};

#endif