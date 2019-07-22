all: test-ht example-ht 

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR_A = ${HOME}/src/googletest-release-1.8.0/googletest
GOOGLE_TEST_LIB = ${HOME}/lib/gtest/libgtest.a
GOOGLE_TEST_INCLUDE = ${HOME}/src/googletest-release-1.8.0/googletest/include/

GTEST_DOWNLOAD = ${HOME}/src/googletest-release-1.8.0/

# Where to find user code.
USER_DIR = .
LINKED_LIST_DIR = ../../A1/code/

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
	cp ../release-1.8.0.tar.gz ${HOME}/src/
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

hashtable: Hashtable.c Assert007.c Hashtable_priv.h Hashtable.h LinkedList.h
	@echo ===========================
	@echo Building Hashtable
	@echo ===========================
	gcc -c -g -Wall Hashtable.c 

example-ht.o: Hashtable.c Hashtable.h LinkedList.h
	@echo ===========================
	@echo Building example_ht.o for testing...
	@echo ===========================
	gcc -c -Wall example_ht.c \
		-o example_ht.o

example-ht: Hashtable.c Hashtable.h Hashtable_priv.h LinkedList.h
	gcc -g Hashtable.c example_ht.c Assert007.o LinkedList.o -o example_ht
	@echo Run the example with ./example_ht

test-ht:  $(GOOGLE_TEST_LIB) test_hashtable.o Hashtable.o LinkedList.o Assert007.o
	@echo ===========================
	@echo Building the test suite
	@echo ===========================
	g++ -g -o test_suite test_hashtable.o Hashtable.o LinkedList.o Assert007.o \
		 -L${HOME}/lib/gtest -lgtest -lpthread
	@echo ===========================
	@echo Run tests by running ./test_suite
	@echo ===========================

Hashtable.o: Hashtable.c Hashtable.h Hashtable_priv.h LinkedList.h
	@echo ===========================
	@echo Building Hashtable.o for testing...
	@echo ===========================
	gcc -c -Wall -g Hashtable.c -o Hashtable.o

test_hashtable.o: test_hashtable.cc
	@echo ===========================
	@echo Building test_hashtable.o for testing...
	@echo ===========================
	g++ -c -Wall -I $(GOOGLE_TEST_INCLUDE) test_hashtable.cc \
		-o test_hashtable.o

test_linkedlist.o : test_linkedlist.cc
	@echo ===========================
	@echo Building test_linkedlist.o for testing...
	@echo ===========================
	g++ -c -Wall -I $(GOOGLE_TEST_INCLUDE) test_linkedlist.cc \
		-o test_linkedlist.o

Assert007.o: Assert007.c Assert007.h
	@echo Building Assert007.o for testing....
	gcc -c -Wall -g Assert007.c

.PHONY: clean 

clean:
	rm -f example_ll example_ht test_suite Hashtable.o

