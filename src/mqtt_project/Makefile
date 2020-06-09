CC=g++
CFLAGS=-I. -lsctp
#CFLAGS=

OBJECTS = mqtt_example

all: $(OBJECTS)

$(OBJECTS):%:%.cpp
	@echo Compiling $<  to  $@
	$(CC) -o $@ $< $(CFLAGS)

	
clean:
	rm  $(OBJECTS) 
