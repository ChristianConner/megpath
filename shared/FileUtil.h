#ifndef FILEUTIL__H
#define FILEUTIL__H

#include <iostream>
#include <sstream>
#include <sys/stat.h>
using namespace std;


class FileUtil{
	public:
		static bool mkDirectory(string dir);
		static bool isDirectory(string path);
		static bool mkPath(string path);
};
#endif
