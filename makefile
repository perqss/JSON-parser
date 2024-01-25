CC = g++
CFLAGS = -std=c++20 -g
LDFLAGS = -lcppunit  # Link against CPPUNIT, remove it if you don't want to run the tests
SOURCES = JSON_parser.cpp tests/test_parser.cpp # put the .cpp file with main() in here instead of tests/test_parser.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = out

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

test:
	chmod +x out
	./out

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
