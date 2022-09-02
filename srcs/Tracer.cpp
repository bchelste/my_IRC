/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tracer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 18:40:02 by lbenedar          #+#    #+#             */
/*   Updated: 2022/05/28 21:34:38 by lbenedar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tracer.hpp"

Tracer* Tracer::_self = 0;
void*   Tracer::subFlushStream(void*)
{
    Instance()->flushStream();
    return (0);
}

Tracer::Tracer()
{
    std::string     log_name; 
    //pthread_t       flush_thread;   
    unsigned int    i = 0;

    log_name = std::string("logs/") + getCustomCurrentTime() + ".log";
    while (i != TR_OPEN_TRIES)
    {
        fs.open(log_name.c_str(), std::fstream::out | std::fstream::app);
        if (fs.is_open())
            break ;
        std::cout << "Couldn't open file. Try - " << i++ << std::endl;
        usleep(1000000);
    }
    c_start = clock();
    //pthread_mutex_init(&fmutex, 0);
    //pthread_create(&flush_thread, 0, subFlushStream, 0);
    //pthread_detach(flush_thread);
}

Tracer::~Tracer()
{
    //pthread_mutex_destroy(&fmutex);
    if (fs.is_open())
        fs.close();
}

Tracer  *Tracer::Instance()
{
    if (!_self)
        _self = new Tracer();
    return (_self);
}

void    Tracer::inFunc(const std::string& msg)
{
    writeInFile(msg);
    lvl += 2;
}

void    Tracer::outFunc(const std::string& msg)
{
    lvl -= 2;
    writeInFile(msg);
}

void    Tracer::writeInFile(const std::string& msg)
{
    //pthread_mutex_lock(&fmutex);
    fs << "[" << std::setw(12) << std::right << (double)(clock() - c_start) << "]";
    fs << " | " << std::string(lvl, ' ') << msg << std::endl;
    //pthread_mutex_unlock(&fmutex);
}

void    Tracer::flushStream()
{
    usleep(1000000);
    // could be deleted. dont know what happened if value will be deleted,
    //                   but in same time _self wont be 0
    while (_self) 
    {
        pthread_mutex_lock(&fmutex);
        fs.flush();
        pthread_mutex_unlock(&fmutex);
        usleep(1500000);
    }
}

bool    Tracer::DeleteInstance()
{
    if (_self)
    {
        delete (_self);
        _self = 0;
        return (true);
    }
    return (false);
}

std::fstream&   Tracer::getFileStream()
{
    return (fs);
}

std::clock_t&   Tracer::getTimerStart()
{
    return (c_start);
}

std::string     Tracer::getCustomCurrentTime()
{
    time_t              rawTime;
    struct tm           *currTime;
    std::stringstream   string_time;

    std::time(&rawTime);
    currTime = std::gmtime(&rawTime);
    string_time <<  currTime->tm_mday << "." <<
                    currTime->tm_mon + 1 << "." <<
                    currTime->tm_year + 1900 << "_" <<
                    currTime->tm_hour << ":" << 
                    currTime->tm_min << ":" <<
                    currTime->tm_sec;                    
    return (string_time.str());
}

int             Tracer::getLVL()
{
    return (lvl);
}