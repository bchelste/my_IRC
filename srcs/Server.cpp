#include "Server.hpp"

Server::Server(uint port, std::string password)
{
	this->port = port;
	this->password = password;

	sockfd = 0;
	bzero(fds, sizeof(fds));
	nfds = 0;
	serv_status = UP;
}

Server::Server(const Server& obj)
{
	*this = obj;
}

Server	&Server::operator=(const Server& obj)
{
	if (this == &obj)
		return (*this);
	port = obj.port;
	password = obj.password;
	//users = obj.users;
	//channels = obj.channels;
	memcpy(fds, obj.fds, sizeof(fds));
	sockfd = obj.sockfd;
	nfds = obj.nfds;
	serv_status = obj.serv_status;
	return (*this);
}

Server::~Server()
{

}

int		Server::SetupListen()
{
	Tracer*	trace = Tracer::Instance();
	struct	sockaddr_in	addr;
	int		bReuseAddr = 1;

	trace->inFunc("Entered in SetupListen");
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		trace->outFunc("Exited in SetupListen");
		return (serv_status = SOCK_ERR);
	}
	trace->writeInFile("Socket ready!");
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(sockfd);
		trace->outFunc("Exited in SetupListen");
		return (serv_status = FCNTL_ERR);
	}
	trace->writeInFile("FCNTL ready!");
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &bReuseAddr, sizeof(int)) < 0)
	{
		close(sockfd);
		trace->outFunc("Exited in SetupListen");
		return (serv_status = SET_SOCK_ERR);
	}
	trace->writeInFile("setsocketopt ready!");
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0)
	{
		close(sockfd);
		trace->outFunc("Exited in SetupListen");
		return (serv_status = BIND_ERR);
	}
	trace->writeInFile("bind ready!");
	if (listen(sockfd, 8) < 0)
	{
		close(sockfd);
		trace->outFunc("Exited in SetupListen");
		return (serv_status = LISTEN_ERR);
	}
	trace->writeInFile("Server is listening!");
	trace->outFunc("Exited SetupListen");
	return (UP);
}

void		Server::WaitAction()
{
	int		timeout, res;
	bool	resizeArray = false;
	Tracer	*logger = Tracer::Instance();
	
	logger->inFunc("Entered in WaitAction");
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;
	nfds = 1;
	timeout = 360000;
	while (serv_status == UP)
	{
		logger->writeInFile("Waiting on poll...");
		if ((res = poll(fds, nfds, timeout)) <= 0)
		{
			if (!res)
				logger->writeInFile("!!!ERROR: Poll timeout!!!");
			else
				logger->writeInFile(std::string("!!!ERROR: Poll error(") + strerror(errno) + ")");
			break;
		}
		logger->writeInFile("Check info from poll...");
		for (int i = 0; i < nfds; ++i)
			if (CheckFDS(i, &resizeArray))
				break;
		if (resizeArray)
		{
			logger->writeInFile("Resize array...");
			resizeArray = false;
			ResizeArray();
		}
	}
	for (int i = 0; i < nfds; ++i)
		if (fds[i].fd >= 0)
			close(fds[i].fd);
	logger->outFunc("Exited WaitAction");
}

int		Server::CheckFDS(int i, bool *resizeArray)
{
	if (fds[i].revents == 0)
		return (0);
	if (fds[i].revents != POLLIN)
	{
		Tracer::Instance()->writeInFile("!!!ERROR: Revents fd error!!!");
		serv_status = CLOSE_SERV;
		return (1);
	}
	if (fds[i].fd == sockfd)
		AcceptConnections();
	else
		RecieveData(i, resizeArray);
	return (0);
}

void		Server::AcceptConnections()
{
	int new_sd = 0;

	Tracer::Instance()->inFunc("Entered AcceptConnections");
	while (new_sd >= 0)
	{
		Tracer::Instance()->writeInFile("Try accept connection...");
		new_sd = accept(sockfd, NULL, NULL);
		if (new_sd < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				Tracer::Instance()->writeInFile("!!!Error: Couldnt accept!!!");
				serv_status = CLOSE_SERV;
			}
			break;
		}
		fds[nfds].fd = new_sd;
		fds[nfds].events = POLLIN;
		++nfds;
		Tracer::Instance()->writeInFile("Setup fcntl...");
		if (fcntl(new_sd, F_SETFL, O_NONBLOCK) < 0)
		{
			close(new_sd);
			Tracer::Instance()->writeInFile("!!!Error: Couldnt accept!!!");
			serv_status = CLOSE_SERV;
			break;
		}
		Tracer::Instance()->writeInFile("Add connection...");
		users.insert(std::make_pair(dup(new_sd), User(new_sd)));
		Tracer::Instance()->outFunc("Exited AcceptConnections");
	}
}

void		Server::RecieveData(int i, bool *resizeArray)
{
	bool			bCloseConnection = false;
	int				res;
	char			buffer[MAX_BUF];

	Tracer::Instance()->inFunc("Entered ReceiveData");
	while(!bCloseConnection)
	{
		Tracer::Instance()->writeInFile("Receive data...");
		res = recv(fds[i].fd, &buffer[0], MAX_BUF - 1, 0);
		if (res < 0)
		{
			if (errno != EWOULDBLOCK)
			{
				Tracer::Instance()->writeInFile("!!!ERROR: couldn't receive data!!!");
				bCloseConnection = true;
			}
			break;
		}
		if (!res)
		{
			Tracer::Instance()->writeInFile("Blank message...");
			bCloseConnection = true;
			break;
		}
		{
			std::stringstream stream;
			stream << res << " , MAX_BUF = " << MAX_BUF;
			Tracer::Instance()->writeInFile(std::string("Test res: ") + stream.str());
		}
		
		std::string		message(buffer, res);
		Tracer::Instance()->writeInFile("Received message: " + message);

		// part of command parser:

		Tracer::Instance()->writeInFile("Parse message for commands...");
		std::map<int, User>::iterator itEnd = users.end();
		for (std::map<int, User>::iterator it = users.begin(); it != itEnd; ++it)
		{
			if (same_file(fds[i].fd, it->first))
			{
				Tracer::Instance()->writeInFile("Create queue of commands");
				it->second.createCmdQueue(message);

				std::string *command;
				while (it->second.cmdQueue.size())
				{
					command = &it->second.cmdQueue.front();
					if (command->find("\r\n", 0) == std::string::npos)
						break;
					*command = command->substr(0, command->length() - 2);
					Tracer::Instance()->writeInFile("Use command (" + *command + ") from queue");
					useCommand(*command, it->second);
					it->second.cmdQueue.pop();
					if (it->second.getStatus() < 0)
					{
						bCloseConnection = true;
						break;
					}
					{
						std::stringstream	stream;
						stream << "size of cmdQueue = " << it->second.cmdQueue.size();
						Tracer::Instance()->writeInFile(stream.str());
					}
				}
				break ;
			}
		}
	}
	if (bCloseConnection)
	{
		std::map<int, User>::iterator	userIt;
		std::map<std::string, Channel>::iterator tmpIt;
		for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end();)
		{
			userIt = findUser(fds[i].fd);
			if (userIt != users.end())
			{
				std::string partMessage = ":" + userIt->second.getNickName() + "!" + userIt->second.getUserName() + 
											"@" + userIt->second.getHostName() + " PART " + it->second.getName() + "\r\n";
				it->second.broadcast(partMessage);
				std::cout << "user (" << userIt->second.getNickName() << ") left channel (" << it->second.getName() << ")" << std::endl; 
				it->second.deleteUser(userIt->second.getNickName());

			}
			if (it->second.getUsers().size() == 0)
			{
				tmpIt = it;
				++tmpIt;
				Tracer::Instance()->writeInFile("Remove channel from server");
				std::cout << "channel (" << it->second.getName() << ") has been removed from channel" << std::endl; 
		 		removeChannelMap(it->second.getName());
				it = tmpIt;
			}
			else
				++it;
		}
		
		Tracer::Instance()->writeInFile("Remove user from server");
		users.erase(findUser(fds[i].fd));
		//users.erase(fds[i].fd);
		Tracer::Instance()->writeInFile("Set fd to -1");
		fds[i].fd = -1;
		*resizeArray= true;
	}
	Tracer::Instance()->outFunc("Exited ReceiveData");
}

void	Server::ResizeArray()
{
	for (int i = 0; i < nfds; ++i)
	{
		if (fds[i].fd == -1)
		{
			for (int j = i; j < nfds - 1; ++j)
				fds[j].fd = fds[j + 1].fd;
			--i;
			--nfds;
		}
	}
}

 int Server::checkNick(std::string newNick)
 {
 	if (newNick.size() > 9)
 		return (1);
 	std::string::iterator it = newNick.begin(); 
 	while (it != newNick.end())
 	{
 		if (((*it >= 'a') && (*it <= 'z')) || ((*it >= 'A') && (*it <= 'Z')) || ((*it >= '0') && (*it <= '9')) ||
		 (*it == '{') || (*it == '}') || (*it == '|') || (*it == '^') ||
		 (*it == '[') || (*it == ']') || (*it == '\\') || (*it == '~'))
 			++it;
		else
 			return (1);
	}
 	return (0);
}

int Server::inUseNick(std::string newNick)
{
	std::map<int, User>::iterator it_end;
	it_end = this->users.end();
	for (std::map<int, User>::iterator tmp = this->users.begin(); tmp != it_end; ++tmp)
		if (tmp->second.getNickName() == newNick)
			return (1);
	return (0);
}

void Server::changeNickUser(std::string prevNickName, std::string newNickName)
{
	std::map<int, User>::iterator it_end;
	it_end = this->users.end();
	for (std::map<int, User>::iterator tmp = this->users.begin(); tmp != it_end; ++tmp)
	{
		if (tmp->second.getNickName() == prevNickName)
		{
			tmp->second.setNickName(newNickName);
			break ;
		}
	}
}


int Server::checkPassWord(std::string newPasWord, User &user)
{
	Tracer::Instance()->inFunc("Entered checkPassWord");
	if (!this->password.compare(newPasWord))
	{
		user.setStatus(1);
		Tracer::Instance()->writeInFile("Successfully connected");
		return (1);
	}
	else
		Tracer::Instance()->writeInFile("!Warning: Wrong password!");
	Tracer::Instance()->outFunc("Exited checkPassWord");
	return (1);
}

int Server::useCommand(InputMessage inputMessage, User &user)
{
	const std::string &cmd = inputMessage.getCmnd();
	if ((cmd == "PASS") || (cmd == "pass"))
		return (Commands::PASS(inputMessage, user, *this));
	else if (user.getStatus() == 1 && ((cmd == "NICK") || (cmd == "nick")))
		return (Commands::NICK(inputMessage, user, *this));
	else if (user.getStatus() == 1 && ((cmd == "USER") || (cmd == "user")))
		return (Commands::USER(inputMessage, user));
	else if ((cmd == "QUIT") || (cmd == "quit"))
		return (Commands::QUIT(inputMessage, user));
	else if (user.getStatus() == 2 && ((cmd == "PRIVMSG") || (cmd == "privmsg")))
		return (Commands::PRIVMSG(inputMessage, user, *this));
	else if ((cmd == "NOTICE") || (cmd == "notice"))
		return (Commands::NOTICE(inputMessage, user, *this));
	else if (user.getStatus() == 2 && ((cmd == "JOIN") || (cmd == "join")))
		return (Commands::JOIN(inputMessage, user, *this));
	else if (user.getStatus() == 2 && ((cmd == "PART") || (cmd == "part")))
		return (Commands::PART(inputMessage, user, *this));
	else if (user.getStatus() == 2 && ((cmd == "KICK") || (cmd == "kick")))
		return (Commands::KICK(inputMessage, user, *this));
	else
		return (1);	
}

std::map<int, User>::iterator	Server::findUser(int fd)
{
	std::map<int, User>::iterator it;
	for (it = users.begin(); it != users.end(); ++it)
		if (same_file(it->first, fd))
			return (it);
	return (it);
}


std::map<int, User> 	&Server::getUserMap()
{
	return (users);
}

std::map<std::string, Channel>	&Server::getChannelMap()
{
	return (channels);
}

std::map<std::string, Channel>::iterator	Server::addChannelMap(const std::string &name)
{
	if (channels.find(name) == channels.end())
		channels.insert(std::make_pair(name, Channel(name)));
	else
		std::cerr << "Channel is already in map" << std::endl;
	return (channels.find(name));
}

void	Server::removeChannelMap(const std::string &name)
{
	std::map<std::string, Channel>::iterator	it;
	it = channels.find(name);
	if (it != channels.end() && it->second.getUsers().empty())
		channels.erase(it);
	else
		std::cerr << "Channel is not empty" << std::endl;
}