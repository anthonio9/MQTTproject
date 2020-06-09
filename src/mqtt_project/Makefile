CC=g++
LIB=mqtt_base.cpp
CFLAGS=-I. -lsctp -std=c++11 -Wall -Wextra
#CFLAGS=

OBJECTS = mqtt_example

all: $(OBJECTS)

$(OBJECTS):%:%.cpp
	@echo Compiling $<  to  $@
	$(CC) $(LIB) -o $@ $< $(CFLAGS)

	
clean:
	rm  $(OBJECTS) 
