CURDIR=../
include ../Makefile.vars

CFLAGS += -I../include/

# Binary names of known test cases
TESTS = \
	string_replace \
	
# Build all
all: app tests

# Ensure application library is up to date.
app:
	@$(MAKE) -w -C .. all

# Tests only
tests: $(TESTS)

# Implicit rule of how to make executables
%: %.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) $(TESTBIN_LDFLAGS) -o $@ $<
	
clean:
	$(RM_FILE) $(TESTS)
	
.PHONY: all clean app tests
