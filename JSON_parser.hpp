#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <utility>
#include <algorithm>
#include <map>
#include <sstream>
#include <any>
#include <memory>

enum ResourceType {UNKNOWN, ARRAY, OBJECT, STRING};

class Resource;
class JSONParser;

class JSONParser
{
    public:
        JSONParser() {}
        std::string removeWhitespace(const std::string& input) 
        {
            std::string result;
            bool inString = false;
            char prevC = '\0';

            for (char c : input) {
                if (c == '\'' && prevC != '\\') {
                    inString = !inString;
                }

                if (!std::isspace(c) || inString) {
                    result += c;
                }
                prevC = c;
            }
            return result;
        }

        std::pair<std::string, std::size_t> removeOutermostCurlyBrackets(std::string currentObject, std::size_t objectStart)
        {
            std::size_t objectEnd;
            std::string nestedObject;
            std::stack<char> s;
            s.push('{');
            std::size_t i = objectStart + 1;

            while(!s.empty())
            {
                if (currentObject[i] == '{')
                    s.push('{');
                else if (currentObject[i] == '}')
                {
                    s.pop();
                    if (s.empty())
                        break;
                }
                
                nestedObject += currentObject[i];
                i++;
                if (i == currentObject.size()) // throw exception for wrong json
                {

                }
            }
            objectEnd = i;
            return std::make_pair(nestedObject, objectEnd);
        }

        std::pair<std::string, std::size_t> removeOuterMostSquareBrackets(std::string currentArray, std::size_t arrayStart)
        {
            std::size_t arrayEnd;
            std::string nestedArray;
            std::stack<char> s;
            s.push('[');
            std::size_t i = arrayStart + 1;

            while(!s.empty())
            {
                if (currentArray[i] == '[')
                    s.push('[');
                else if (currentArray[i] == ']')
                {
                    s.pop();
                    if (s.empty())
                        break;
                }
                
                nestedArray += currentArray[i];
                i++;
                if (i == currentArray.size()) // throw exception for wrong json
                {

                }
            }
            arrayEnd = i;
            return std::make_pair(nestedArray, arrayEnd);
        }        

        std::size_t handleString(std::string data, std::size_t pos)
        {
            std::size_t firstOccurrence = data[pos];
            std::size_t finalPos;
            std::string result;
            char prevC = '\0';
            for(std::size_t i = pos; i < data.size(); ++i)
            {
                result += data[i];
                if (data[i] == '\'' && prevC != '\\')
                {
                    finalPos = i + 1;
                    break;
                }
                prevC = data[i];
            }
            return finalPos;
        }

        std::pair<std::vector<std::any>, std::size_t> handleArray(std::string data, std::size_t pos)
        {
            std::vector<std::any> arr; 
            std::size_t startPos = 0;
            std::size_t endPos;
            std::size_t i = startPos;
            std::pair<std::string, std::size_t> result = removeOuterMostSquareBrackets(data, pos);
            data = result.first;
            std::size_t endOfArrayPos = result.second;
            std::string value;
            while (startPos < data.size())
            {
                if (startPos >= endOfArrayPos)
                {
                    break;
                }
                else if (data[startPos] == '[')
                {
                    std::pair<std::vector<std::any>, std::size_t> result = handleArray(data, startPos);
                    arr.push_back(result.first);
                    endPos = result.second + 1;
                }
                else if (data[startPos] == '{')
                {
                    std::pair<std::any, std::size_t> result = parseUtil(data, startPos);
                    arr.push_back(result.first);
                    endPos = result.second + 1;                    
                }
                else 
                {
                    startPos = data.find_first_not_of(',', startPos);
                    endPos = data.find(',', startPos);
                    if (endPos == std::string::npos)
                        endPos = data.size();
                    value = data.substr(startPos, endPos - startPos);
                    arr.push_back(value);
                }
                startPos = endPos + 1;
            }
            return std::make_pair(arr, endOfArrayPos);
        }

        std::any parse(const std::string& data)
        {
            std::unordered_map<std::string, std::any> map;
            std::size_t objectEnd = 0;
            std::string dataWithoutWhitespace = removeWhitespace(data);
            std::string currentObject = dataWithoutWhitespace;
            std::size_t objectStart = 0;
            std::pair<std::any, std::size_t> result = parseUtil(currentObject, objectStart);
            return result.first;
        }

        std::pair<std::any, std::size_t> parseUtil(std::string currentObject, std::size_t objectStart)
        {
            std::unordered_map<std::string, std::any> map;
            std::vector<std::any> arr;
            std::size_t keyStartPos = 0;
            std::size_t keyEndPos;
            std::size_t valStartPos;
            std::size_t valEndPos;
            std::pair<std::string, std::size_t> result = removeOutermostCurlyBrackets(currentObject, objectStart);
            currentObject = result.first;
            std::size_t objectEnd = result.second;

            while ((keyEndPos = currentObject.find(':', keyStartPos)) != std::string::npos)
            {
                if ((valStartPos = currentObject.find_first_not_of(":", keyEndPos)) == std::string::npos)
                    break;

                std::string key = currentObject.substr(keyStartPos, keyEndPos - keyStartPos);

                if (currentObject[valStartPos] == '\'')
                {
                    valEndPos = handleString(currentObject, valStartPos + 1);
                }
                else if (currentObject[valStartPos] == '{')
                {
                    std::pair<std::any, std::size_t> result = parseUtil(currentObject, valStartPos);
                    map[key] = result.first;
                    valEndPos = result.second + 1;
                }
                else if (currentObject[valStartPos] == '[')
                {
                    std::pair<std::vector<std::any>, std::size_t> result = handleArray(currentObject, valStartPos);
                    map[key] = result.first;
                    valEndPos = result.second + 1;
                }
                else
                {
                    valEndPos = currentObject.find(',', valStartPos);
                    if (valEndPos == std::string::npos)
                        valEndPos = currentObject.size();
                }

                auto it = map.find(key);
                if (it == map.end())
                {
                    std::string value = currentObject.substr(valStartPos, valEndPos - valStartPos);
                    map[key] = value;
                }
                keyStartPos = valEndPos + 1;
            }
            return std::make_pair(map, objectEnd);
        }
};

class Resource
{
    private:
        std::string data;
        std::unordered_map<std::string, std::any> parsedData;
        std::vector<std::any> arr;
        std::string value;
        Resource* resourcePtr;
        JSONParser parser;
        Resource(std::string value, bool parse) : value(value) {}
        Resource(std::vector<std::any>, bool parse) {}
        Resource(std::unordered_map<std::string, std::any> map, bool parse) : parsedData(map) {}
    public:
        Resource(std::string data) : data(data)
        {
            parsedData = std::any_cast<std::unordered_map<std::string, std::any>>(parser.parse(data));
        }
        Resource& operator[](std::string key)
        {
            if (parsedData[key].type() == typeid(std::string))
            {
               resourcePtr = new Resource(std::any_cast<std::string>(parsedData[key]), false);
            }
            else if (parsedData[key].type() == typeid(std::vector<std::any>))
            {
                resourcePtr = new Resource(std::any_cast<std::vector<std::any>>(parsedData[key]), false);
            }
            else if (parsedData[key].type() == typeid(std::unordered_map<std::string, std::any>))
            {
                resourcePtr = new Resource(std::any_cast<std::unordered_map<std::string, std::any>>(parsedData[key]), false);
            }
            return *resourcePtr;
        }
        ~Resource()
        {
            delete resourcePtr;
        }
};

