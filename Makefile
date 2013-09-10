# Define the compiler options
CXXFLAGS  = -pipe -O2 -Wall -W
CXXFLAGS += -Wmissing-braces -Wparentheses -Wold-style-cast -pthread -Iinclude

# Define what to do when make is executed without arguments.
all: quantjs
	g++ -o quantjs -pthread main.o libv8.a  -L. -lv8
quantjs: main.o

# Define the rules for the object files.
main.o: main.cc
	g++ -c $(CXXFLAGS) main.cc

clean:
	rm -rf *o quantjs
		
run:
	./quantjs
		
runa:
	./quantjs
