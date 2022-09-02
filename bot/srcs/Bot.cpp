#include "Bot.hpp"
#include "sstream"

Bot::Bot(std::string file)
{
	std::ifstream	ifs;
	char			buf[30];
	std::size_t		find_pos;

	ifs.open(file.c_str());
	if (!ifs.is_open())
		exit(1);
	
	ifs.getline(buf, 30);
	ip = buf;
	if ((find_pos = ip.find(" ")) == std::string::npos)
		exit(1);
	ip = ip.substr(find_pos + 1);
	ifs.getline(buf, 30);
	port = buf;
	if ((find_pos = port.find(" ")) == std::string::npos)
		exit(1);
	port = port.substr(find_pos + 1);
	ifs.getline(buf, 30);
	password = buf;
	if ((find_pos = password.find(" ")) == std::string::npos)
		exit(1);
	password = password.substr(find_pos + 1);
	ifs.getline(buf, 30);
	channel = buf;
	if ((find_pos = channel.find(" ")) == std::string::npos)
		exit(1);
	channel = channel.substr(find_pos + 1);
	nick = "infobot";

	botfd = 0;
	servfd = 0;
	bot_status = UP;
}

Bot::Bot(const Bot& obj)
{
	*this = obj;
}

Bot	&Bot::operator=(const Bot& obj)
{
	if (this == &obj)
		return (*this);
	ip = obj.ip;
	port = obj.port;
	password = obj.password;
	channel = obj.channel;
	nick = obj.nick;

	botfd = obj.botfd;
	servfd = obj.servfd;
	bot_status = obj.bot_status;
	return (*this);
}

Bot::~Bot()
{

}

int		Bot::SetupConnection()
{
	struct	sockaddr_in	addr;

	if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return (bot_status = SOCK_ERR);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(atoi(port.c_str()));

	if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0)
	{
		close(botfd);
		return (bot_status = LISTEN_ERR);
	}
	if ((botfd = connect(servfd, (struct sockaddr*)&addr, sizeof(addr))))
	{
		close(botfd);
		return (bot_status = SOCK_ERR);
	}
	return (UP);
}

int		Bot::LogIn()
{
	std::stringstream	stream;
	char				buf[MAX_BUF];
	size_t				res;

	stream << "PASS " << password << "\r\n";
	if ((res = send(servfd, stream.str().c_str(), stream.str().length(), 0)) <= 0)
			return (-1);
	stream.str("");
	stream << "NICK " << nick << "\r\n";
	if ((res = send(servfd, stream.str().c_str(), stream.str().length(), 0)) <= 0)
		return (-1);
	stream.str("");
	stream << "USER bot * * :bot\r\n";
	if ((res = send(servfd, stream.str().c_str(), stream.str().length(), 0)) <= 0)
		return (-1);
	usleep(5000);
	if ((res = recv(servfd, buf, MAX_BUF, O_NONBLOCK)) <= 0)
		return (-1);
	std::string	message(buf);
	if (message.find(" 001 ") >= message.find(nick))
		return (-1);
	stream.str("");
	stream << "JOIN " << channel << " \r\n";
	if ((res = send(servfd, stream.str().c_str(), stream.str().length(), 0)) <= 0)
		return (-1);
	return (0);
}

void		Bot::WaitAction()
{
	char	buf[MAX_BUF];
	int		res;
	std::string	message;

	while (bot_status == UP)
	{
		res = recv(servfd, buf, MAX_BUF - 1, 0);
		if (res < 0)
		{
			bot_status = RCV_ERR;
			break ; 
		}
		else if (res == 0)
			break ;
		buf[res] = 0;
		message = buf;
		std::cout << message << std::endl;
		createMsgQueue(message);
		processMessage();
	}
}

std::string		Bot::checkMessage(const std::string &message, std::string &channel)
{
	std::size_t	pos_colon;
	std::size_t	pos_grid;

	pos_colon = message.find(":");
	if ((pos_colon != std::string::npos) && ((pos_colon = message.find(":", pos_colon + 1)) != std::string::npos) &&
		(pos_colon > message.find("PRIVMSG") || pos_colon > message.find("privmsg")))
	{
		
		pos_grid = message.find("#");
		if (pos_grid == std::string::npos)
			return ("");
		channel = message.substr(pos_grid, pos_colon - pos_grid - 1);
		return (message.substr(pos_colon + 1));
	}
	return ("");
}

int		Bot::createMsgQueue(const std::string& src)
{
	size_t prev = 0;
	size_t next;
	std::string separator = "\r\n";
	size_t delta = separator.length();

	if (msgQueue.size() == 1)
	{
		std::string tmp = this->msgQueue.back();
		msgQueue.pop();
		if ((next = src.find(separator, prev)) != std::string::npos)
		{
			msgQueue.push(std::string(tmp + src.substr(prev, (next- prev))));
			prev = next + delta;
		}
		else
		{
			msgQueue.push(std::string(tmp + src));
			return (0);
		}
	}
	while((next = src.find(separator, prev)) != std::string::npos)
	{
		msgQueue.push(src.substr(prev, (next - prev)));
		prev = next + delta;
	}
	std::string last_elem = src.substr(prev);
	if (last_elem.length())
		msgQueue.push(last_elem);
	return (0);
}

void	Bot::processMessage()
{
	std::string command;
	std::string channel;
	std::string *strFromQueue;

	while (msgQueue.size())
	{
		strFromQueue = &msgQueue.front();
		if (strFromQueue->find("/r/n", 0) != std::string::npos)
			break;
		command = checkMessage(*strFromQueue, channel);
		if (command == "")
		{
			msgQueue.pop();
			continue;
		}
		processCommand(command, channel);
		msgQueue.pop();
	}
}

void	Bot::processCommand(const std::string& cmd, const std::string& channel)
{
	std::size_t	pos;

	if ((pos = cmd.find(" is ")) != std::string::npos && pos != 0)
		setFactoid(cmd, pos, channel);
	else if ((pos = cmd.find("?")) != std::string::npos && pos != 0)
		accessFactoid(cmd, pos, channel);
	else if ((pos = cmd.find("infobot, forget ")) != std::string::npos && pos == 0)
		forgetFactoid(cmd, channel);
	else if ((pos = cmd.find("infobot, join ")) != std::string::npos && pos == 0)
		joinChannel(cmd);
	else if ((pos = cmd.find("infobot, part ")) != std::string::npos && pos == 0)
		partChannel(cmd);
}

void	Bot::setFactoid(const std::string& cmd, std::size_t pos, const std::string &channel)
{
	std::stringstream	stream;
	std::string factoid_name = cmd.substr(0, pos);
	std::string	factoid_value = cmd.substr(pos + std::string(" is ").length());
	
	factoid_value = factoid_value.substr(0, factoid_value.find(" "));
	if (factoid_name.find("no, infobot, ") != std::string::npos)
	{
		factoid_name = factoid_name.substr(std::string("no, infobot, ").length());
		if (factoid_name.find(" ") != std::string::npos)
			return ;
		factoids[channel][factoid_name] = factoid_value;
	}
	else if (cmd.find(" ") == pos)
		factoids[channel][factoid_name] = factoid_value;
	else
		return ;
	{
		stream << "PRIVMSG " << channel << " :" << factoid_name << " has been added\r\n";
		send(servfd, stream.str().c_str(), stream.str().length(), 0);
	}
}

void	Bot::accessFactoid(const std::string& cmd, std::size_t pos, const std::string &channel)
{
	std::stringstream	stream;
	std::string			factoid;
	std::size_t			end_pos;
	std::map<std::string, std::map<std::string, std::string> >::iterator chanIt;
	std::map<std::string, std::string>::iterator factIt;

	if ((end_pos = cmd.find_last_of(" ", pos)) == std::string::npos)
		factoid = cmd.substr(0);
	else
		factoid = cmd.substr(end_pos + 1);
	factoid = cmd.substr(end_pos + 1);
	factoid = factoid.substr(0, factoid.length() - 1);
	if (cmd.find(factoid) != 0)
		return ;
	if ((chanIt = factoids.find(channel)) != factoids.end() && 
		(factIt = chanIt->second.find(factoid)) != chanIt->second.end())
	{
		stream << "PRIVMSG " << channel << " :somebody said " << factIt->first << " is " << factIt->second << "\r\n";
	}
	else
		stream << "PRIVMSG " << channel << " :i don't what is " << factoid << "\r\n";
	send(servfd, stream.str().c_str(), stream.str().length(), 0);
}

void	Bot::forgetFactoid(const std::string& cmd, const std::string &channel)
{
	std::stringstream	stream;
	std::map<std::string, std::map<std::string, std::string> >::iterator chanIt;

	std::size_t	start_pos = std::string("infobot, forget ").length();
	std::string	factoid;
	factoid = cmd.substr(start_pos, cmd.find(" ", start_pos));
	if ((chanIt = factoids.find(channel)) != factoids.end() && chanIt->second.erase(factoid))
	{
		stream << "PRIVMSG " << channel << " :factoid (" << factoid << ") succesfully erased \r\n";
		send(servfd, stream.str().c_str(), stream.str().length(), 0);
	}
}

void	Bot::joinChannel(const std::string &cmd)
{
	std::stringstream	stream;
	size_t				pos;
	std::string			tmpChan;

	stream.str("");
	if ((pos = cmd.find("#")) == std::string::npos && pos + 1 == cmd.length())
		return ;
	tmpChan = cmd.substr(pos, cmd.find(" "));
	stream << "JOIN " << tmpChan << " \r\n";
	if (send(servfd, stream.str().c_str(), stream.str().length(), 0) > 0)
		factoids.insert(std::make_pair(tmpChan, std::map<std::string, std::string>()));
}

void	Bot::partChannel(const std::string &cmd)
{
	std::stringstream	stream;
	size_t				pos;
	std::string			tmpChan;

	stream.str("");
	if ((pos = cmd.find("#")) == std::string::npos && pos + 1 == cmd.length())
		return ;
	std::cout << cmd << std::endl;
	tmpChan = cmd.substr(pos, cmd.find(pos, ' '));
	std::cout << tmpChan << std::endl;
	stream << "PART " << tmpChan << "\r\n";
	if (send(servfd, stream.str().c_str(), stream.str().size(), 0) > 0)
		factoids.erase(tmpChan);
}