#include <iostream>
#include "JSON_parser.hpp"
#include <list>
#include <stack>
#include <fstream>

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
    std::string s = "text: 'It's his fault'";
    //std::cout << "The JSON string:\n\n" << str << "\n" << std::endl;
    Resource resource(str);
    //std::list<int> l;
    //Resource resource1 = resource["owner"]["age"];
    //resource["animals"];
    //std::vector<int> v = resource["coordinates"].as_vector<int>();
    //std::vector<double> coordinates = resource["animals"][0]["coordinates"][3].as_vector<double>();
    //for (auto coordinate : coordinates)
    //    std::cout<<coordinate << std::endl;
    //int age = resource["owner"]["age"].as<int>();
    //std::cout << age << std::endl;
    //std::unordered_map<std::string, std::any> map = resource["owner"].as_map<std::any>();
    //map["name"] = resource["owner"]["age"].as<int>();
    
    //Resource res = resource["owner1"]["height"];
   //resource["owner1"]["height"] = 180;
   //std::vector<std::any> v{10, 50};
//    resource["animals"][0]["coordinates"] = v;   
//    std::vector<int> v1 = resource["animals"][0]["coordinates"].as_vector<int>();
   //std::any res = resource["animals"][0]["coordinates"].as<std::any>();
   //std::vector<std::any> v2 = resource["animals"][0]["coordinates"].as_vector<std::any>();
    //std::cout << resource["age"].as<int>();
    //std::cout << std::to_string()
    //std::cout<<resource["owner"]["name"].as<std::string>()<<std::endl;

    Resource resource1("json_files/example2.json", true);
    std::cout <<resource1["animals"][0]["height"].as<int>();
}