CXX = c++
CXXFLAGS = -std=c++17 -Wall -Wextra

snake: main.cpp
	$(CXX) $(CXXFLAGS) -o logger main.cpp -lz

clean:
	rm -f logger

.PHONY: clean
