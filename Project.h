#pragma once
bool runTest(const std::string &file, std::map<char, std::string> &results, std::string &retMessage);
void solveFiles();
void solveInput();
void replaceCapWithLower(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets);
void printRSets(std::map<char, std::vector<std::string>> &Rsets);
void printTStrings(std::vector<std::pair<std::string, bool>> &t_strings, const bool &all = true);
void printReslt(std::map<char, std::string> result);