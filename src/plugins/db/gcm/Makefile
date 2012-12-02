TOPDIR=../../../..
include $(TOPDIR)/Makefile.vars

CXXFLAGS+=-I../../../../include

OBJS=\
	main.o \
	GCMDatabase.o

$(TARGET)/plugins/db/gcm$(PLUGINEXTENSION): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PLUGIN_LDFLAGS) $^ -o $@

clean:
	$(RM_FILE) $(OBJS)

.PHONY: clean
