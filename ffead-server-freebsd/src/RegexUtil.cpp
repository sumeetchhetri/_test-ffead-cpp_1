/*
 * RegexUtil.cpp
 *
 *  Created on: 14-Aug-2012
 *      Author: sumeetc
 */

#include "RegexUtil.h"

map<string, regex_t> RegexUtil::patterns;

RegexUtil::RegexUtil() {
	// TODO Auto-generated constructor stub

}

RegexUtil::~RegexUtil() {
	// TODO Auto-generated destructor stub
}

vector<string> RegexUtil::search(const string& text, const string& pattern) {
	vector<string> vec;
	string ttext(text);
	regex_t regex;
	int reti;
	if(patterns.find(pattern)!=patterns.end())
	{
		regex = patterns[pattern];
	}
	else
	{
		/* Compile regular expression */
		reti = regcomp(&regex, pattern.c_str(), REG_EXTENDED);
		if(reti)
		{
			cout << "Could not compile regex\n" << endl;
		}
		else
		{
			patterns[pattern] = regex;
		}
	}
	regmatch_t pm;
	reti = regexec(&regex, ttext.c_str(), 1, &pm, 0);
	while (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		if(!reti) {
			string match;
			match = ttext.substr(pm.rm_so, pm.rm_eo-pm.rm_so);
			vec.push_back(match);
		} else {
			break;
		}
		ttext = ttext.substr(pm.rm_eo);
		pm.rm_eo = -1;
		pm.rm_so = -1;
		reti = regexec (&regex, ttext.c_str(), 1, &pm, 0);
	}
	return vec;
}

string RegexUtil::replace(const string& text, const string& pattern, const string& with) {
	string ttext(text);
	string rettxt;
	regex_t regex;
	int reti;
	if(patterns.find(pattern)!=patterns.end())
	{
		regex = patterns[pattern];
	}
	else
	{
		/* Compile regular expression */
		reti = regcomp(&regex, pattern.c_str(), REG_EXTENDED);
		if(reti)
		{
			cout << "Could not compile regex\n" << endl;
		}
		else
		{
			patterns[pattern] = regex;
		}
	}
	regmatch_t pm;
	reti = regexec(&regex, ttext.c_str(), 1, &pm, 0);
	while (reti == 0) {    /* while matches found */
		/* substring found between pm.rm_so and pm.rm_eo */
		/* This call to regexec() finds the next match */
		if(!reti) {
			string match;
			match = ttext.substr(pm.rm_so, pm.rm_eo-pm.rm_so);
			rettxt += ttext.substr(0, pm.rm_so) + with;
		} else {
			rettxt += ttext;
			break;
		}
		ttext = ttext.substr(pm.rm_eo);
		pm.rm_eo = -1;
		pm.rm_so = -1;
		reti = regexec (&regex, ttext.c_str(), 1, &pm, 0);
	}
	if(ttext!="")rettxt += ttext;
	return rettxt;
}
