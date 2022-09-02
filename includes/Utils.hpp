#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sys/stat.h>

typedef unsigned int uint;

int		ft_find_duplicate(std::vector<std::string> targets, std::string victim);
bool    same_file(int fd1, int fd2);

struct file_desc
{
    file_desc(int fd) : desc(fd) {}

    operator int() {return desc;}

    bool operator==(const file_desc &other) const {return desc == other.desc;}
    bool operator!=(const file_desc &other) const {return desc == other.desc;}

    int desc;
};
