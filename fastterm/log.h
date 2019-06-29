#ifndef LOG_H__
#define LOG_H__

#include <iostream>
#include <fstream>
#include <string>

#ifdef LOG_CPP__
#else
extern int debug_level;
extern bool debug_sat;
#endif

const int ERROR = 0;
const int WARNING = 1;
const int INFO = 2;
const int DEBUG3 = 3;
const int DEBUG4 = 4;
const int DEBUG5 = 5;
const int DEBUG6 = 6;
const int DEBUG7 = 7;
const int DEBUG8 = 8;
const int DEBUG9 = 9;

std::string levelToString(const int);

#ifndef NDEBUG

#define LOG(level, params) do { if (level <= debug_level) { std::cerr << levelToString(level); params; std::cerr << std::endl; } } while (0)

#else

#define LOG(level, params)

#endif

#define LOGSAT(params) do { if (debug_sat) { std::cerr << "SAT "; params; std::cerr << std::endl; } } while (0)

#endif
