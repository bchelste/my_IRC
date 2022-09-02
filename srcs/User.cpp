#include "User.hpp"

User::User(int newFd)
{
	this->fd = dup(newFd);
	this->status = 0;
//	this->mode = 'w'; 						// user receives wallops
	this->userName = "default name";
	this->nickName = "default nickname";
	this->realName = "default realname";
	this->passWord = "";

	struct sockaddr_in	addr;
	long len = sizeof(addr);
	if (!getsockname(newFd, (struct sockaddr *)&addr, (socklen_t*)&len))
		hostname = inet_ntoa(addr.sin_addr);
	else
		std::cerr << "Could not get address" << std::endl;	
}

User::User(const User &obj)
{
	*this = obj;
}

User::~User()
{
	// int ret = close(this->fd); 	
	// if (ret == -1)
	// 	errno = EBADF;
	if (status == 2 || status == -2)
	{
		Tracer::Instance()->writeInFile("user (" + nickName + ") left server");
		std::cout << "user (" << nickName << ") left server" << std::endl;
	}
	// for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end();)
	// {
	// 	++it;
	// 	(*(it - 1))->deleteUser(nickName);
	// }
}

User	&User::operator=(const User& obj)
{
	if (&obj == this)
		return (*this);
	fd = dup(obj.fd);
	status = obj.status;
	// mode = obj.mode;
	nickName = obj.nickName;
	passWord = obj.passWord;
	realName = obj.realName;
	userName = obj.userName;
	cmdQueue = obj.cmdQueue;
	hostname = obj.hostname;
	channels = obj.channels;
	return (*this);
}

// setters:

void User::setStatus(int newStatus)
{
	this->status = newStatus;
}
/*
void User::setMode(char c)
{
	this->mode = c;
}
*/
void User::setNickName(std::string newNickName)
{
	this->nickName = newNickName;
}

void User::setPassword(std::string newPassWord)
{
	this->passWord = newPassWord;
}

void User::setRealName(std::string newRealName)
{
	this->realName = newRealName;
}

void User::setUserName(std::string newUserName)
{
	this->userName = newUserName;
}

int User::getFd()
{
	return (fd);
}

int User::getStatus() const
{
	return (this->status);
}
/*
char User::getMode() const
{
	return (this->mode);
}
*/
const std::string &User::getNickName() const
{
	return (this->nickName);
}

const std::string &User::getRealName() const
{
	return (this->realName);
}

const std::string &User::getHostName() const
{
	return (hostname);
}

const std::string &User::getUserName() const
{
	return (this->userName);
}

int User::createCmdQueue(const std::string& src)
{
	size_t prev = 0;
	size_t next;
	std::string separator = "\r\n";
	size_t delta = separator.length();

	Tracer::Instance()->inFunc("Entered in createCmdQueue");

	//std::cout << "string come to User: " << src << "||" << std::endl;
	//std::cout << "separator: " << separator << std::endl;

	if (this->cmdQueue.size() == 1)
	{
		Tracer::Instance()->writeInFile("added command to previous cmd");
		std::string tmp = this->cmdQueue.back();
		Tracer::Instance()->writeInFile("Previous command is " + tmp);
		cmdQueue.pop();
		if ((next = src.find(separator, prev)) != std::string::npos)
		{
			cmdQueue.push(std::string(tmp + src.substr(prev, (next- prev) + delta)));
			prev = next + delta;
		}
		else
		{
			cmdQueue.push(std::string(tmp + src));
			return (0);
		}
	}

	Tracer::Instance()->writeInFile("added commands to previous cmd");
	while((next = src.find(separator, prev)) != std::string::npos)
	{
		this->cmdQueue.push(src.substr(prev, (next - prev) + delta));
		prev = next + delta;
	}
	std::string last_elem = src.substr(prev);
	if (last_elem.length())
		this->cmdQueue.push(last_elem);
	Tracer::Instance()->outFunc("Exited in createCmdQueue");

	return (0);
}

int	User::sendMessage(const std::string& msg)
{
	int res;

	res = send(fd, msg.c_str(), msg.size(), 0);
	if (res == -1)
		std::cout << strerror(errno) << std::endl;
	return (res);
}

void	User::deleteChannel(Channel* channel)
{
	for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); ++it)
		if (*it == channel)
		{
			channels.erase(it);
			break ;
		}

}

std::vector<Channel *> &User::getChannels()
{
	return (this->channels);
}

void 	User::addChannel(Channel *newChan)
{
	this->channels.push_back(newChan);
}
