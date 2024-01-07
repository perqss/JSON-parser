all:
	g++ -std=c++20 tests/test_parser.cpp -lcppunit -o test_parser
test:
	chmod +x test_parser
	./test_parser
clean:
	${RM} test_parser