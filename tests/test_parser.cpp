#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestSuite.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include "../JSON_parser.hpp"

class MyTest : public CppUnit::TestCase
{
    public:
        std::string json_str =   "{  \n"
                        "    animals: [  \n"
                        "        { name: 'Cat', arr: [2, [5, -2], 8, [1, 2, 3]], height: 1.5},  \n"
                        "        { 'name': Dog, arr: [-4, 3], height: 2.5, width: 0.8 }, \n"
                        "        { name: Cow, arr: [-7, 0, -3], 'height': 4.0, comment: It says moo... },  \n"
                        "        { name: \"Goat\", arr: [1, -8, -9, 4]}  \n"
                        "    ],  \n"
                        "    owner: { name: 'Sam', age: 32, address: '123 Animal Drive, New York, 98765.' },  \n"
                        "    age: 32,\n"
                        "}";
        void testSimpleProperties()
        {
            Resource res(json_str);
            double cat_height = res["animals"][0]["height"].as<double>();
            CPPUNIT_ASSERT_EQUAL(1.5, cat_height);
            std::string owner_address = res["owner"]["address"].as<std::string>();
            std::string adr = "'123 Animal Drive, New York, 98765.'";
            CPPUNIT_ASSERT_EQUAL(adr, owner_address);
            int dog_width = res["animals"][1]["width"].as<int>();
            CPPUNIT_ASSERT_EQUAL(0, dog_width);
        }

        void testArrays()
        {
            Resource res(json_str);
            std::vector<int> goat_arr = res["animals"][3]["arr"].as_vector<int>();
            std::vector<int> exprected_goat_arr{1, -8, -9, 4};
            bool areEqual = std::equal(exprected_goat_arr.begin(), exprected_goat_arr.end(), goat_arr.begin(), goat_arr.end());
            CPPUNIT_ASSERT_EQUAL(true, areEqual);
            std::vector<int> cat_nested_array = res["animals"][0]["arr"][3].as_vector<int>();
            std::vector<int> expected_cat_nested_array{1, 2, 3};
            areEqual = std::equal(expected_cat_nested_array.begin(), expected_cat_nested_array.end(), 
                                  cat_nested_array.begin(), cat_nested_array.end());
            CPPUNIT_ASSERT_EQUAL(true, areEqual);
        }

        void testObjects()
        {
            Resource res(json_str);
            std::unordered_map<std::string, std::string> owner = res["owner"].as_map<std::string>();
            std::string owner_name = "'Sam'";
            std::string owner_age = "32";
            std::string owner_address = "'123 Animal Drive, New York, 98765.'";
            CPPUNIT_ASSERT_EQUAL(owner_name, owner["name"]);
            CPPUNIT_ASSERT_EQUAL(owner_age, owner["age"]);
            CPPUNIT_ASSERT_EQUAL(owner_address, owner["address"]);
        }

        void testAssignment()
        {
            Resource res(json_str);
            CPPUNIT_ASSERT_EQUAL(0, res["animals"][2]["arr"][1].as<int>());
            res["animals"][2]["arr"][1] = 15;
            CPPUNIT_ASSERT_EQUAL(15, res["animals"][2]["arr"][1].as<int>());
            std::vector<std::any> new_arr{1.23, 5.25};
            std::vector<double> new_arr_double{1.23, 5.25};
            CPPUNIT_ASSERT_EQUAL(2, res["animals"][0]["arr"][0].as<int>());
            res["animals"][0]["arr"][0] = new_arr;
            std::vector<double> cat_arr = res["animals"][0]["arr"][0].as_vector<double>();
            bool areEqual = std::equal(new_arr_double.begin(), new_arr_double.end(), cat_arr.begin(), cat_arr.end());
            CPPUNIT_ASSERT_EQUAL(true, areEqual);
        }

        void testFileInput()
        {
            Resource res("json_files/example1.json", true);
            CPPUNIT_ASSERT_EQUAL(2, res["animals"][0]["height"].as<int>());
        }
};

CppUnit::TestSuite* createTestSuite()
{
    CppUnit::TestSuite* suite = new CppUnit::TestSuite("MyTestSuite");
    suite->addTest(new CppUnit::TestCaller<MyTest>("testSimpleProperties", &MyTest::testSimpleProperties));
    suite->addTest(new CppUnit::TestCaller<MyTest>("testArrays", &MyTest::testArrays));
    suite->addTest(new CppUnit::TestCaller<MyTest>("testObjects", &MyTest::testObjects));
    suite->addTest(new CppUnit::TestCaller<MyTest>("testAssignment", &MyTest::testAssignment));
    suite->addTest(new CppUnit::TestCaller<MyTest>("testFileInput", &MyTest::testFileInput));
    return suite;
}

int main()
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(createTestSuite());
    return runner.run() ? 0 : 1;
}