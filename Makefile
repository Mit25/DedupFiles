all: tester.cpp md5lib.a
	g++ -pthread -o t tester.cpp md5lib.a

md5lib.a: md5c.o
	ar rcs md5lib.a md5c.o

md5c.o: md5c.cpp
	g++ -c md5c.cpp