all: jini-test

jini-test: jini.cpp jini.h
	g++ -o jini-test jini.cpp -Wall -pedantic -O2 -DJINI_TEST_MAIN
	@echo ">>> Type ./jini-test to run the jini.cpp unit tests."

clean:
	rm -f jini-test.exe