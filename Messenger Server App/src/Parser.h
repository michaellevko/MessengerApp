/*
 * Parser.h
 *
 *  Created on: Mar 9, 2017
 *      Author: user
 */
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

#ifndef SRC_PARSER_H_
#define SRC_PARSER_H_

using namespace std;

template<typename Out>
void split(const string &s, char delim, Out result);
vector<string> split(const string &s, char delim);

#endif /* SRC_PARSER_H_ */
