TOPDIR=../../../..
include $(TOPDIR)/Makefile.vars

$(TARGET)/plugins/ui/qt4$(PLUGINEXTENSION): phony
ifeq ($(OS), Windows_NT)
	cd src && $(QMAKE_BIN) src.pro -spec win32-g++ -r DLLDESTDIR=$(dir $@) -r TARGET=..\$(notdir $(basename $@)) -r "LIBS=$(LDFLAGS) $(PLUGIN_LDFLAGS)" && make -w
else
	cd src && $(QMAKE_BIN) src.pro -spec linux-g++ -r CONFIG+=debug -r TARGET=../$(basename $@) && make -w
endif

phony:

clean:
	$(MAKE) -w -C src clean

.PHONY: clean phony
