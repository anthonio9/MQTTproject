CC=gcc
CFLAGS=-I. -lsctp
#CFLAGS=

OBJECTS = sctp_srv sctp_cli base_sctp

all: $(OBJECTS)

$(OBJECTS):%:%.cpp
	@echo Compiling $<  to  $@
	$(CC) -o $@ $< $(CFLAGS)

	
clean:
	rm  $(OBJECTS) 
