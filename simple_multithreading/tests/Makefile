CXX = /usr/bin/c++

CPPFLAGS = -Wall -isystem $(GTEST_DIR)/include
LDFLAGS = -L /usr/lib/ -lpthread -lgtest -lgmock

all:
	$(CXX) -o run_tests $(CPPFLAGS) ../product.cpp ../shop.cpp ./tests.cpp $(LDFLAGS)
