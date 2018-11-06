# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

RESCOMP = windres
INCLUDES += -I../Code -I../Code/Shared -I../Vendors
FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MMD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LIBS +=
LDDEPS +=
ALL_LDFLAGS += $(LDFLAGS) -L../Vendors/bass -L../Vendors/d3d9 -L../Vendors/detours -L../Vendors/lua/lib -L../Vendors/opus/lib -shared -Wl,-soname=SampleMod.so
LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),debug)
TARGETDIR = ../Bin/Debug
TARGET = $(TARGETDIR)/SampleMod.so
OBJDIR = obj/Debug/SampleMod
DEFINES += -DNOMINMAX -DDEBUG -D_DEBUG -DGC_DBG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O0 -fPIC -g -std=c++17
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O0 -fPIC -g -std=c++17

else ifeq ($(config),release)
TARGETDIR = ../Bin/Release
TARGET = $(TARGETDIR)/SampleMod.so
OBJDIR = obj/Release/SampleMod
DEFINES += -DNOMINMAX
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -O3 -fPIC -g -std=c++17
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -O3 -fPIC -g -std=c++17

else
  $(error "invalid configuration $(config)")
endif

# Per File Configurations
# #############################################


# File sets
# #############################################

OBJECTS :=

OBJECTS += $(OBJDIR)/Oakwood.o
OBJECTS += $(OBJDIR)/SampleMod.o

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking SampleMod
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning SampleMod
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/Oakwood.o: ../Code/Shared/Oakwood/Oakwood.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/SampleMod.o: ../Code/SampleMod/SampleMod.cpp
	@echo $(notdir $<)
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif