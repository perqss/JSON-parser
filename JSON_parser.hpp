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

enum ResourceType {UNKNOWN, ARRAY, OBJECT, PRIMITIVE};

class Resource;

class Resource
{
    private:
        std::string data;
        std::unordered_map<std::string, Resource*> resource_map;
        std::vector<Resource*> resource_vector;
    public:
        Resource(std::string data) : data(data) {}
};

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

        std::string removeOutermostCurlyBrackets(std::string& data, std::string result, std::size_t& objectEnd)
        {
            std::string parsedData;
            std::stack<char> s;
            std::size_t firstOccurrence = data.find('{');

            if (firstOccurrence == std::string::npos)
                return result;

            data.erase(firstOccurrence, 1);
            s.push('{');
            std::size_t i = firstOccurrence;
            while(!s.empty())
            {
                if (data[i] == '{')
                    s.push('{');
                else if (data[i] == '}')
                {
                    s.pop();
                    if (s.empty())
                        break;
                }
                parsedData += data[i];
                i++;
                if (i == data.size()) // throw exception for wrong json
                {

                }
            }
            data.erase(i, 1);
            std::size_t sizeDiff = result.size() - data.size();
            objectEnd = i + sizeDiff;
            return parsedData;
        }



        // std::size_t handleObject(std::string key, std::string& data, std::size_t pos, std::unordered_map<std::string, std::any>& map)
        // {
        //     std::stack<char> s;
        //     s.push(data[pos]);
        //     std::size_t keyStartPos = pos + 1;
        //     std::size_t keyEndPos;
        //     std::size_t valStartPos;
        //     std::size_t valEndPos;
        //     while ((keyEndPos = data.find(':', keyStartPos)) != std::string::npos)
        //     {
        //         valStartPos = data.find_first_not_of(":", keyEndPos);

        //     }
        // }

        // std::size_t handleArray(std::string key, std::string& arr, std::size_t po, std::unordered_map<std::string, std::any>& map)
        // {

        // }
        void handlePrimitiveType(std::string key, std::string value, std::unordered_map<std::string, std::any>& map)
        {   
            map[key] = value;
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
            }
            return finalPos;
        }

        std::any parse(const std::string& data)
        {
            std::unordered_map<std::string, std::any> map;
            std::size_t objectEnd = 0;
            std::string dataWithoutWhitespace = removeWhitespace(data);
            std::string result = dataWithoutWhitespace;
            return parseUtil(dataWithoutWhitespace, result, objectEnd);
        }

        std::any parseUtil(std::string& data, std::string result, std::size_t& objectEnd)
        {
            std::unordered_map<std::string, std::any> map;
            std::size_t keyStartPos = 0;
            std::size_t keyEndPos;
            std::size_t valStartPos;
            std::size_t valEndPos;
            char val;

            result = removeOutermostCurlyBrackets(data, result, objectEnd);

            while ((keyEndPos = result.find(':', keyStartPos)) != std::string::npos)
            {
                if ((valStartPos = result.find_first_not_of(":", keyEndPos)) == std::string::npos)
                    break;
                //valEndPos = data.find(',', valStartPos);
                std::string key = result.substr(keyStartPos, keyEndPos - keyStartPos);
                // if (data[valStartPos] == '{')
                // {
                //     parse(data, valStartPos + 1);
                // }
                if (result[valStartPos] == '\'')
                {
                    valEndPos = handleString(result, valStartPos + 1);
                }
                else if (result[valStartPos] == '{')
                {
                    map[key] = parseUtil(data, result, objectEnd);
                    valEndPos = objectEnd;
                    val = result[valEndPos];
                }
                else if (result[valStartPos] == '[')
                {

                }
                else
                {
                    valEndPos = result.find(',', valStartPos);
                    if (valEndPos == std::string::npos)
                        valEndPos = result.size();
                }

                auto it = map.find(key);
                if (it == map.end())
                {
                    std::string value = result.substr(valStartPos, valEndPos - valStartPos);
                    map[key] = value;
                }
                keyStartPos = valEndPos + 1;
                //handlePrimitiveType(key, value, map);
                //std::cout << "key: " << key << " " << "value: "<< value << std::endl;
            }
            return map;
        }

        void parse1(std::string& data)
        {
            std::map<std::string, std::string> m;

            std::string key, val;
            std::istringstream iss(data);

            while(std::getline(std::getline(iss, key, ':') >> std::ws, val))
            {
                std::cout << "key: " << key << " " << "value: "<< val << std::endl;
                m[key] = val;
            }
        }

        void parse2(std::string& data)
        {

        }
};

