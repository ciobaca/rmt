#define LOG_CPP__
#include "log.h"
#include <iostream>
#include <string>
#include <cassert>

int debug_level;
bool debug_sat;

using namespace std;

std::string levelToString(const int level)
{
  if (level == ERROR) {
    return "ERR ";
  } else if (level == WARNING) {
    return "WRN ";
  } else if (level == INFO) {
    return "INF ";
  } else if (level == DEBUG3) {
    return "DB3 ";
  } else if (level == DEBUG4) {
    return "DB4 ";
  } else if (level == DEBUG5) {
    return "DB5 ";
  } else if (level == DEBUG6) {
    return "DB6 ";
  } else if (level == DEBUG7) {
    return "DB7 ";
  } else if (level == DEBUG8) {
    return "DB8 ";
  } else if (level == DEBUG9) {
    return "DB9 ";
  } else {
    return "OTH ";
  }
}
