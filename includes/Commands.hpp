#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "InputMessage.hpp"
# include "User.hpp"
# include "Server.hpp"
# include "Replies.hpp"

class InputMessage;
class User;
class Server;

class Commands
{
	public:
		
		Commands();
		virtual ~Commands();

		//static int ChooseCommand(InputMessage inputMessage, User &user, Server &serv);

		static int automaticReplies(User &user, std::string reply);

		static int PASS(InputMessage &inputMessage, User &user, Server &serv);
		static int NICK(InputMessage &inputMessage, User &user, Server &serv);
		static int USER(InputMessage &inputMessage, User &user);
		static int QUIT(InputMessage &inputMessage, User &user);

		static int PRIVMSG(InputMessage &inputMessage, User &user, Server &serv);
		static int NOTICE(InputMessage &inputMessage, User &user, Server &serv);

		static int JOIN(InputMessage &inputMessage, User &user, Server &serv);
		static int PART(InputMessage &inputMessage, User &user, Server &serv);
		static int KICK(InputMessage &inputMessage, User &user, Server &serv);
		
};

#endif 