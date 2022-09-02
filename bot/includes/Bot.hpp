#pragma once

#include <map>
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "poll.h"
#include "unistd.h"
#include "strings.h"
#include "cstring"
#include "iostream"
#include "errno.h"
#include "fcntl.h"
#include "Utils.hpp"
#include "stdlib.h"
#include "fstream"
#include "queue"
#define	MAX_BUF			128

class Bot{
public:
    Bot(std::string file);
	Bot(const Bot&);
	~Bot();

	Bot	&operator=(const Bot&);
	int				SetupConnection();
	int				LogIn();
	void			WaitAction();
	std::string		checkMessage(const std::string &message, std::string &channel);
	int 			createMsgQueue(const std::string& src);
	void			processMessage();
	void			processCommand(const std::string& cmd, const std::string& channel);

	void			setFactoid(const std::string& cmd, std::size_t pos, const std::string &channel);
	void			accessFactoid(const std::string& cmd, std::size_t pos, const std::string &channel);
	void			forgetFactoid(const std::string& cmd, const std::string &channel);
	void			joinChannel(const std::string &channel);
	void			partChannel(const std::string &channel);

private:
	
	int															botfd;
	int															servfd;
    std::string													port;
	std::string													ip;
	std::string													password;
	std::string													channel;
	std::string													nick;
	std::queue<std::string> 									msgQueue;
	std::map<std::string, std::map<std::string, std::string> > 	factoids;

	enum Status
	{
		UP = 0,
		SOCK_ERR = 1,
		BIND_ERR = 2,
		LISTEN_ERR = 3,
		SET_SOCK_ERR = 4,
		ERR_POLL = 5,
		FCNTL_ERR = 6,
		CLOSE_SERV = 7,
		RCV_ERR = 8
	};

	Status			bot_status;
};