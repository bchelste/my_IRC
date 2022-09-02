
#include "Replies.hpp"

// PASS replies:

std::string Replies::ERR_NEEDMOREPARAMS(InputMessage inputMessage) // 461
{
	std::string tmp;
	tmp = inputMessage.getCmnd() + " :Not enough parameters" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_ALREADYREGISTRED() // 462
{
	return (" :You may not reregister\r\n");
}

// NICK replies:

std::string Replies::ERR_NONICKNAMEGIVEN() // 431
{
	return (":No nickname given\r\n");
}

std::string Replies::ERR_NICKNAMEINUSE(InputMessage inputMessage) //433
{
	std::string tmp;
	tmp = inputMessage.getPrmtrs()[0] + " :Nickname is already in use" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_ERRONEUSNICKNAME(InputMessage inputMessage) // 432
{
	std::string tmp;
	tmp = inputMessage.getPrmtrs()[0] + " :Erroneus nickname" + "\r\n";
	return (tmp);
}

/* WE DO NOT USE 
std::string ERR_NICKCOLLISION(std::string nick) // 436 
>>>>>>> f7bfc50ece703e321609151189726a03d461a880
{
	std::string tmp;
	tmp = nick + " :Nickname collision KILL";
	return (tmp);
}
*/
// USER replies:
// ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED

// PRIVMSG replies:

std::string Replies::ERR_NORECIPIENT(std::string command) // 411
{
	std::string tmp;
	tmp = ":No recipient given (" + command + ")" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_CANNOTSENDTOCHAN(std::string channel_name) // 404
{
	std::string tmp;
	tmp = channel_name + " :Cannot send to channel" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_WILDTOPLEVEL(std::string mask) // 414
{
	std::string tmp;
	tmp = mask + " :Wildcard in toplevel domain" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_NOSUCHNICK(std::string nickname) // 401
{
	std::string tmp;
	tmp = nickname + " :No such nick/channel" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_NOTEXTTOSEND() // 412
{
	return (":No text to send\r\n");
}

std::string Replies::ERR_NOTOPLEVEL(std::string mask) // 413
{
	std::string tmp;
	tmp = mask + " :No toplevel domain speciified" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_TOOMANYTARGETS(std::string target) // 407
{
	std::string tmp;
	tmp = target + " :Duplicate recipients. No message delivered" + "\r\n";
	return (tmp);
}

std::string Replies::RPL_AWAY(std::string nick, std::string awayMessage) // 301
{
	std::string tmp;
	tmp = nick + " :" + awayMessage + "\r\n";
	return (tmp);
}

// NOTICE replies: 
// replies must never be sent

// PART replies:

std::string Replies::ERR_NOSUCHCHANNEL(std::string channelName) // 403
{
	std::string tmp;
	tmp = channelName + " :No such channel" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_NOTONCHANNEL(std::string channelName) // 442
{
	std::string tmp;
	tmp = channelName + " :You're not on that channel" + "\r\n";
	return (tmp);
}

// JOIN replies:

std::string Replies::ERR_BADCHANMASK(std::string channelName) // 476
{
	std::string tmp;
	tmp = channelName + " :Bad Channel Mask" + "\r\n";
	return (tmp);
}

std::string Replies::ERR_BADCHANNELKEY(std::string channelName) // 475
{
	std::string tmp;
	tmp = channelName + " :Cannot join channel (+k)";
	return (tmp);
}

// KICK replies:

std::string Replies::ERR_CHANOPRIVSNEEDED(std::string channelName) // 482
{
	std::string tmp;
	tmp = channelName + " :You're not channel operator\r\n";
	return (tmp);
}

std::string Replies::ERR_USERNOTINCHANNEL(std::string channelName, std::string userName) // 441
{
	std::string tmp;
	tmp = userName + " " + channelName + " :They aren't on that channel\r\n";
	return (tmp);
}

std::string Replies::RPL_WELCOMECHAN(const std::string &channelName, const std::string &userName)
{
	std::string tmp;
	tmp = ":127.0.0.1 332 " + userName + " " + channelName + " :Welcome to the channel\r\n";
	return (tmp);
}

std::string Replies::RPL_NAMREPLY(const std::string &channelName, const std::string &userName, Channel &channel)
{
	std::stringstream stream;
	std::map<const std::string*, User*> users = channel.getUsers();

	stream << ":127.0.0.1 353 " << userName << " = " << channelName << " :";
	for (std::map<const std::string*, User*>::iterator it = users.begin();;)
	{
		if (channel.isOper(*(it->first)))
			stream << "@";
		stream << *(it->first);
		if (++it != users.end())
			stream << " ";
		else
			break;
	}
	stream << "\r\n";
	return (stream.str());
}

std::string Replies::RPL_ENDOFNAMES(const std::string &channelName, const std::string &userName)
{
	std::stringstream stream;
	stream << ":127.0.0.1 366 " << userName << " " << channelName << " :END of NAMES list\r\n";
	return (stream.str());
}


std::string Replies::NICK(const std::string& nickname, const std::string& oldNick)
{
	return (":" + oldNick + " NICK " + nickname + "\r\n");
}
