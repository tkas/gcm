TOPDIR=../../../..
include $(TOPDIR)/Makefile.vars

CXXFLAGS+=-I../../../../include

OBJS=\
	main.o

$(TARGET)/plugins/import/pqgpx$(PLUGINEXTENSION): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PLUGIN_LDFLAGS) $^ -o $@

clean:
	$(RM_FILE) $(OBJS)

.PHONY: clean
