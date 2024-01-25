#include <iostream>
#include "JSON_parser.hpp"
#include <list>
#include <stack>
#include <fstream>

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
    std::unordered_map<std::string, std::any> owner_map = std::any_cast<std::unordered_map<std::string, std::any>>(parsedData);
    std::string name = std::any_cast<std::string>(owner_map["name"]);
    std::string address = std::any_cast<std::string>(owner_map["address"]);
    int age = std::stoi(std::any_cast<std::string>(owner_map["age"]));
    return Owner(name, age, address);
}


int main ()
{
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

    std::string str1 =  "{ \n"
                        " animal:{dog:{cat:{meow: 1}}}, \n"
                        "} \n";

        std::string str2 =   "{  \n"
    "    owner: { name: Sam, age: 32.5, animal: {cat: meow}, address: '123 Animal Drive, New York, 98765.' },  \n"
    "    owner1: {height: 187.6, age: 54}, \n"
    "    coordinates: [1, 22, 33, -5],       \n"
    "    age: 532, height: 182.5,\n"
    "}";

    // creating Resource instance
    Resource resource(str);
    Resource resource2(str2);

    // accessing fields

    std::vector<int> dog_coords = resource["animals"][1]["coordinates"].as_vector<int>();
    for (auto coordinate : dog_coords)
        std::cout << coordinate << " ";
    std::cout << std::endl;

    float age = resource2["owner"]["age"].as<float>();
    std::cout << age << std::endl;

    std::unordered_map<std::string, std::any> owner_map = resource2["owner"].as_map<std::any>();
    std::string name = std::any_cast<std::string>(owner_map["name"]);
    std::cout << name << std::endl;

    // or

    name = resource2["owner"]["name"].as<std::string>();
    std::cout << name << std::endl;

    // modifying fields
    std::cout << resource2["owner"]["animal"]["cat"].as<std::string>() << std::endl;
    resource2["owner"]["animal"]["cat"] = "woof";
    std::cout << resource2["owner"]["animal"]["cat"].as<std::string>() << std::endl;
    // you have to pass a vector of std::any if you wish to modify a field to be an array
    std::vector<std::any> v{10, 50};
    resource["animals"][0]["coordinates"] = v;   
    std::vector<int> v1 = resource["animals"][0]["coordinates"].as_vector<int>(); // you need to convert to the same type you passed to the 
    // vector (int in this case), unless you pass a vector of strings
    for (auto elem : v1)
        std::cout << elem << " ";
    std::cout << std::endl;

    // if you wish that a field was an object, you need to pass std::unordered_map<std::string, std::any>
    std::unordered_map<std::string, std::any> new_map;
    new_map["key"] = 5.62;
    resource2["age"] = new_map;
    std::unordered_map<std::string, double> age_map = resource2["age"].as_map<double>();
    for (auto it = age_map.begin(); it != age_map.end(); ++it)
    {
        std::cout << "Key: " << it->first << ", Value: " << it->second << std::endl;
    }
    // you can also specialize the conversion function for your own custom type
    Owner owner = resource["owner"].as<Owner>();
    std::cout << "name: " << owner.name << ", " << "age: " << owner.age << ", " << "address: "<< owner.address << std::endl;
}