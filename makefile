CC = g++
CFLAGS = -std=c++20 -g
LDFLAGS = -lcppunit  # Link against CPPUNIT
SOURCES = JSON_parser.cpp tests/test_parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = test_parser

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

test:
	chmod +x test_parser
	./test_parser

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
