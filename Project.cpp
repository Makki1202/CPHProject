#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <algorithm>
#include <filesystem> // C++17 feature
#include "Project.h"

int main() {
    if (USEFILES)
    {
        solveFiles();
    }
    else
    {
        solveInput();
    }
}

void solveInput()
{
    std::string retMessage = "";
    std::map<char, std::string> results;
    bool retVal = runTest("", results, retMessage);

    if (retVal)
    {
        if (!results.empty())
        {
            // std::cout << "Results:\n";
            printReslt(results);
        }
    }
    else
    {
        std::cout << retMessage << std::endl;
    }
}

void solveFiles()
{   
    std::string path = FOLDERPATH; // Current directory
    auto programStart = std::chrono::high_resolution_clock::now();

    for (const auto &entry:std::filesystem::directory_iterator(path))
    {
        auto fileStart = std::chrono::high_resolution_clock::now();

        std::string retMessage = "";
        std::string file = entry.path().filename().string();
        std::map<char, std::string> results;
        bool retVal = runTest(file, results, retMessage);

        std::cout << file << ": " << retMessage << std::endl;
        if (retVal)
        {
            if (!results.empty())
            {
                std::cout << file << " results:\n";
                printReslt(results);
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        // Calculate elapsed time in microseconds
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - fileStart);

        std::cout << file << " execution time: " << duration.count() << " us" << std::endl;
    }

    auto end = std::chrono::high_resolution_clock::now();
    // Calculate elapsed time in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - programStart);

    std::cout << "Total execution time: " << duration.count() << " us" << std::endl;
}


bool runTest(const std::string &file, std::map<char, std::string> &results, std::string &retMessage)
{

    int n;
    std::string s;
    std::vector<std::string> T_line_strings;
    std::vector<std::string> R_sets_strings;
    bool retVal = true;
    if (file.empty())
    {
        retVal = readInputFromStdin(n, s, T_line_strings, R_sets_strings);
    }
    else
    {
        retVal = readInputFromFile(file, n, s, T_line_strings, R_sets_strings);
    }

    if (!retVal)
    {
        retMessage = "NO";
        return false;
    }
 
    if (s.empty() || !isAllLowercase(s))
    {
        // Format, string must be only lower case letters
        retMessage = "NO";
        return false;
    }
    std::map<char, int> repeats; // Map for repetitions of uppercase letters
    std::set<char> upperCharsUsed; // Set for seeing what R sets we need
    std::vector<std::pair<std::string, bool>> t_strings; // t strings, bool is to indicate if it is interesting

    retVal = convertAndCheckTstrings(T_line_strings, s, repeats, upperCharsUsed, t_strings);
    if (!retVal)
    {
        retMessage = "NO";
        return false;
    }

    std::map<char, std::vector<std::string>> Rsets;
    std::set<char> RsetsFound; // Set to make sure we have a Rset for every upper char

    retVal = convertAndCheckRsets(R_sets_strings, upperCharsUsed, RsetsFound, s, repeats, Rsets);
    if (!retVal)
    {
        retMessage = "NO";
        return false;
    }

    filterTstrings(t_strings);
    if (t_strings.empty())
    {
        createResults(results, Rsets);
        retMessage = "YES";
        return true;
    }

    retVal = testTstrings(results, t_strings, Rsets, s);
    if (!retVal)
    {
        retMessage = "NO";
        return false;
    }

    retVal = runAllCombos(t_strings, Rsets, results, s);

    if (!retVal)
    {
        retMessage = "NO";
        return false;
    }
    else
    {
        retMessage = "YES";
        return true;
    }
}

bool convertAndCheckTstrings(std::vector<std::string> &T_line_strings, std::string &s, std::map<char, int> &repeats, std::set<char> &upperCharsUsed, std::vector<std::pair<std::string, bool>> &t_strings)
{
    bool retFlag = true;
    for (auto &&line : T_line_strings)
    {
        if (line.size() > s.size())
        {
            retFlag = false;
            break;
        }
        bool interestingLine = false;
        if (checkTStringAndFindRepeats(line, repeats, upperCharsUsed, s, interestingLine))
        {
            addStringToTstrings(t_strings, line, interestingLine);
        }
        else
        {
            // Bad format, or a lowercase letter from the t_string is not in string s
            retFlag = false;
            break;
        }
    }
    return retFlag;
}

bool convertAndCheckRsets(std::vector<std::string> &R_sets_strings, std::set<char> &upperCharsUsed, std::set<char> &RsetsFound, std::string &s, std::map<char, int> &repeats, std::map<char, std::vector<std::string>> &Rsets)
{
    for (auto &&line : R_sets_strings)
    {
        // TODO: Maybe check that line is correct format
        // If a set is not used discard it
        if (!std::isupper(line[0]))
        {
            // Format, R sets must begin with a capital letter
            return false;
        }
        if (upperCharsUsed.find(line[0]) == upperCharsUsed.end())
        {
            continue;
        }
        RsetsFound.insert(line[0]);
        std::vector<std::string> strings = convertStringToContainer(line, s, repeats);

        // If vector is empty, then there is no solution.
        if (strings.empty())
        {
            return false;
        }

        // Save the R set
        Rsets[line[0]] = strings;
    }

    if (RsetsFound != upperCharsUsed)
    {
        // Format, we're missing a set
        return false;
    }
    return true;
}

bool testTstrings(std::map<char, std::string> &results, std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, std::string &s)
{
    size_t prevSize = results.size();
    size_t newSize = 0;
    int outcome = 0;
    bool tryAll = false;

    while (true)
    {
        outcome = runRemoveElementsFromSets(t_strings, Rsets, s, results, tryAll);
        if (outcome == 1)
        {
            return false;
        }

        // If no changes were made
        if (outcome == 0)
        {
            if (!tryAll)
            {
                tryAll = true; // try once more with tryAll = true
            }
            else
            {
                break; // already tried with tryAll, done
            }
        }
    }

    return true;
}

bool checkTStringAndFindRepeats(const std::string &t_string, std::map<char, int> &repeats, std::set<char> &upperCharsUsed, const std::string &s, bool &interestingLine)
{
    // Check if string is a possible substring based on lowercase letters and
    // check repetitions of upper case letters and
    // store what upper case letters are used.
    bool valid = true;
    char current = '\n';
    int count = 1;
    std::string lowercaseString = "";

    for (size_t i = 0; i < t_string.size(); ++i) {
        if (std::isupper(t_string[i]))
        {
            upperCharsUsed.insert(t_string[i]);
            lowercaseString.clear();
        }
        else if (std::islower(t_string[i]))
        {
            lowercaseString += t_string[i];
            interestingLine = true;
            if (s.find(lowercaseString) == std::string::npos)
            {
                // Format, catches characters that are not upper or lower case letters
                // Catches any lower case letter not in string s
                valid = false;
                break;
            }
        }
        else
        {
            // Format, catches characters that are not upper or lower case letters
            // Catches any lower case letter not in string s
            valid = false;
            break;
        }
        
        if (t_string[i] == current) {
            count++;
        } else {
            // handle repeated characters
            updateRepeats(count, repeats, current, interestingLine);
            current = t_string[i];
            count = 1;
        }
    }
    // handle last sequence of repeated characters
    updateRepeats(count, repeats, current, interestingLine);

    return valid;
}

std::vector<std::string> convertStringToContainer(std::string&line, const std::string &s, const std::map<char, int> &repeats)
{
    // Convert the R set string to a vector while getting rid of elements
    // that are not in string s or repetitions of that element that are not in s
    std::vector<std::string> values;
    bool repeat  = repeats.find(line[0]) != repeats.end();
    // Find the part after ':'
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        std::string items = line.substr(colonPos + 1);

        std::stringstream ss(items);
        std::string token;

        while (std::getline(ss, token, ',')) {
            if (!token.empty())
            {
                if (s.find(token) == std::string::npos)
                {
                    continue;
                }
                bool tokenPossible = true;

                if (repeat)
                {
                    std::string repeatedString;
                    for (int i = 0; i < repeats.at(line[0]); i++)
                    {
                        repeatedString += token;
                    }
                    if (s.find(repeatedString) == std::string::npos)
                    {
                        tokenPossible = false;
                    }
                }
                if (tokenPossible)
                {
                    values.push_back(token);
                }

            }

        }
    }
    else
    {
        // Format, line must have ":"
        return {};
    }

    return values;
}

int runRemoveElementsFromSets(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, const std::string &s, std::map<char, std::string> &results, const bool &all)
{
    int retVal = 0;
    for (const auto& pattern : t_strings)
    {
        if (!pattern.second && !all)
        {
            continue;
        }
        // Step 1: collect unique uppercase letters (order matters)
        std::map<char, std::set<std::string>> usedElements;
        std::map<char, std::string> currentElements;
        std::vector<char> variables;
        std::set<char> seen;
        for (char c : pattern.first)
        {
            if (std::isupper(c) && seen.insert(c).second)
            {
                variables.push_back(c);
                usedElements[c] = {};
            }
        }

        // Step 2: initialize index vector
        std::vector<size_t> indices(variables.size(), 0);

        bool done = variables.empty();
        char latestUpper;

        // Step 3: generate all combinations
        while (!done)
        {
            for (int i = 0; i < variables.size(); i++)
            {
                currentElements[variables[i]] = Rsets.at(variables[i])[indices[i]];
            }
            // Build one expanded string
            std::string result;
            bool earlybreak = false;
            seen.clear();
            for (char c : pattern.first)
            {
                if (std::islower(c))
                {
                    result += c;
                }
                else
                {
                    result += currentElements[c];
                    if (seen.insert(c).second)
                    {
                        latestUpper = c;
                    }
                }
                // Check if result is substring of s
                if (s.find(result) == std::string::npos)
                {
                    earlybreak = true;
                    break;
                }
            }

            // Check if result is substring of s
            if (!earlybreak && s.find(result) != std::string::npos)
            {
                // std::cout << "success with ";
                for (auto &&[k, v] : currentElements)
                {
                    // std::cout << k << ": " << v << " ";
                    usedElements[k].insert(v);
                }
                // std::cout << std::endl;
            }

            // Find index where there was a problem
            int varIdx = (int)std::distance(
                        variables.begin(),
                        std::find(variables.begin(), variables.end(), latestUpper));
            

            // Increment indices like a counter
            for (int i = (int)indices.size() - 1; i >= 0; --i)
            {
                // Set all indices to 0 until we get to desired index to increment
                if (i > varIdx)
                {
                    indices[i] = 0;
                    continue;
                }
                indices[i]++;
                if (indices[i] < Rsets.at(variables[i]).size())
                    break; // still valid combination
                indices[i] = 0;
                if (i == 0)
                    done = true; // overflow
            }
        }

        for (auto &&[key, list] : usedElements)
        {
            if (list.size() != Rsets[key].size())
            {
                Rsets[key] = std::vector(list.begin(), list.end());
                retVal = 2;
            }
            if (list.size() == 1)
            {
                results[key] = Rsets[key][0];
                replaceCapWithLower(t_strings, Rsets);
            }
            if (list.empty())
            {
                return 1;
            }
        }
    }

    return retVal;
}

bool runAllCombos(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, std::map<char, std::string> &currResults, const std::string &s)
{
    // Step 1: collect unique uppercase letters (order matters)
    std::map<char, std::string> currentElements;
    std::vector<char> variables;
    for (auto &[k, v] : Rsets)
    {
        if (currResults.find(k) == currResults.end())
        {
            variables.push_back(k);
        }
    }

    if (variables.empty())
    {
        // All uppercase letters already assigned
        return true;
    }

    // Step 2: initialize index vector
    std::vector<size_t> indices(variables.size(), 0);

    bool done = false;

    // Step 3: generate all combinations
    while (!done)
    {
        for (int i = 0; i < variables.size(); i++)
        {
            currentElements[variables[i]] = Rsets.at(variables[i])[indices[i]];
        }
        
        bool match = true;
        for (auto &&[string, b] : t_strings)
        {
            // Build one expanded string
            std::string result;
            for (char c : string)
            {
                if (std::islower(c))
                {
                    result += c;
                }
                else
                {
                    result += currentElements[c];
                }
            }
            // Check if result is substring of s
            if (s.find(result) == std::string::npos)
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            currResults.insert(currentElements.begin(), currentElements.end());
            return true;
        }

        // Increment indices like a counter
        for (int i = (int)indices.size() - 1; i >= 0; --i)
        {
            indices[i]++;
            if (indices[i] < Rsets.at(variables[i]).size())
                break; // still valid combination
            indices[i] = 0;
            if (i == 0)
                done = true; // overflow
        }
    }

    return false;
}

bool readInputFromStdin(int &n, std::string &s, std::vector<std::string> &Tstrings, std::vector<std::string> &Rsets)
{
    bool res = true;
    if (!(std::cin >> n)) 
    {
        res = false;
    } 
    if (!(std::cin >> s)) 
    {
        res = false;
    } 
    if (s.empty() || !isAllLowercase(s))
    {
        // Format, string must be only lower case letters
        res = false;
    }
    std::string line;
    for (int i = 0; i < n; i++)
    {
        if (!(std::cin >> line)) 
        {
            res = false;
        } 
        Tstrings.push_back(line);
    }

    while (std::cin >> line)
    {
        Rsets.push_back(line);
    }

    return res;
}

bool readInputFromFile(const std::string &file, int &n, std::string &s, std::vector<std::string> &Tstrings, std::vector<std::string> &Rsets)
{
    std::ifstream testFile(FOLDERPATH + file);
    if (!testFile)
    {
        std::cout << "Error: Could not open file.\n";
        return false;
    }

    if (!(testFile >> n))
    {
        // Format, first line must be an int
        testFile.close();
        return false;
    }
    std::string line;
    std::getline(testFile, line); // consume the newline after the number

    std::getline(testFile, s); // Get string s
    removeCarriage(s);

    for (int i = 0; i < n; ++i)
    {
        if (!std::getline(testFile, line))
        {
            // Format, there must be n lines
            testFile.close();
            return false;
        }
        removeCarriage(line);
        Tstrings.push_back(line);
    }

    while (std::getline(testFile, line))
    {
        removeCarriage(line);
        Rsets.push_back(line);
    }


    testFile.close();
    return true;

}

void filterTstrings(std::vector<std::pair<std::string, bool>> &t_strings)
{
    // Remove any strings with only 1 character(1 or more of the same),  because they will always be true
    // as long as their R set has > 0 values. This can only be done after checking
    // if any sets have no solutions
    for (auto it = t_strings.begin(); it != t_strings.end(); ) {
        if (isSingleCharString((*it).first)) {
            it = t_strings.erase(it); // erase returns the next iterator
        } else {
            ++it;
        }
    }
    
}

bool isSingleCharString(const std::string &s)
{
    return std::all_of(s.begin(), s.end(), [&](char c){ return c == s[0]; });
}

bool addStringToTstrings(std::vector<std::pair<std::string, bool>> &t_strings, const std::string &newStr, const bool &interestingLine)
{
    // Check if newStr is already a subset of an existing string
    for (const auto& s : t_strings) {
        if (s.first.find(newStr) != std::string::npos) {
            // newStr is contained in s, so we don't add it
            return false;
        }
    }

    // Remove any strings that are subsets of newStr
    for (auto it = t_strings.begin(); it != t_strings.end();) {
        if (newStr.find((*it).first) != std::string::npos) {
            it = t_strings.erase(it); // erase returns next iterator
        } else {
            ++it;
        }
    }

    // Add new string
    t_strings.push_back(std::make_pair(newStr, interestingLine));
    return true;
}

void replaceCapWithLower(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets)
{
    for (auto &&string : t_strings)
    {
        std::string newStr = "";
        for (auto &&c : string.first)
        {
            if (std::isupper(c) && Rsets[c].size() == 1)
            {
                newStr += Rsets[c][0];
            }
            else
            {
                newStr += c;
            }
        }
        string.first = newStr;
    }
}

void updateRepeats(int &count, std::map<char, int> &repeats, char &current, bool &interestingLine)
{
    if (count > 1)
    {
        if (repeats.count(current) > 0)
        {
            repeats[current] = std::max(count, repeats.at(current));
        }
        else
        {
            repeats[current] = count;
        }
        interestingLine = true;
    }
}

bool isAllLowercase(const std::string& s) 
{
    return std::all_of(s.begin(), s.end(), [](unsigned char c) {
        return std::islower(c);
    });
}

void removeCarriage(std::string &s)
{
    if (!s.empty() && s[s.size() - 1] == '\r')
    s.erase(s.size() - 1);
}

void createResults(std::map<char, std::string> &results, std::map<char, std::vector<std::string>> &Rsets)
{
    for (auto &&[k, v] : Rsets)
    {
        results[k] = v[0];
    }
    
}

void printReslt(std::map<char, std::string> result)
{
    for (auto &&[k, v] : result)
    {
        std::cout << k << ": " << v << std::endl;
    }
    
}

void printRSets(std::map<char, std::vector<std::string>> &Rsets)
{
    
    for (auto & [key, val] : Rsets)
    {
        std::cout << key << ": ";
        for (auto &&i : val)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
}

void printTStrings(std::vector<std::pair<std::string, bool>> &t_strings, const bool &all)
{
    for (auto &&i : t_strings)
    {
        if (all || i.second)
        {
            std::cout << i.first << ", ";
        }
    }
    std::cout << std::endl;
}