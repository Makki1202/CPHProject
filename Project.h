#pragma once
bool runTest(const std::string &file, std::string &retMessage);
void printRSets(std::map<char, std::vector<std::string>> &Rsets);
void printTStrings(std::vector<std::pair<std::string, bool>> &t_strings, const bool &all = true);
void printReslt(std::map<char, std::string> result);