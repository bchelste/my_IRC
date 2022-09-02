#include <sys/socket.h>
#include "Commands.hpp"

std::vector<std::string> ft_separator(std::string src, std::string separator)
{
	std::vector<std::string> res;

	size_t prev = 0;
	size_t next;
	size_t delta = separator.size();
	
	while ((next = src.find(separator, prev)) != std::string::npos)
	{
		res.push_back(src.substr(prev, (next - prev)));
		prev = next + delta;
	}
	res.push_back(src.substr(prev));
	return (res);
}

Commands::Commands()
{
	
}

Commands::~Commands()
{
	
}

int Commands::automaticReplies(User &user, std::string reply)
{
	Tracer::Instance()->inFunc("Entered automaticReplies");
	Tracer::Instance()->writeInFile("Reply(" + reply + ") to user (" + user.getNickName() + ")");
	Tracer::Instance()->outFunc("Exited automaticReplies");
	return (send(user.getFd(), reply.c_str(), reply.size(), 0));
}

int Commands::PASS(InputMessage &inputMessage, User &user, Server &serv)
{
	if (inputMessage.getPrmtrs().size() == 0)
		return (automaticReplies(user, Replies::ERR_NEEDMOREPARAMS(inputMessage)));
	else if (user.getStatus() == 1)
		return (automaticReplies(user, Replies::ERR_ALREADYREGISTRED()));
	else
		return (serv.checkPassWord(inputMessage.getPrmtrs()[0], user));
	return (1);
}

int Commands::NICK(InputMessage &inputMessage, User &user, Server &serv)
{
	std::string prevNickName = user.getNickName();
	if (inputMessage.getPrmtrs().size() == 0)
		return (automaticReplies(user, Replies::ERR_NONICKNAMEGIVEN()));
	else if (serv.checkNick(inputMessage.getPrmtrs()[0]) == 1)
		return (automaticReplies(user, Replies::ERR_ERRONEUSNICKNAME(inputMessage)));
	else if (serv.inUseNick(inputMessage.getPrmtrs()[0]) == 1)
		return (automaticReplies(user, Replies::ERR_NICKNAMEINUSE(inputMessage)));
	else
	{
		Tracer::Instance()->inFunc("Entered NICK");
		user.setNickName(inputMessage.getPrmtrs()[0]);
		Tracer::Instance()->writeInFile("Nick successfully added");
		if (prevNickName != "default nickname")
		{
			if (user.getStatus() == 2)
				std::cout	<< "user (" << prevNickName << ") has changed his nickname to (" 
							<< user.getNickName() << ")" << std::endl;
			int res = user.sendMessage(Replies::NICK(user.getNickName(), prevNickName));
			if (res < 0)
				Tracer::Instance()->writeInFile(std::string("!!!Error: ") + strerror(errno) + "!!!");
		}
		else if (user.getRealName() != "default realname")
		{
			std::string msgToSend = ":127.0.0.1 001 " + user.getNickName() + " :" +  WELCOME_REPLY + " " + user.getNickName() + "!"
										+ user.getUserName() + "@" + user.getHostName() + "\r\n";
			int res = user.sendMessage(msgToSend);
			if (res < 0)
				Tracer::Instance()->writeInFile(std::string("!!!Error: ") + strerror(errno) + "!!!");
			else
			{
				Tracer::Instance()->writeInFile("Reply(" + msgToSend + ") has been sended to user(" + user.getNickName() + ")");
				Tracer::Instance()->writeInFile("user (" + user.getNickName() + ") has entered in server");
				std::cout << "user (" << user.getNickName() << ") has entered in server" << std::endl;
				user.setStatus(2);
			}
		}
		Tracer::Instance()->outFunc("Exited NICK");
		return (0);
	}
	return (1);
}

int Commands::USER(InputMessage &inputMessage, User &user)
{
	if (inputMessage.getPrmtrs().size() == 0)
		return (automaticReplies(user, Replies::ERR_NEEDMOREPARAMS(inputMessage.getCmnd())));
	else if (user.getStatus() == 0)
		return (automaticReplies(user, Replies::ERR_ALREADYREGISTRED()));
	else
	{
		Tracer::Instance()->inFunc("Entered USER");
		if (user.getStatus() != 1 && inputMessage.getPrmtrs().size() > 3 && 
			(user.getUserName() != inputMessage.getPrmtrs()[0] || user.getRealName() != inputMessage.getPrmtrs()[3]))
			std::cout << "user (" << user.getNickName() << ") changed his username to (" <<
							inputMessage.getPrmtrs()[0] << ") or realname to (" << 
							inputMessage.getPrmtrs()[3] << ")" << std::endl;
		user.setUserName(inputMessage.getPrmtrs()[0]);
		if (inputMessage.getPrmtrs().size() > 3)
			user.setRealName(inputMessage.getPrmtrs()[3]);
		if (user.getStatus() == 1 && user.getNickName() != "default nickname")
		{
			struct hostent* host = gethostbyname("127.0.0.1");
			if (!host)
				Tracer::Instance()->writeInFile("!!!ERROR: Couldnt get host!!!");
			else
			{
				std::string msgToSend = ":127.0.0.1 001 " + user.getNickName() + " :" +  WELCOME_REPLY + " " + user.getNickName() + "!"
										+ user.getUserName() + "@" + user.getHostName() + "\r\n";
				int res = user.sendMessage(msgToSend);
				if (res < 0)
					Tracer::Instance()->writeInFile(std::string("!!!Error: ") + strerror(errno) + "!!!");
				else
				{
					Tracer::Instance()->writeInFile("Reply(" + msgToSend + ") has been sended to user(" + user.getNickName() + ")");
					Tracer::Instance()->writeInFile("user (" + user.getNickName() + ") has entered in server");
					std::cout << "user (" << user.getNickName() << ") has entered in server" << std::endl;
					user.setStatus(2);
				}
			}
		}
		Tracer::Instance()->outFunc("Exited USER");
		return (0);
	}
	return (1);
}

int Commands::QUIT(InputMessage &inputMessage, User &user)
{
	Tracer::Instance()->inFunc("Entered QUIT");
	
	if (inputMessage.getPrmtrs().size() > 1)
	{
		
		std::string quitMessage = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " QUIT";
		quitMessage = quitMessage + " :" + inputMessage.getPrmtrs()[0];
		Tracer::Instance()->writeInFile("!Warning: wrong number of parameters!");
		Tracer::Instance()->outFunc("Exited QUIT");
		return (user.sendMessage(quitMessage));
	}
	if (user.getStatus() == 2)
		user.setStatus(-2);
	else
		user.setStatus(-1);
	Tracer::Instance()->outFunc("Exited QUIT");
	return (1);
}


int Commands::PRIVMSG(InputMessage &inputMessage, User &user, Server &serv)
{
	Tracer::Instance()->inFunc("Entered PRIVMSG");

	if (inputMessage.getPrmtrs().size() == 0)
	{
		Tracer::Instance()->outFunc("Exited PRIVMSG -> ERR_NORECIPIENT");
		return (automaticReplies(user, Replies::ERR_NORECIPIENT(inputMessage.getCmnd())));
	}
	else if (inputMessage.getPrmtrs().size() == 1)
	{
		Tracer::Instance()->outFunc("Exited PRIVMSG -> ERR_NOTEXTTOSEND");
		return (automaticReplies(user, Replies::ERR_NOTEXTTOSEND()));
	}

	std::string msgText = inputMessage.getPrmtrs()[1];

	std::vector<std::string> targets = ft_separator(inputMessage.getPrmtrs()[0], ",");
	std::string::iterator it, pos;
	std::string msgToSend;

	for (size_t i = 0; i < targets.size(); ++i) // check presence of target
	{
		if (ft_find_duplicate(targets, targets[i]) > 1)
		{
			Tracer::Instance()->outFunc("Exited PRIVMSG -> ERR_TOOMANYTARGETS");
			return (automaticReplies(user, Replies::ERR_TOOMANYTARGETS(targets[i])));
		}
		if (targets[i][0] == '#') // if target is a channel
		{
			std::string findChannel = targets[i];
			Tracer::Instance()->writeInFile("channel to send: " + findChannel);
			Tracer::Instance()->writeInFile("channel to send: " + serv.getChannelMap().begin()->first);
			if (serv.getChannelMap().find(findChannel) == serv.getChannelMap().end())
			{
				Tracer::Instance()->outFunc("Exited PRIVMSG -> ERR_CANNOTSENDTOCHAN");
				return (automaticReplies(user, Replies::ERR_CANNOTSENDTOCHAN(targets[i])));
			}
		}
		else // if target is a user
		{
			std::map<int, User>::iterator it = serv.getUserMap().begin();
			std::map<int, User>::iterator ite = serv.getUserMap().end();
			for (; it != ite; ++it)
				if (it->second.getNickName() == targets[i])
					break ;
			if (it == ite)
			{
				Tracer::Instance()->outFunc("Exited PRIVMSG -> ERR_NOSUCHNICK");
				return (automaticReplies(user, Replies::ERR_NOSUCHNICK(targets[i])));
			}
		}
	}
	
	for (size_t i = 0; i < targets.size(); ++i)
	{
		msgToSend = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " PRIVMSG " + targets[i] + " :" + msgText + "\r\n";
		if (targets[i][0] == '#')
		{
			std::string channelToSend = targets[i];
			std::map<std::string, Channel>::iterator tmp = serv.getChannelMap().find(channelToSend);
			//Tracer::Instance()->writeInFile("send message: " + msgToSend + " to channel: " + tmp->second.getName());
			tmp->second.broadcast_except(msgToSend, &user);
		}
		else
		{
			std::map<int, User>::iterator it = serv.getUserMap().begin();
			std::map<int, User>::iterator ite = serv.getUserMap().end();
			for (; it != ite; ++it)
				if (it->second.getNickName() == targets[i])
					break ;
			Tracer::Instance()->writeInFile("send message: " + msgToSend + " to user:" + it->second.getNickName());
			send(it->second.getFd(), msgToSend.c_str(), msgToSend.size(), 0);
		}
	}
	Tracer::Instance()->outFunc("Exited PRIVMSG");
	return (1);
}

int Commands::NOTICE(InputMessage &inputMessage, User &user, Server &serv)
{
	Tracer::Instance()->inFunc("Entered NOTICE");

	if (inputMessage.getPrmtrs().size() == 0)
	{
		Tracer::Instance()->outFunc("Exited NOTICE -> ERR_NORECIPIENT");
		return (0);
	}
	else if (inputMessage.getPrmtrs().size() == 1)
	{
		Tracer::Instance()->outFunc("Exited NOTICE -> ERR_NOTEXTTOSEND");
		return (0);
	}

	std::string msgText = inputMessage.getPrmtrs()[1];

	std::vector<std::string> targets = ft_separator(inputMessage.getPrmtrs()[0], ",");
	std::string::iterator it, pos;
	std::string msgToSend;

	for (size_t i = 0; i < targets.size(); ++i) // check presence of target
	{
		if (ft_find_duplicate(targets, targets[i]) > 1)
		{
			Tracer::Instance()->outFunc("Exited NOTICE -> ERR_TOOMANYTARGETS");
			return (0);
		}
		if (targets[i][0] == '#') // if target is a channel
		{
			std::string findChannel = targets[i];
			Tracer::Instance()->writeInFile("channel to send: " + findChannel);
			Tracer::Instance()->writeInFile("channel to send: " + serv.getChannelMap().begin()->first);
			if (serv.getChannelMap().find(findChannel) == serv.getChannelMap().end())
			{
				Tracer::Instance()->outFunc("Exited NOTICE -> ERR_CANNOTSENDTOCHAN");
				return (0);
			}
		}
		else // if target is a user
		{
			std::map<int, User>::iterator it = serv.getUserMap().begin();
			std::map<int, User>::iterator ite = serv.getUserMap().end();
			for (; it != ite; ++it)
				if (it->second.getNickName() == targets[i])
					break ;
			if (it == ite)
			{
				Tracer::Instance()->outFunc("Exited NOTICE -> ERR_NOSUCHNICK");
				return (0);
			}
		}
	}
	
	for (size_t i = 0; i < targets.size(); ++i)
	{
		msgToSend = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " NOTICE " + targets[i] + " :" + msgText + "\r\n";
		if (targets[i][0] == '#')
		{
			std::string channelToSend = targets[i];
			std::map<std::string, Channel>::iterator tmp = serv.getChannelMap().find(channelToSend);
			tmp->second.broadcast_except(msgToSend, &user);
		}
		else
		{
			std::map<int, User>::iterator it = serv.getUserMap().begin();
			std::map<int, User>::iterator ite = serv.getUserMap().end();
			for (; it != ite; ++it)
				if (it->second.getNickName() == targets[i])
					break ;
			Tracer::Instance()->writeInFile("send message: " + msgToSend + " to user:" + it->second.getNickName());
			send(it->second.getFd(), msgToSend.c_str(), msgToSend.size(), 0);
		}
	}
	Tracer::Instance()->outFunc("Exited NOTICE");
	return (1);
}

void leaveChannels(InputMessage &inputMessage, User &user, Server &serv)
{
	std::vector<Channel *> &chanVec = user.getChannels();
	std::size_t				userSize;
	std::string				channelName;

	Tracer::Instance()->inFunc("Entered leaveChannels");
	for (std::vector<Channel *>::iterator it = chanVec.begin(); it != chanVec.end();)
	{
		std::string partMessage = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " PART " + (*it)->getName();
		if (inputMessage.getPrmtrs().size() > 1)
					partMessage  = partMessage + " :" + inputMessage.getPrmtrs()[1];
		partMessage = partMessage + "\r\n";
		Tracer::Instance()->writeInFile("Builded message: " + partMessage);
		std::cout << "user (" << user.getNickName() << ") left channel (" << (*it)->getName() << ")" << std::endl; 
		userSize = (*it)->getUsers().size();
		channelName = (*it)->getName();
		(*it)->broadcast(partMessage);
		(*it)->deleteUser(user.getNickName());
		if (userSize == 1)
		{
			Tracer::Instance()->writeInFile("Remove channel from server");
		 	serv.removeChannelMap(channelName);
			std::cout << "channel (" << channelName << ") has been removed from channel" << std::endl; 
		}	
	}
	Tracer::Instance()->outFunc("Exited leaveChannels");
}

int Commands::JOIN(InputMessage &inputMessage, User &user, Server &serv)
{
	if(inputMessage.getPrmtrs().size() == 0)
	{
		return (automaticReplies(user, Replies::ERR_NEEDMOREPARAMS(inputMessage)));
	}
	else if ((inputMessage.getPrmtrs()[0] == "#0") || (inputMessage.getPrmtrs()[0] == "0"))
	{
		Tracer::Instance()->inFunc("Entered JOIN");
		leaveChannels(inputMessage, user, serv);
		Tracer::Instance()->outFunc("Exited JOIN");
		return (1);
	}

	Tracer::Instance()->inFunc("Entered JOIN");
	std::vector<std::string> chanVec = ft_separator(inputMessage.getPrmtrs()[0], ",");
	std::vector<std::string> keysVec = std::vector<std::string>();
	
	std::stringstream stream;
	stream << "number of channels to join: " << chanVec.size();
	Tracer::Instance()->writeInFile(stream.str());
	stream.str("");

	if (inputMessage.getPrmtrs().size() > 1)
		keysVec = ft_separator(inputMessage.getPrmtrs()[1], ",");

	stream << "number of channel's keys: " << keysVec.size();
	Tracer::Instance()->writeInFile(stream.str());
	stream.str("");

	std::vector<std::string>::iterator itKey = keysVec.begin();
	
	for (std::vector<std::string>::iterator it = chanVec.begin(); it != chanVec.end(); ++it)
	{
		std::string channelName = *it;
		Tracer::Instance()->writeInFile(channelName);
		if (channelName.c_str()[0] != '#')
		{
			Tracer::Instance()->outFunc("Exited JOIN");
			return (automaticReplies(user, Replies::ERR_BADCHANMASK(*it)));
		}
		
		if (serv.getChannelMap().find(*it) == serv.getChannelMap().end())
		{
			std::map<std::string, Channel>::iterator chanIt = serv.addChannelMap(channelName);
			
			std::cout << "channel (" << channelName << ") has been created" << std::endl; 
			chanIt->second.addUser(user);
			chanIt->second.addOperator(user);
			std::string newKey;
			if (itKey != keysVec.end())
			{
				newKey = *itKey;
				++itKey;
			}
			chanIt->second.setKey(newKey, user.getNickName());
			std::string joinMessage = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " JOIN :" + chanIt->second.getName() + "\r\n";
			chanIt->second.broadcast(joinMessage);
			user.sendMessage(Replies::RPL_WELCOMECHAN(chanIt->second.getName(), user.getNickName()));
			user.sendMessage(Replies::RPL_NAMREPLY(chanIt->second.getName(), user.getNickName(), chanIt->second));
			user.sendMessage(Replies::RPL_ENDOFNAMES(chanIt->second.getName(), user.getNickName()));
			std::cout << "user (" << user.getNickName() << ") has joined to the channel (" << chanIt->second.getName() << ")" << std::endl; 
		}
		else
		{
			std::string key;
			if (itKey != keysVec.end())
			{
				key = *itKey;
				++itKey;
			}
			Channel &joinChan = serv.getChannelMap().find(*it)->second;
			if (joinChan.checkKey(key) == false)
			{
				Tracer::Instance()->outFunc("Exited JOIN");
				return(automaticReplies(user, Replies::ERR_BADCHANNELKEY(channelName)));
			}
			joinChan.addUser(user);
			std::string joinMessage = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " JOIN :" + joinChan.getName() + "\r\n";
			joinChan.broadcast(joinMessage);
			user.sendMessage(Replies::RPL_WELCOMECHAN(joinChan.getName(), user.getNickName()));
			user.sendMessage(Replies::RPL_NAMREPLY(joinChan.getName(), user.getNickName(), joinChan));
			user.sendMessage(Replies::RPL_ENDOFNAMES(joinChan.getName(), user.getNickName()));
			std::cout << "user (" << user.getNickName() << ") has joined to the channel (" << joinChan.getName() << ")" << std::endl; 
		}
	}
	return (0);
}

int Commands::PART(InputMessage &inputMessage, User &user, Server &serv)
{
	Tracer::Instance()->inFunc("Entered PART");

	if(inputMessage.getPrmtrs().size() == 0)
	{
		Tracer::Instance()->outFunc("Exited PART->ERR_NEEDMOREPARAMS");
		return (automaticReplies(user, Replies::ERR_NEEDMOREPARAMS(inputMessage)));
	}
	
	std::vector<Channel *> userChan = user.getChannels();
	std::vector<Channel *>::iterator tmpIt;

	std::vector<std::string> chanVec = ft_separator(inputMessage.getPrmtrs()[0], ",");
	for (std::vector<std::string>::iterator it = chanVec.begin(); it != chanVec.end(); ++it)
	{
		if ((tmpIt = Channel::findChanInVec(userChan, *it)) == userChan.end())
		{
			Tracer::Instance()->writeInFile("Exited PART->ERR_NOSUCHCHANNEL");
			automaticReplies(user, Replies::ERR_NOSUCHCHANNEL(*it));
			continue ;
		}
		else
		{
			Channel *tmpChan = *tmpIt;

			if (tmpChan->isUser(user.getNickName()) == 0)
			{
				Tracer::Instance()->writeInFile("Exited PART->ERR_NOTONCHANNEL");
				automaticReplies(user, Replies::ERR_NOTONCHANNEL(*it));
				continue ;
			}
			else
			{
				std::string partMessage = ":" + user.getNickName() + "!" + user.getUserName() + "@" + user.getHostName() + " PART " + tmpChan->getName();
				if (inputMessage.getPrmtrs().size() > 1)
					partMessage  = partMessage + " :" + inputMessage.getPrmtrs()[1];
				partMessage = partMessage + "\r\n";
				Tracer::Instance()->writeInFile("Builded message: " + partMessage);
				tmpChan->broadcast(partMessage);
				tmpChan->deleteUser(user.getNickName());
				std::cout << "user (" << user.getNickName() << ") left channel (" << tmpChan->getName() << ")" << std::endl; 

				if (tmpChan->getUsers().size() == 0)
				{
					Tracer::Instance()->writeInFile("Remove channel from server");
					std::cout << "channel (" << tmpChan->getName() << ") has been removed from channel" << std::endl; 
					serv.removeChannelMap(tmpChan->getName());
				}
			}
		}
	}
	Tracer::Instance()->outFunc("Exited PART");
	return (0);
}

int Commands::KICK(InputMessage &inputMessage, User &user, Server &serv)
{
	Tracer::Instance()->inFunc("Entered KICK");

	if(inputMessage.getPrmtrs().size() < 2)
	{
		Tracer::Instance()->outFunc("Exited KICK->ERR_NEEDMOREPARAMS");
		return (automaticReplies(user, Replies::ERR_NEEDMOREPARAMS(inputMessage)));
	}

	std::vector<std::string> chanVec = ft_separator(inputMessage.getPrmtrs()[0], ",");
	std::vector<std::string> userVec = ft_separator(inputMessage.getPrmtrs()[1], ",");

	for(std::vector<std::string>::iterator it = chanVec.begin(); it != chanVec.end(); ++it)
	{
		if (serv.getChannelMap().find(*it) == serv.getChannelMap().end())
		{
			Tracer::Instance()->writeInFile("Exited KICK->ERR_NOSUCHCHANNEL");
			automaticReplies(user, Replies::ERR_NOSUCHCHANNEL(*it));
			continue ;
		}

		Channel &tmpChan = serv.getChannelMap().find(*it)->second;
		if (tmpChan.isOper(user.getNickName()) == 0)
		{
			Tracer::Instance()->writeInFile("Exited KICK->ERR_CHANOPRIVSNEEDED");
			automaticReplies(user, Replies::ERR_CHANOPRIVSNEEDED(*it));
			continue ;
		}

		for (std::vector<std::string>::iterator it_user = userVec.begin(); it_user != userVec.end(); ++it_user)
		{
			if (tmpChan.isUser(*it_user) == 0)
			{
				Tracer::Instance()->writeInFile("Exited KICK->ERR_USERNOTINCHANNEL");
				automaticReplies(user, Replies::ERR_USERNOTINCHANNEL(*it_user, *it));
				continue ;
			}
			else
			{
				std::stringstream stream;
				stream << ":" << user.getNickName() << "!" << user.getUserName() << "@" << user.getHostName() << " KICK " << *it << " " << *it_user; 
				if (inputMessage.getPrmtrs().size() > 2)
					stream << " :" << inputMessage.getPrmtrs()[2];
				stream << "\r\n";
				tmpChan.broadcast(stream.str());
				tmpChan.deleteUser(*it_user);
				std::cout << "user (" << *it_user << ") left channel (" << *it << ")" << std::endl; 
			}
		}
	}
	Tracer::Instance()->outFunc("Exited KICK");
	return (0);
}
