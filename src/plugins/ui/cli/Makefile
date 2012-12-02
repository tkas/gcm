TOPDIR=../../../..
include $(TOPDIR)/Makefile.vars

OBJS=\
	main.o \
	CLIUI.o \
	CLIDialog.o \
	CLIProgressDialog.o \
	CLIDialogBuilder.o \
	ImportTask.o \
	ExportTask.o

CXXFLAGS+=-I../../../../include

$(TARGET)/plugins/ui/cli$(PLUGINEXTENSION): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(PLUGIN_LDFLAGS) $^ -o $@

clean:
	$(RM_FILE) $(OBJS)

.PHONY: clean
