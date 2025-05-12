CPP_VERSION=c++23

all: simple

simple: example/simple.cpp include/sqlite.cpp
	g++ -O2 -Iinclude example/simple.cpp include/sqlite.cpp -o example/simple -std=${CPP_VERSION} -lsqlite3
