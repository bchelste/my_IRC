#include "Bot.hpp"
#include "string.h"
#include "stdlib.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Wrong number of arguments" << std::endl;
        return (1);
    }
    Bot bot = Bot(argv[1]);
    std::cout << "Bot created" << std::endl;
    if (bot.SetupConnection())
        return(1);
    std::cout << "Connection setup" << std::endl;    
    if (bot.LogIn())
        return (1);
    std::cout << "Login succesful" << std::endl;    
    bot.WaitAction();
    return(0);
}