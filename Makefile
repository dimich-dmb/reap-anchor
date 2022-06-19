CFLAGS = -Wall -Werror -O2

.PHONY: all clean

all: reap-anchor

clean:
	$(RM) reap-anchor
