#include <iostream>
#include "JSON_parser.hpp"

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
    "    owner: { name: Sam, age: 32, animal: {cat: meow}, address: '123 Animal Drive, New York, 98765.' },  \n"
    "    owner1: {name: Marcus, age: 54},"
    "    age: 532, height: 182.5,\n"
    "}";
    std::string s = "text: 'It's his fault'";
    //std::cout << "The JSON string:\n\n" << str << "\n" << std::endl;
    JSONParser parser;
    Resource resource(str);
    resource["owner"]["name"];
}