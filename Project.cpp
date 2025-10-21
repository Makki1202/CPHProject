#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <chrono>
#include <algorithm>
#include "Project.h"

const std::string FOLDERPATH = "tests/";
const std::string TESTPREFIX = "test0";
const std::string FILEEXTENSION = ".swe";
const int FILEAMOUNT = 7;

bool checkTStringAndFindRepeats(const std::string &t_string, std::map<char, int> &repeats, std::set<char> &upperCharsUsed, const std::string &s, bool &interestingLine) {

    bool valid = true;
    char current = t_string[0];
    int count = 1;
    std::string lowercaseString = "";
    if (std::isupper(current))
    {
        upperCharsUsed.insert(current);
    }
    else if (std::islower(current))
    {
        lowercaseString += current;
        interestingLine = true;
    }
    else
    {
        valid = false;
        return valid;
    }

    for (size_t i = 1; i < t_string.size(); ++i) {
        if (std::isupper(t_string[i]))
        {
            upperCharsUsed.insert(t_string[i]);
            if (s.find(lowercaseString) == std::string::npos)
            {
                valid = false;
                break;
            }
            else
            {
                lowercaseString = "";
            }
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
            current = t_string[i];
            count = 1;
        }
    }
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

    return valid;
}

std::vector<std::string> simpleConvertStringToContainer(std::string&line)
{
    std::vector<std::string> values;
    // Find the part after ':'
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos) {
        std::string items = line.substr(colonPos + 1);

        std::stringstream ss(items);
        std::string token;

        while (std::getline(ss, token, ',')) {
            if (!token.empty())
            {
                values.push_back(token);
            }
        }
    }

    return values;
}

std::vector<std::string> convertStringToContainer(std::string&line, const std::string &s, const std::map<char, int> &repeats)
{
    std::vector<std::string> values;
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

                if (repeats.count(line[0]) > 0)
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

bool isSingleCharString(const std::string &s) {
    if (s.empty()) return false; // optional, depends on whether "" counts
    return std::all_of(s.begin(), s.end(), [&](char c){ return c == s[0]; });
}

bool addStringToSet(std::vector<std::pair<std::string, bool>> &t_strings, const std::string &newStr, const bool &interestingLine) {
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


void filterTstrings(std::vector<std::pair<std::string, bool>> &t_strings)
{
    for (auto it = t_strings.begin(); it != t_strings.end(); ) {
        if (isSingleCharString((*it).first)) {
            it = t_strings.erase(it); // erase returns the next iterator
        } else {
            ++it;
        }
    }
    
}

int runRemoveElementsFromSets(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, const std::string &s, std::map<char, std::string> &results, std::string &retMessage, const bool &all = false)
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

        // Step 3: generate all combinations
        while (!done)
        {
            // Build one expanded string
            std::string result;
            for (char c : pattern.first)
            {
                if (std::islower(c))
                {
                    result += c;
                }
                else
                {
                    // find the index corresponding to this variable
                    size_t varIdx = std::distance(
                        variables.begin(),
                        std::find(variables.begin(), variables.end(), c));
                    std::string val = Rsets.at(c)[indices[varIdx]];
                    result += val;
                    currentElements[c] = val;
                }
            }

            // Check if result is substring of s
            if (s.find(result) != std::string::npos)
            {
                // std::cout << "success with ";
                for (auto &&[k, v] : currentElements)
                {
                    // std::cout << k << ": " << v << " ";
                    usedElements[k].insert(v);
                }
                // std::cout << std::endl;
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

            if (variables.empty()) break;
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
            }
            if (list.empty())
            {
                retMessage = "NO";
                return 1;
            }
        }
    }

    return retVal;
}

std::map<char, std::string> runAllCombos(std::vector<std::pair<std::string, bool>> &t_strings, std::map<char, std::vector<std::string>> &Rsets, const std::string &s, std::string &retMessage)
{
    // Step 1: collect unique uppercase letters (order matters)
    std::map<char, std::string> currentElements;
    std::vector<char> variables;
    for (auto &[k, v] : Rsets)
    {
        variables.push_back(k);
    }

    // Step 2: initialize index vector
    std::vector<size_t> indices(variables.size(), 0);

    bool done = variables.empty();

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
            retMessage = "YES";
            return currentElements;
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

        if (variables.empty()) break;
    }

    return {};
}

int main() {
    auto programStart = std::chrono::high_resolution_clock::now();

    for (int i = 1; i <= FILEAMOUNT; i++)
    {
        auto fileStart = std::chrono::high_resolution_clock::now();
        std::string retMessage = "";
        std::string file = std::string(TESTPREFIX + std::to_string(i) + FILEEXTENSION);
        bool retVal = runTest(file, retMessage);
        if (retVal)
        {
            std::cout << file << ": " << retMessage << std::endl;
    
            auto end = std::chrono::high_resolution_clock::now();
            // Calculate elapsed time in microseconds
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - fileStart);

            std::cout << file << " execution time: " << duration.count() << " us" << std::endl;
        }
        else
        {
            std::cout << file << ": " << retMessage << std::endl;
        }
        
    }

    auto end = std::chrono::high_resolution_clock::now();
    // Calculate elapsed time in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - programStart);

    std::cout << "Total execution time: " << duration.count() << " us" << std::endl;
}
bool isAllLowercase(const std::string& s) {
    return std::all_of(s.begin(), s.end(), [](unsigned char c) {
        return std::islower(c);
    });
}

void removeCarriage(std::string &s)
{
    if (!s.empty() && s[s.size() - 1] == '\r')
    s.erase(s.size() - 1);
}

bool runTest(const std::string &file, std::string &retMessage)
{
    std::ifstream testFile(FOLDERPATH + file);
    if (!testFile)
    {
        retMessage = "Error: Could not open file.\n";
        return false;
    }

    int n;
    if (!(testFile >> n))
    {
        // Format, first line must be an int
        retMessage = "NO"; 
        testFile.close();
        return true;
    }

    std::string line;
    std::getline(testFile, line); // consume the newline after the number
    std::string s;
    std::getline(testFile, s); // Get string s
    removeCarriage(s);
    if (s.empty() || !isAllLowercase(s))
    {
        // Format, string must be only lower case letters
        retMessage = "NO";
        testFile.close();
        return true;
    }

    std::map<char, int> repeats; // Map for repetitions of uppercase letters
    std::set<char> upperCharsUsed; // Set for seeing what R sets we need
    std::vector<std::pair<std::string, bool>> t_strings; // t strings

    for (int i = 0; i < n; ++i)
    {
        if (!std::getline(testFile, line))
        {
            // Format, there must be n lines
            retMessage = "NO";
            testFile.close();
            return true;
        }
        removeCarriage(line);
        // Check if string is a possible substring based on lowercase letters and
        // find repetitions of upper case letters and
        // store what upper case letters are used.
        bool interestingLine = false;
        if (checkTStringAndFindRepeats(line, repeats, upperCharsUsed, s, interestingLine))
        {
            addStringToSet(t_strings, line, interestingLine);
        }
        else
        {
            // Bad format, or a lowercase letter from the t_string is not in string s
            retMessage = "NO";
            testFile.close();
            return true;
        }
    }

    // if (repeats.empty()) {
    //     std::cout << "No repeated patterns.\n";
    // } else {
    //     for (auto &[k, v] : repeats)
    //         std::cout << "Character '" << k
    //                 << "' repeated " << v << " times.\n";
    // }

    std::map<char, std::vector<std::string>> Rsets;

    size_t counter = 0; // Counter to make sure we have a set for every upper char
    while (std::getline(testFile, line))
    {
        removeCarriage(line);
        // TODO: Maybe check that line is correct format
        // If a set is not used discard it
        if (upperCharsUsed.find(line[0]) == upperCharsUsed.end())
        {
            continue;
        }
        counter++;
        // Convert the R set string to a vector while getting rid of elements
        // that are not in string s or repetitions of that element that are not in s
        std::vector<std::string> strings = convertStringToContainer(line, s, repeats);
        // std::vector<std::string> strings = simpleConvertStringToContainer(line);
        
        // If any vector is empty, then there is no solution.
        if (strings.empty())
        {
            retMessage = "NO";
            testFile.close();
            return true;
        }

        // Save the R set
        Rsets[line[0]] = strings;
    }
    testFile.close();

    if (counter != upperCharsUsed.size())
    {
        // Format, we're missing a set
        retMessage = "NO";
        return true;
    }

    std::map<char, std::string> results;

    printTStrings(t_strings);
    printRSets(Rsets);

    // Remove any strings with only 1 character(1 or more of the same),  because they will always be true
    // as long as their R set has > 0 values. This can only be done after checking
    // if any sets have no solutions
    filterTstrings(t_strings);
    // printTStrings(t_strings);
    size_t prevSize = results.size();
    size_t newSize = 0;
    int outcome = 0;
    do
    {
        // std::cout << "Trying interesting strings:" << std::endl;
        outcome = runRemoveElementsFromSets(t_strings, Rsets, s, results, retMessage);
        if (outcome == 1)
        {
            return true;
        }
        newSize = results.size();
        if (prevSize == newSize) // No new sets only have 1 element
        {
            continue;
        }
        prevSize = newSize;
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

    } while (outcome == 2);
    
    // printRSets(Rsets);


    // printTStrings(t_strings, true);
    outcome = runRemoveElementsFromSets(t_strings, Rsets, s, results, retMessage, true);
    if (outcome == 1)
    {
        return true;
    }

    std::map<char, std::string> result = runAllCombos(t_strings, Rsets, s, retMessage);

    if (result.empty())
    {
        retMessage = "NO";
        return true;
    }
    else
    {
        printReslt(result);
        return true;
    }
}

void printReslt(std::map<char, std::string> result)
{
    std::cout << "Results: " << std::endl;
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