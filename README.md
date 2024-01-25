JSON parser for C++20 using std::any, templates and concepts. Allows loading JSON from a string or a file and operating on it in C++. 

**BUILDING**


To run the library simply do ```#include "JSON_parser.hpp"``` in your .cpp file and change tests/test_parser.cpp in SOURCES section in makefile to your .cpp file with the main function. Afterwards do ```make``` and run your executable.
You can also run tests written in CppUnit by doing ```make test```. You're also able to run the library in docker using the attached Dockerfile and the instructions inside it.

**LOADING**


Example string:
```cpp
std::string str =   "{  \n"
                        "    animals: [  \n"
                        "        { name: 'Cat', coordinates: [2, [5, -2], 8, [1, 2, 3]], height: 1.5},  \n"
                        "        { 'name': Dog, coordinates: [-4, 3], height: 2.5, width: 0.8 }, \n"
                        "        { name: Cow, coordinates: [-7, 0, -3], 'height': 4.0, comment: 'It\\'s says... moo.' },  \n"
                        "        { name: \"Goat\", coordinates: [1, -8, -9, 4]}  \n"
                        "    ],  \n"
                        "    owner: { name: 'Sam', age: 32, address: '123 Animal Drive, New York, 98765.' },  \n"
                        "    age: 32,\n"
                        "}";
```

To load it use 
```cpp
Resource resource(str);
```
or 
```cpp 
Resource resource("path/to/your/json/file", true);
```
to load it from a file.

**FIELD ACCESS**

To access fields you can use [] operators with template conversion functions specialized for int, long long, float, double and string. Example:
```cpp
float age = resource["owner"]["age"].as<float>();
```

You can also convert fields to vectors (or any container using the push_back method) or std::unordered_map, by using conversion methods as_vector<type> or as_map<type>. Example:
```cpp 
std::vector<int> dog_coords = resource["animals"][1]["coordinates"].as_vector<int>();
```

It is also possible to specialize the conversion function for one of your own custom classes like this:
```cpp
class Owner
{
    public:
        std::string name;
        int age;
        std::string address;
        Owner() {}; // default constructor needed
        Owner(std::string name, int age, std::string address) : name(name), age(age), address(address) {}
};

template<> // conversion specialized for custom type
Owner Resource::as<Owner>(const Owner& owner)
{
    /*
        The underlying types are std::string, std::vector<std::any> and std::unordered_map<std::string, std::any>. so
        you need to use them while doing std::any_cast during conversion
    */
    std::unordered_map<std::string, std::any> owner_map = std::any_cast<std::unordered_map<std::string, std::any>>(parsedData);
    std::string name = std::any_cast<std::string>(owner_map["name"]);
    std::string address = std::any_cast<std::string>(owner_map["address"]);
    int age = std::stoi(std::any_cast<std::string>(owner_map["age"]));
    return Owner(name, age, address);
}
```

**ASSIGNING VALUES**

You can assign new values to the structure simply by doing 
```cpp 
resource["owner"]["name"] = "name"
```
which will overwrite the "name" property. 
You're also able to assign vectors or maps: 
```cpp
    std::unordered_map<std::string, std::any> new_map;
    new_map["key"] = 5.62;
    resource["age"] = new_map;
    std::unordered_map<std::string, double> age_map = resource2["age"].as_map<double>(); // key is std::string by default
```
If you want the conversion functions to work properly after the assignment, in this case you need to have ```std::any``` as the value of the map. Same with ```std::vector```, ```std::any``` needs to be its type. That's because the conversion functions underneath do
```std::any_cast<type>``` where type is ```std::vector<std::any>```, ```std::unordered_map<std::string``` or ```std::string```.


More examples available in demo.cpp.


