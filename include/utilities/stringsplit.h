#ifndef STRINGSPLIT_H_
#define STRINGSPLIT_H_

#include "Global.h"

typedef std::vector<std::string> VecStr;

void StringSplit(const std::string& line, VecStr& container);
void StringSplit(const std::string& line, VecStr& container, const std::string& anyChar);

#endif
