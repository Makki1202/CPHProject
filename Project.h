#pragma once

const std::string FOLDERPATH = "tests/";
const std::string TESTPREFIX = "test0";
const std::string FILEEXTENSION = ".swe";
const int FILEAMOUNT = 7;
const bool USEFILES = true;

bool runTest(const std::string &file, std::map<char, std::string> &results, std::string &retMessage);
bool testTstrings(std::map<char, std::string> &results, std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, std::string &s);
bool convertAndCheckRsets(std::vector<std::string> &R_sets_strings, std::set<char> &upperCharsUsed, std::set<char> &RsetsFound, std::string &s, std::map<char, int> &repeats, std::map<char, std::vector<std::string>> &Rsets);
bool convertAndCheckTstrings(std::vector<std::string> &T_line_strings, std::string &s, std::map<char, int> &repeats, std::set<char> &upperCharsUsed, std::vector<std::pair<std::string, bool>> &t_strings);
void solveFiles();
void solveInput();
void replaceCapWithLower(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets);
void printRSets(std::map<char, std::vector<std::string>> &Rsets);
void printTStrings(std::vector<std::pair<std::string, bool>> &t_strings, const bool &all = true);
void printReslt(std::map<char, std::string> result);
void createResults(std::map<char, std::string> &results, std::map<char, std::vector<std::string>> &Rsets);
void updateRepeats(int &count, std::map<char, int> &repeats, char &current, bool &interestingLine);
