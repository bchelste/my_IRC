#pragma once

#include <map>
#include "sys/socket.h"
#include "netinet/in.h"
#include "poll.h"
#include "unistd.h"
#include "strings.h"
#include "cstring"
#include "iostream"
#include "errno.h"
#include "fcntl.h"
#include "Utils.hpp"
#include "Commands.hpp"
#include "Channel.hpp"
#include "Tracer.hpp"
#define MAX_CONNECTION	100
#define	MAX_BUF			128

class Commands;
class User;
class Channel;

//add :public Commands to Server

class Server{
public:
    Server(uint port, std::string password);
	Server(const Server&);
	~Server();

	Server	&operator=(const Server&);
	int		SetupListen();
	void	WaitAction();

	void	changeNickUser(std::string prevNickName, std::string newNickName);
	int		inUseNick(std::string newNick);
	int		checkNick(std::string newNick);

	int 	checkPassWord(std::string newPassWord, User &user);
	int		useCommand(InputMessage inputMessage, User &user);

	std::map<int, User>::iterator	findUser(int fd);

	std::map<int, User> 			&getUserMap();
	std::map<std::string, Channel>	&getChannelMap();

	std::map<std::string, Channel>::iterator	addChannelMap(const std::string &name);
	void										removeChannelMap(const std::string &name);

private:
	struct pollfd	fds[MAX_CONNECTION];
	int				nfds;
	int				sockfd;
    uint    		port;
	std::string		password;
	
	std::map<int, User>	users;
	std::map<std::string, Channel> channels;

	enum Status
	{
		UP = 0,
		SOCK_ERR = 1,
		BIND_ERR = 2,
		LISTEN_ERR = 3,
		SET_SOCK_ERR = 4,
		ERR_POLL = 5,
		FCNTL_ERR = 6,
		CLOSE_SERV = 7
	};

	Status			serv_status;

	int		CheckFDS(int i, bool *resizeArray);
	void	AcceptConnections();
	void	RecieveData(int i, bool *resizeArray);
	void	ResizeArray();
	
};