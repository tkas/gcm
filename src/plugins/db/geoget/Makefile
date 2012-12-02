TOPDIR=../../../..
include $(TOPDIR)/Makefile.vars

CXXFLAGS+=-I../../../../include
LDFLAGS+=-lz

OBJS=\
	main.o \
	GGDatabase.o \
#	MurmurHash3.o

$(TARGET)/plugins/db/geoget$(PLUGINEXTENSION): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PLUGIN_LDFLAGS) $^ -o $@

clean:
	$(RM_FILE) $(OBJS)

.PHONY: clean
