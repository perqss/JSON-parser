#include "JSON_parser.hpp"
// --------------------------------------------------------
// JSONParser class methods
//---------------------------------------------------------

std::string JSONParser::removeWhitespace(const std::string& input)
{
    std::string result;
    bool inString = false;
    char prevC = '\0';

    for (char c : input)
    {
        if ((c == '\'' && prevC != '\\') || (c == '"' && prevC != '\\'))
        {
            inString = !inString;
        }

        if ((!std::isspace(c) || inString) && c != '"')
        {
            result += c;
        }
        prevC = c;
    }
    return result;
}

std::pair<std::string, std::size_t> JSONParser::removeOutermostCurlyBrackets(std::string currentObject, std::size_t objectStart)
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
            throw std::runtime_error("Bad JSON format");
        }
    }
    objectEnd = i;
    return std::make_pair(nestedObject, objectEnd);
}

std::pair<std::string, std::size_t> JSONParser::removeOuterMostSquareBrackets(std::string currentArray, std::size_t arrayStart)
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
            throw std::runtime_error("Non JSON format");
        }
    }
    arrayEnd = i;
    return std::make_pair(nestedArray, arrayEnd);
}

std::size_t JSONParser::handleString(std::string data, std::size_t pos)
{
    std::size_t firstOccurrence = data[pos];
    std::size_t finalPos;
    std::string result;
    char prevC = '\0';
    for(std::size_t i = pos; i < data.size(); ++i)
    {
        result += data[i];
        if ((data[i] == '\'' && prevC != '\\') || (data[i] == '"' && prevC != '\\'))
        {
            finalPos = i + 1;
            break;
        }
        prevC = data[i];
    }
    return finalPos;
}

std::pair<std::vector<std::any>, std::size_t> JSONParser::handleArray(std::string data, std::size_t pos)
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

std::any JSONParser::parse(const std::string& data)
{
    std::unordered_map<std::string, std::any> map;
    std::size_t objectEnd = 0;
    std::string dataWithoutWhitespace = removeWhitespace(data);
    std::string currentObject = dataWithoutWhitespace;
    std::size_t objectStart = 0;
    std::pair<std::any, std::size_t> result = parseUtil(currentObject, objectStart);
    return result.first;
}

std::pair<std::any, std::size_t> JSONParser::parseUtil(std::string currentObject, std::size_t objectStart)
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

        if (currentObject[valStartPos] == '\'' || currentObject[valStartPos] == '"')
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

// --------------------------------------------------------
// Resource class methods
//---------------------------------------------------------

std::any Resource::parseData(std::string dataToParse)
{
    try
    {
        return std::any_cast<std::unordered_map<std::string, std::any>>(parser.parse(dataToParse));
    }
    catch (const std::bad_any_cast& e)
    {
        throw std::runtime_error("Passed string is not in JSON format");
    }
}

Resource::Resource(std::string data, bool filePath)
{
    if (!filePath)
    {
        this->data = data;
        parsedData = parseData(data);
    }
    else 
    {
        std::filesystem::path filePath(data);
        if (std::filesystem::exists(filePath))
        {
            std::ifstream t(data);
            std::stringstream buffer;
            buffer << t.rdbuf();
            std::string dataToParse = buffer.str();
            this->data = dataToParse;
            parsedData = parseData(dataToParse);
        }
        else
        {
            throw std::runtime_error("File does not exist");
        }
    }
}

Resource::Resource(const Resource& other) : data(other.data), parsedData(other.parsedData), resourcePrevPtr(other.resourcePrevPtr), keys(other.keys)
{
    if (other.resourceNextPtr != nullptr) 
        resourceNextPtr = std::unique_ptr<Resource>(new Resource(other.resourceNextPtr->parsedData, ""));
}

Resource::Resource(Resource&& source) : data(std::move(source.data)), parsedData(std::move(source.parsedData)), resourceNextPtr(std::move(source.resourceNextPtr)), 
                                      parser(std::move(source.parser)), resourcePrevPtr(source.resourcePrevPtr), keys(std::move(source.keys))
{
    source.resourcePrevPtr = nullptr;
}

Resource& Resource::operator= (const Resource& source)
 {
    if (this != &source)
    {
        data = source.data;
        parsedData = source.parsedData;
        resourceNextPtr = std::unique_ptr<Resource>(new Resource(source.parsedData, ""));
        resourcePrevPtr = source.resourcePrevPtr;
        keys = source.keys;
        parser = source.parser;
    }
    return *this;
}

std::any Resource::modifyMap(std::size_t index, std::any value, std::any data)
{
    bool isValueString = false;
    bool isKeyString = false;

    std::string Skey;
    std::size_t Ikey;

    if (index == keys.size())
        return value;

    if (keys[index].type() == typeid(std::string))
    {
        Skey = std::any_cast<std::string>(keys[index]);
        isKeyString = true;
        std::unordered_map<std::string, std::any> map = std::any_cast<std::unordered_map<std::string, std::any>>(data);
        data = modifyMap(index + 1, value, map[Skey]);
        map[Skey] = data;
        return map;
    }
    else
    {
        Ikey = std::any_cast<std::size_t>(keys[index]);
        isKeyString = false;
        std::vector<std::any> v = std::any_cast<std::vector<std::any>>(data);
        data = modifyMap(index + 1, value, v[Ikey]);
        v[Ikey] = data;
        return v;
    }
}

Resource& Resource::operator[](std::string key)
{
    try
    {
        std::unordered_map<std::string, std::any> map = std::any_cast<std::unordered_map<std::string, std::any>>(parsedData);
        std::any value = map.at(key);
        resourceNextPtr = std::unique_ptr<Resource>(new Resource());
        resourceNextPtr->parsedData = value;
        resourceNextPtr->resourcePrevPtr = this;
        resourceNextPtr->keys = keys;
        resourceNextPtr->keys.push_back(key);
        return *resourceNextPtr;
    }
    catch (const std::bad_any_cast& e)
    {
        throw std::runtime_error("Object does not have a key: " + key);
    }
    catch (const std::out_of_range& e)
    {
        throw std::runtime_error("Key: " + key + " out of range");
    }
}

Resource& Resource::operator[](std::size_t index)
{
    try
    {
        std::vector<std::any> v = std::any_cast<std::vector<std::any>>(parsedData);
        std::any value = v.at(index);
        resourceNextPtr = std::unique_ptr<Resource>(new Resource());
        resourceNextPtr->parsedData = value;
        resourceNextPtr->resourcePrevPtr = this;
        resourceNextPtr->keys = keys;
        resourceNextPtr->keys.push_back(index);
        return *resourceNextPtr;
    }
    catch(const std::bad_any_cast& e)
    {
        throw std::runtime_error("Vector does not have a index: " + std::to_string(index));
    }
    catch (const std::out_of_range& e)
    {
        throw std::runtime_error("Index: " + std::to_string(index) + " out of range");
    }
}

template<>
Resource& Resource::operator=<const char*>(const char* value)
{
    std::string SValue = value;
    return insertValue(SValue);
}

template<>
std::string Resource::as<std::string>(const std::string& def)
{
    try
    {
        return std::any_cast<std::string>(parsedData);
    }
    catch(const std::bad_any_cast& e)
    {
        return def;
    }
}

template<>
long long Resource::as<long long>(const long long& def)
{
    try
    {
        if (parsedData.type() == typeid(std::string))
            return std::stoll(std::any_cast<std::string>(parsedData));
        else
            return std::any_cast<long long>(parsedData);
    }
    catch(const std::exception& e)
    {
        return def;
    }
}

template<>
int Resource::as<int>(const int& def)
{
    try
    {
        if (parsedData.type() == typeid(std::string))
            return std::stoi(std::any_cast<std::string>(parsedData));
        else
            return std::any_cast<int>(parsedData);
    }
    catch(const std::exception& e)
    {
        return def;
    }
}

template<>
float Resource::as<float>(const float& def)
{
    try
    {
        if (parsedData.type() == typeid(std::string))
            return std::stof(std::any_cast<std::string>(parsedData));
        else
            return std::any_cast<float>(parsedData);
    }
    catch(const std::exception& e)
    {
        return def;
    }
}

template<>
double Resource::as<double>(const double& def)
{
    try
    {
        if (parsedData.type() == typeid(std::string))
            return std::stod(std::any_cast<std::string>(parsedData));
        else
            return std::any_cast<double>(parsedData);
    }
    catch(const std::exception& e)
    {
        return def;
    }
}

// specialization for any type
template<>
std::any Resource::as<std::any>(const std::any& def)
{
    return parsedData;
}