/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tracer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lbenedar <lbenedar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 18:40:25 by lbenedar          #+#    #+#             */
/*   Updated: 2022/05/25 20:08:24 by lbenedar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRACER_HPP
# define TRACER_HPP

# include "fstream"
# include "ctime"
# include "unistd.h"
# include "iostream"
# include "pthread.h"
# include "sstream"
# include "iomanip"

# define TR_OPEN_TRIES 10

class Tracer
{
public:
    static Tracer*  Instance();
    static bool     DeleteInstance();
    static void*    subFlushStream(void*);

    void            inFunc(const std::string&);
    void            outFunc(const std::string&);
    void            writeInFile(const std::string&);
    void            flushStream();

    std::fstream&   getFileStream();
    std::clock_t&   getTimerStart();
    std::string     getCustomCurrentTime();
    int             getLVL();
    
private:
    static Tracer*  _self;
    Tracer();
    virtual ~Tracer();

    std::fstream    fs;
    pthread_mutex_t fmutex;
    std::clock_t    c_start;
    int             lvl;
};

#endif