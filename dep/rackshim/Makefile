# Build rackshim

# Remember to: export RACK_DIR=/path/to/Rack (the Rack sourcetree)
ifndef RACK_DIR
$(error RACK_DIR is not defined)
endif

# Add .cpp and .c files to the build
SOURCES += $(wildcard src/*.cpp)

# Include Rack includes
FLAGS += -I$(RACK_DIR)/include -I$(RACK_DIR)/dep/include

# Generate dependency files alongside the object files
FLAGS += -MMD -MP
FLAGS += -g

# Optimization
FLAGS += -O3 -march=nocona -ffast-math -fno-finite-math-only
FLAGS += -Wall -Wextra -Wno-unused-parameter

# Detect architecture if ARCH is not defined
ifndef ARCH
MACHINE = $(shell gcc -dumpmachine)
ifneq (,$(findstring linux,$(MACHINE)))
	# Linux
	ARCH = lin
else ifneq (,$(findstring apple,$(MACHINE)))
	# Mac
	ARCH = mac
else ifneq (,$(findstring mingw,$(MACHINE)))
	# Windows
	ARCH = win
else
	$(error Could not determine machine type)
endif
endif

ifeq ($(ARCH), lin)
	TARGET := rackshim
	FLAGS += -DARCH_LIN
	LDFLAGS += -rdynamic \
		$(shell pkg-config --libs gtk+-2.0) \
		-lGLEW -lglfw -ljansson -lspeexdsp -lcurl -lzip -lrtaudio -lrtmidi -lcrypto -lssl
endif

ifeq ($(ARCH), mac)
	TARGET := rackshim
	FLAGS += -DARCH_MAC
	CXXFLAGS += -stdlib=libc++
	MAC_SDK_FLAGS = -mmacosx-version-min=10.7
	FLAGS += $(MAC_SDK_FLAGS)
	LDFLAGS += -stdlib=libc++ $(MAC_SDK_FLAGS) \
		-framework Cocoa -framework OpenGL \
		-lGLEW -lglfw -ljansson -lspeexdsp -lcurl -lzip -lrtaudio -lrtmidi -lcrypto -lssl
endif

ifeq ($(ARCH), win)
	TARGET := rackshim.exe
	FLAGS += -DARCH_WIN -D_USE_MATH_DEFINES
	LDFLAGS += -L$(RACK_DIR) -lRack \
		-static-libgcc -static-libstdc++ -lpthread -lws2_32 \
		-Wl,--export-all-symbols,--out-implib,libRack.a -mwindows \
		-lgdi32 -lopengl32 -lcomdlg32 -lole32 \
		-lglew32 -lglfw3dll -lcurl -lzip -lrtaudio -lrtmidi -lcrypto -lssl \
		-Wl,-Bstatic -ljansson -lspeexdsp
endif

# Link against dependencies built by Rack
LDFLAGS += -L$(RACK_DIR)/dep/lib

# The install location for `make install`
DEP_LOCAL ?= .

# Commands
ifeq ($(ARCH), win)
	CMAKE := cmake -G 'MSYS Makefiles' -DCMAKE_INSTALL_PREFIX="$(realpath $(DEP_LOCAL))"
else
	CMAKE := cmake -DCMAKE_INSTALL_PREFIX="$(realpath $(DEP_LOCAL))"
endif

ifneq ($(ARCH), mac)
	CXXFLAGS += -Wsuggest-override
endif

CXXFLAGS += -std=c++11 $(FLAGS)

# Derive object files from sources and place them before user-defined objects
SOURCE_OBJECTS := $(patsubst %, build/%.o, $(SOURCES))
DEPENDENCIES := $(patsubst %, build/%.d, $(SOURCES))

# Link in all Rack object files, except for main.cpp.o
# TODO: Trim to list of actually needed objects (unfortunately probably most)
RACK_OBJECTS = $(filter-out $(wildcard main.cpp.*), $(wildcard $(RACK_DIR)/build/src/*.o $(RACK_DIR)/build/src/*/*.o))
RACK_OBJECTS += $(wildcard $(RACK_DIR)/build/dep/nanovg/src/*.o $(RACK_DIR)/build/dep/osdialog/*.o)

# Final targets
$(TARGET): $(SOURCE_OBJECTS) $(OBJECTS) $(RACK_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(DEPENDENCIES)

build/%.cpp.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(TARGET)

clean:
	rm -rfv build $(TARGET) dist

dist: all
	rm -rf dist
	mkdir -p dist
	# Strip and copy binary
	cp $(TARGET) dist/
ifeq ($(ARCH), mac)
	strip -x dist/$(TARGET)
else
	strip -s dist/$(TARGET)
endif

.DEFAULT_GOAL := all
