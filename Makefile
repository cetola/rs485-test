sources = read-demo.c write-demo.c
CFLAGS=-Wall
APP_VERSION = "1.0.0"

all: read-demo write-demo

read-demo:
	        $(CC) -DAPP_VERSION='"$(APP_VERSION)"' $(CFLAGS) $(INC) $(LDFLAGS) -o $@ read-demo.c

write-demo:
	        $(CC) -DAPP_VERSION='"$(APP_VERSION)"' $(CFLAGS) $(INC) $(LDFLAGS) -o $@ write-demo.c

clean:
	        $(RM) read-demo write-demo

.PHONY: all clean
