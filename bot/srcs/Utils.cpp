#include "Utils.hpp"

int		ft_find_duplicate(std::vector<std::string> targets, std::string victim)
{
	int res = 0;
	for (size_t i = 0; i < targets.size(); ++i)
		if (targets[i] == victim)
			++res;
	return (res);
}

bool same_file(int fd1, int fd2) 
{
    struct stat stat1, stat2;
    if(fstat(fd1, &stat1) < 0)
		return (0);
    if(fstat(fd2, &stat2) < 0)
		return (0);
    return ((stat1.st_dev == stat2.st_dev) && (stat1.st_ino == stat2.st_ino));
}
