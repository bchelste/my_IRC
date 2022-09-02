#ifndef REPLIES_HPP
# define REPLIES_HPP

#include <string>
#include "InputMessage.hpp"
#include "Channel.hpp"

class Channel;

struct Replies
{
    Replies() {};
    virtual ~Replies() {};
    // PASS replies:

    static std::string ERR_NEEDMOREPARAMS(InputMessage inputMessage); // 461
    static std::string ERR_ALREADYREGISTRED(); // 462

    // NICK replies:

    static std::string ERR_NONICKNAMEGIVEN(); // 431
    static std::string ERR_NICKNAMEINUSE(InputMessage inputMessage); //433
    static std::string ERR_ERRONEUSNICKNAME(InputMessage inputMessage); // 432
    static std::string ERR_NICKCOLLISION(InputMessage inputMessage); // 436 

    // USER replies:
    // ERR_NEEDMOREPARAMS, ERR_ALREADYREGISTRED

    // PRIVMSG replies:

    static std::string ERR_NORECIPIENT(std::string command); // 411
    static std::string ERR_CANNOTSENDTOCHAN(std::string channel_name); // 404
    static std::string ERR_WILDTOPLEVEL(std::string mask); // 414
    static std::string ERR_NOSUCHNICK(std::string nickname); // 401
    static std::string ERR_NOTEXTTOSEND(); // 412
    static std::string ERR_NOTOPLEVEL(std::string mask); // 413
    static std::string ERR_TOOMANYTARGETS(std::string target); // 407
    static std::string RPL_AWAY(std::string nick, std::string awayMessage); // 301

    // NOTICE replies: 
    // replies must never be sent

	// PART replies:

	static std::string ERR_NOSUCHCHANNEL(std::string channelName); // 403
	static std::string ERR_NOTONCHANNEL(std::string channelName); // 442

	// JOIN replies:

	static std::string ERR_BADCHANMASK(std::string channelName); // 476
	static std::string ERR_BADCHANNELKEY(std::string channelName); // 475
    
	// KICK replies:

	static std::string ERR_CHANOPRIVSNEEDED(std::string channelName); // 482
	static std::string ERR_USERNOTINCHANNEL(std::string channelName, std::string userName); // 441

    static std::string RPL_WELCOMECHAN(const std::string &channelName, const std::string &userName);
    static std::string RPL_NAMREPLY(const std::string &channelName, const std::string &userName, Channel&);
    static std::string RPL_ENDOFNAMES(const std::string &channelName, const std::string &userName);


    static std::string NICK(const std::string&, const std::string&);

    #define WELCOME_REPLY "Welcome to the Internet Relay Network"
};

#endif