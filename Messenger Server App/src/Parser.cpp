#include "Parser.h"

#ifndef PARSER
#define PARSER

using namespace std;

template<typename Out>
void split(const string &s, char delim, Out result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		*(result++) = item;
	}
}

vector<string> split(const string &s, char delim) {
	vector<string> elems;
	split(s, delim, back_inserter(elems));
	return elems;
}

std::string numberToString ( int number )
{
 std::ostringstream ss;
 ss << number;
 return ss.str();
}


#endif /* PARSER */
