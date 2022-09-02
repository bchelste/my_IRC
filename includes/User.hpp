#ifndef USER_HPP
# define USER_HPP

# include <string>
# include <map>
# include <queue>
# include <fcntl.h>
# include <unistd.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <netdb.h>

# include "Server.hpp"
# include "Channel.hpp"

class Server;
class Channel;

class User
{
	friend class Server;
	
	private:
		
		int					fd;
		int					status;
	//	char				mode;
		
		std::string			nickName;
		std::string			passWord;
		std::string			realName;
		std::string			hostname;
		std::string			userName;

		std::queue<std::string> cmdQueue;
		std::vector<Channel *>	channels;

	public:

		User(int fd);
		User(const User&);
		~User();

		User	&operator=(const User&);

		void setStatus(int newStatus);
	//	void setMode(char c);
		
		void setNickName(std::string newNickName);
		void setPassword(std::string newPassWord);
		void setRealName(std::string newRealName);
		void setUserName(std::string newUserName);

		int		getFd();
		int		getStatus() const;
	//	char	getMode() const;
		
		const std::string &getNickName() const;
		const std::string &getRealName() const;
		const std::string &getHostName() const;
		const std::string &getUserName() const;

		int	createCmdQueue(const std::string& src);

		int sendMessage(const std::string& msg);

		void	deleteChannel(Channel*);
		std::vector<Channel *> &getChannels();

		void 	addChannel(Channel *newChan);

};

#endif