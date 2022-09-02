#include "Server.hpp"
#include "string.h"
#include "stdlib.h"

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        std::cerr << "Wrong number of arguments" << std::endl;
        return (1);
    }
    Tracer::Instance();
    Server serv = Server(atoi(argv[1]), argv[2]);
    if (serv.SetupListen())
    {
        Tracer::Instance()->writeInFile(std::string("Error: ") + strerror(errno));
        return(1);
    }
    serv.WaitAction();
    Tracer::DeleteInstance();
    usleep(1000000);
    return(0);
}