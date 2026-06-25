CC = cc

CFLAGS  = -O3 -Wall -Wextra -fPIC -O3
TEST_CFLAGS  = -g -Wall -Wextra
LDFLAGS =

NAME    = libsaguaro

UNAME_S = $(shell uname -s)

# respect traditional UNIX path usage
INCDIR  = /usr/local/include
LIBDIR  = /usr/local/lib

ifeq ($(UNAME_S),Darwin)
$(NAME).dylib: clean
	$(CC) -c -dynamiclib -o $@ saguaro.c $(CFLAGS) $(LDFLAGS)
else
$(NAME).so: clean
	$(CC) -shared -o $@ saguaro.c $(CFLAGS) $(LDFLAGS)
endif

.PHONY: install
install: 
	cp saguaro.h $(INCDIR)
ifeq ($(UNAME_S),Darwin)
	cp $(NAME).dylib $(LIBDIR)
else
	cp $(NAME).so $(LIBDIR)
endif

uninstall:
	rm -f $(INCDIR)/saguaro.h
ifeq ($(UNAME_S),Darwin)
	rm -f $(LIBDIR)/$(NAME).dylib
else
	rm -f $(LIBDIR)/$(NAME).so
endif

.PHONY: tests
tests: clean
	$(CC) -g -o tests/tests saguaro.c tests/tests.c $(TEST_CFLAGS) $(LDFLAGS) -lcrosscheck
	tests/tests

.PHONY: valgrind
valgrind: tests
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck ./tests/tests

.PHONY: clean
clean:
	rm -f $(NAME).dylib
	rm -f $(NAME).so
	rm -f example
	rm -f tests/tests

.PHONY: example
example: clean
	$(CC) -g -o $@ saguaro.c example.c $(CFLAGS) $(LDFLAGS)
