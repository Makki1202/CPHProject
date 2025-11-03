#pragma once

const std::string FOLDERPATH = "tests/";
const bool USEFILES = false;

void solveFiles();
void solveInput();
bool runTest(const std::string &file, std::map<char, std::string> &results, std::string &retMessage);
bool readInputFromStdin(int &n, std::string &s, std::vector<std::string> &Tstrings, std::vector<std::string> &Rsets);
bool readInputFromFile(const std::string &file, int &n, std::string &s, std::vector<std::string> &Tstrings, std::vector<std::string> &Rsets);
bool convertAndCheckTstrings(std::vector<std::string> &T_line_strings, std::string &s, std::map<char, int> &repeats, std::set<char> &upperCharsUsed, std::vector<std::pair<std::string, bool>> &t_strings);
bool convertAndCheckRsets(std::vector<std::string> &R_sets_strings, std::set<char> &upperCharsUsed, std::set<char> &RsetsFound, std::string &s, std::map<char, int> &repeats, std::map<char, std::vector<std::string>> &Rsets);
bool testTstrings(std::map<char, std::string> &results, std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, std::string &s);
bool runAllCombos(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, std::map<char, std::string> &currResults, const std::string &s);
bool checkTStringAndFindRepeats(const std::string &t_string, std::map<char, int> &repeats, std::set<char> &upperCharsUsed, const std::string &s, bool &interestingLine);
std::vector<std::string> convertStringToContainer(std::string&line, const std::string &s, const std::map<char, int> &repeats);
int runRemoveElementsFromSets(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, const std::string &s, std::map<char, std::string> &results, const bool &all = false);

void replaceCapWithLower(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets);
void filterTstrings(std::vector<std::pair<std::string, bool>> &t_strings);
void printRSets(std::map<char, std::vector<std::string>> &Rsets);
void printTStrings(std::vector<std::pair<std::string, bool>> &t_strings, const bool &all = true);
bool isSingleCharString(const std::string &s);
bool addStringToTstrings(std::vector<std::pair<std::string, bool>> &t_strings, const std::string &newStr, const bool &interestingLine);
void printReslt(std::map<char, std::string> result);
void createResults(std::map<char, std::string> &results, std::map<char, std::vector<std::string>> &Rsets);
void updateRepeats(int &count, std::map<char, int> &repeats, char &current, bool &interestingLine);
bool isAllLowercase(const std::string& s);
void removeCarriage(std::string &s);
