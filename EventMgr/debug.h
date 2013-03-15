#ifndef _DEBUG_HEADER_H
#define _DEBUG_HEADER_H

#ifdef DEBUG
#include <fstream>

#define PRINT_DEBUG(s) std::ofstream file("log.txt", std::ios_base::app);\
file << "[" << __DATE__ << ", " << __TIME__ << "] " << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__;\
if(s) file << " **" << s;\
file << std::endl;\
file.close()

#else
#define PRINT_DEBUG(s)
#endif

#endif