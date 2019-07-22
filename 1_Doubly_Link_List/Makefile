all: hello example test

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR_A = ${HOME}/src/googletest-release-1.8.0/googletest
GOOGLE_TEST_LIB = ${HOME}/lib/gtest/libgtest.a
GOOGLE_TEST_INCLUDE = ${HOME}/src/googletest-release-1.8.0/googletest/include/

GTEST_DOWNLOAD = ${HOME}/src/googletest-release-1.8.0/

# Where to find user code.
USER_DIR = .

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR_A)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR_A)/include/gtest/*.h \
                $(GTEST_DIR_A)/include/gtest/internal/*.h

# Builds gtest.a

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR_A)/src/*.cc $(GTEST_DIR_A)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.

release-1.8.0.tar.gz : 
	@echo ----------------
	@echo NEED GTEST ZIP
	@echo ----------------

gtest-src :  | release-1.8.0.tar.gz
	@echo ===========================
	@echo Extracting gtest source 
	@echo ===========================
	mkdir -p ${HOME}/src
	cp release-1.8.0.tar.gz ${HOME}/src/
	tar -xzf ${HOME}/src/release-1.8.0.tar.gz -C ${HOME}/src/

gtest-all.o : |gtest-src
	@echo ===========================
	@echo Building gtest-all.o
	@echo ===========================
	@echo GTEST_DIR: ${GTEST_DIR_A}
	g++ -isystem ${GTEST_DIR_A}/include -I${GTEST_DIR_A} -pthread -c ${GTEST_DIR_A}/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

$(GOOGLE_TEST_LIB) : gtest-all.o
	@echo ===========================
	@echo Building the GTest library
	@echo ===========================
	ar -rv libgtest.a gtest-all.o
	mkdir -p ~/lib/gtest/
	mv libgtest.a ~/lib/gtest/


lib: LinkedList.h LinkedList_priv.h LinkedList.c Assert007.c Assert007.h
	@echo ===========================
	@echo Building LinkedList
	@echo ===========================
	echo Compiling the LinkedList implementation
	gcc -g LinkedList.c Assert007.c

example:
	@echo ===========================
	@echo Building the Example app 
	@echo ===========================
	gcc -g LinkedList.c Assert007.c example_program_ll.c -o example_ll
	@echo ===========================
	@echo Run the example with ./example_ll
	@echo ===========================

test: $(GOOGLE_TEST_LIB) test_linkedlist.o LinkedList.o Assert007.o
	@echo ===========================
	@echo Building the test suite
	@echo ===========================
	g++ -o test_suite test_linkedlist.o LinkedList.o Assert007.o \
		 -L${HOME}/lib/gtest -lgtest -lpthread
	@echo ===========================
	@echo Run tests by running ./test_suite
	@echo ===========================

LinkedList.o: LinkedList.c LinkedList.h 
	@echo ===========================
	@echo Building LinkedList.o for testing...
	@echo ===========================
	gcc -c -Wall -g LinkedList.c -o LinkedList.o

test_linkedlist.o : test_linkedlist.cc
	@echo ===========================
	@echo Building test_linkedlist.o for testing...
	@echo ===========================
	g++ -c -Wall -I $(GOOGLE_TEST_INCLUDE) test_linkedlist.cc \
		-o test_linkedlist.o

Assert007.o: Assert007.c Assert007.h
	@echo ==========================
	@echo Building Assert007.o...
	@echo ==========================
	gcc -c -Wall -g Assert007.c -o Assert007.o

hello: hello_world.c hello_world.h
	gcc -g -o hello_world hello_world.c

.PHONY: clean 

clean:
	rm -f hello_world example_ll test_suite *.o

info:
	./system_info.sh
