#
# Makefile for the Linux platform
#
# @author Michal Kuchta
# @version 1.0
#

TOPDIR=$(CURDIR)

include Makefile.vars

COREOBJS += \
	src/core/libgcm.o \
	src/core/util/path.o \
	src/core/util/Log.o \
	src/core/util/gzstream.o \
	src/core/Object.o \
	src/core/Runnable.o \
	src/core/Thread.o \
	src/core/pluginmanager/PluginManager.o \
	src/core/pluginmanager/Plugin.o \
	src/core/geolib/GenericWaypoint.o \
	src/core/geolib/Geocache.o \
	src/core/geolib/GeocacheWaypoint.o \
	src/core/geolib/GeocacheList.o \
	src/core/geolib/Log.o \
	src/core/geolib/LogList.o \
	src/core/geolib/Attribute.o \
	src/core/geolib/AttributeSet.o \
	src/core/geolib/Filter.o \
	src/core/geolib/filter/Advanced.o \
	src/core/geolib/filter/MatchAll.o \
	src/core/geolib/filter/Combined.o \
	src/core/geolib/filter/CacheName.o \
	src/core/geolib/filter/Difficulty.o \
	src/core/geolib/filter/Distance.o \
	src/core/geolib/filter/Found.o \
	src/core/geolib/filter/GeocacheType.o \
	src/core/geolib/filter/HasCorrectedOrFinal.o \
	src/core/geolib/filter/DoesNotHaveCorrectedOrFinal.o \
	src/core/geolib/filter/MyOwn.o \
	src/core/geolib/filter/MyNotOwn.o \
	src/core/geolib/filter/NotFound.o \
	src/core/geolib/filter/PlacedBy.o \
	src/core/geolib/filter/Size.o \
	src/core/geolib/filter/StatusArchived.o \
	src/core/geolib/filter/StatusAvailable.o \
	src/core/geolib/filter/StatusDisabled.o \
	src/core/geolib/filter/Terrain.o \
	src/core/geolib/filter/FilterIDs.o \
	src/core/geolib/filter/WaypointType.o \
	src/core/geolib/RegisterGeolibFuncs.o \
	src/core/geolib/ReferencePoint.o \
	src/core/ui/UI.o \
	src/core/ui/DialogBuilder.o \
	src/core/ui/Dialog.o \
	src/core/ui/BackgroundTask.o \
	src/core/ui/Command.o \
	src/core/util/File.o \
	src/core/stl/String.o \
	src/core/config/Configuration.o \
	src/core/config/AppConfig.o \
	src/core/sqlite/Database.o \
	src/core/sqlite/Stmt.o \
	src/core/templater/Templater.o \
	src/core/templater/Variable.o \
	src/core/templater/Library.o \
	src/core/expat/xmlparse.o \
	src/core/expat/xmlrole.o \
	src/core/expat/xmltok.o \

APPS = \
	$(TARGET)/gcm-cli$(EXTENSION) \
	$(TARGET)/gcm$(EXTENSION)

PLUGINS = \
	ui/cli \
	ui/qt4 \
	db/gcm \
	db/geoget \
	import/pqgpx \
	export/pqgpx

PLUGIN_FILES = $(addsuffix $(PLUGINEXTENSION), $(addprefix $(TARGET)/plugins/, $(PLUGINS)))

LIBGCM = $(TARGET)/libgcm$(PLUGINEXTENSION)

%/.:
	mkdir -p $@

all: $(TARGET)/. $(LIBGCM) $(APPS) $(PLUGIN_FILES)

# LibGCM
$(LIBGCM): CXXFLAGS+=-DLIBGCM
$(LIBGCM): $(COREOBJS)
	$(CXX) $(CXXFLAGS) $(XML2_LDFLAGS) -shared $^ $(LIBGCM_LDFLAGS) $(LDFLAGS) -o $@

# Applications
$(TARGET)/gcm-cli$(EXTENSION): CXXFLAGS+=-DGCM_CORE
$(TARGET)/gcm-cli$(EXTENSION): src/gcm-cli.o src/core/main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(BIN_LDFLAGS) $(CLI_LDFLAGS) $^ -o $@

$(TARGET)/gcm$(EXTENSION): CXXFLAGS+=-DGCM_CORE
$(TARGET)/gcm$(EXTENSION): src/gcm.o src/core/main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(BIN_LDFLAGS) $(GUI_LDFLAGS) $^ -o $@

src/core/expat/xmltok.o: CXXFLAGS+=-Wno-missing-field-initializers

# Full build
full: clean all

run: all
	cd build && ./gcm

# Plugins
$(PLUGIN_FILES):
	$(MAKEPATH) $(@D)
	$(MAKE) -C $(subst $(PLUGINEXTENSION),,$(subst $(TARGET),src,$@))

clean:
	$(RM_FILE) $(COREOBJS)
	$(RM_FILE) build/gcm-cli$(EXTENSION) build/gcm$(EXTENSION) build/libgcm$(PLUGINEXTENSION)
	$(RM_DIR) build/plugins
	@$(MAKE) -w -I $(TARGET)/.. -C tests clean

	for plugin in $(subst $(PLUGINEXTENSION),,$(subst $(TARGET),src,$(PLUGIN_FILES))); do $(MAKE) -w -C $$plugin clean; done;

tests:
	@$(MAKE) -w -C tests/ tests

.PHONY: all clean full tests $(PLUGIN_FILES)
.EXPORT_ALL_VARIABLES:
