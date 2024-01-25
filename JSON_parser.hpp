#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

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
#include <concepts>
#include <fstream>
#include <filesystem>

class Resource;
class JSONParser;

class JSONParser
{
    public:
        JSONParser() {}
        std::string removeWhitespace(const std::string&);
        std::pair<std::string, std::size_t> removeOutermostCurlyBrackets(std::string, std::size_t);
        std::pair<std::string, std::size_t> removeOuterMostSquareBrackets(std::string, std::size_t );
        std::size_t handleString(std::string, std::size_t);
        std::pair<std::vector<std::any>, std::size_t> handleArray(std::string, std::size_t);
        std::any parse(const std::string&);
        std::pair<std::any, std::size_t> parseUtil(std::string, std::size_t);
};

template <typename T, typename U>
concept IsVectorOfDataType = requires(T t, U elem)
{
    // Checks that T has a push_back method and its value_type is the same as dataType
    { t.push_back(elem) } -> std::same_as<void>;
    requires std::same_as<typename T::value_type, U>;
};

template <typename T, typename ValueType>
concept IsMapOfDataType = requires(ValueType value)
{
    requires std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher, typename T::key_equal, 
             typename T::allocator_type>>;
    requires std::same_as<typename T::mapped_type, ValueType>;
};

template <typename T>
concept Numerical = std::is_arithmetic_v<T>;

class Resource
{
    private:
        std::string data;
        std::any parsedData;
        std::unique_ptr<Resource> resourceNextPtr = nullptr;
        Resource* resourcePrevPtr = nullptr;
        std::vector<std::any> keys;
        JSONParser parser;
        Resource(std::any parsedData, std::string otherConstructor) : parsedData(parsedData) {}
        
    public:
        std::any parseData(std::string dataToParse);
        Resource() {}
        Resource(std::string data, bool filePath = false);
        Resource(const Resource&);
        Resource(Resource&&);
        Resource& operator= (const Resource&);
        std::any modifyMap(std::size_t, std::any, std::any);

        template <typename T>
        Resource& insertValue(T value)
        {
            Resource* current = this;
            while (current->resourcePrevPtr != nullptr)
                current = current->resourcePrevPtr;

            current->parsedData = modifyMap(0, value, current->parsedData);
            parsedData = value;
            return *this;
        }   
        

        template <typename T>
        Resource& operator= (T value)
        {
            return insertValue(value);
        }

        template <Numerical DataType>
        Resource& operator= (DataType value)
        {   
            std::string SValue = std::to_string(value);
            return insertValue(SValue);
        }

        Resource& operator[](std::string);
        Resource& operator[](std::size_t);

        template<typename dataType>
        dataType as(const dataType& def = dataType())
        {
            try
            {
                return std::any_cast<dataType>(parsedData);
            }
            catch(const std::bad_any_cast& e)
            {
                return def;
            }
        }

        template<typename DataType, IsVectorOfDataType<DataType> VectorType=std::vector<DataType>>
        VectorType as_vector(const VectorType& def = VectorType())
        {
            try
            {
                VectorType res;
                std::vector<std::any> dataV = std::any_cast<std::vector<std::any>>(parsedData);
                std::vector<Resource> resourceV;
                for (std::size_t i = 0; i < dataV.size(); ++i)
                    resourceV.push_back(Resource(dataV[i], ""));
                for (std::size_t i = 0; i < resourceV.size(); ++i)
                    res.push_back(resourceV[i].as<DataType>());
                return res;
            }
            catch (const std::bad_any_cast& e)
            {
                return def;
            }
        }
        
        // in as_map key is always a string
        template<typename ValueType, IsMapOfDataType<ValueType> MapType=std::unordered_map<std::string, ValueType>>
        MapType as_map(const MapType& def = MapType())
        {
            try
            {
                MapType res;
                std::unordered_map<std::string, std::any> dataM = std::any_cast<std::unordered_map<std::string, std::any>>(parsedData);
                std::unordered_map<std::string, Resource> resourceM;
                for (const auto& pair : dataM)
                    resourceM[pair.first] = Resource(dataM[pair.first], "");
                for (const auto& pair : resourceM)
                    res[pair.first] = resourceM[pair.first].as<ValueType>();
                return res;
            }
            catch (const std::bad_any_cast& e)
            {
                return def;
            }
        }
};

#endif // JSON_PARSER_HPP