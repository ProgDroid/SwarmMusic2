# $ make

CXX = g++
SRCDIR = src
BUILDDIR = build
TARGET = bin/SwarmMusic

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CXXFLAGS = -std=c++14 -O3 -fomit-frame-pointer -march=native -m64 -Wall -pipe -DFX -DXMESA -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__
# LIB = -lGL -lGLU -lglut -lX11 -lm -lAntTweakBar -lrt -lm -lasound -L lib -L rtmidi -pthread -lportaudio -lrtmidi

LIB = -lGL -lGLU -lglut -lX11 -lm -lAntTweakBar -lasound -L lib -pthread -lstk -lglfw3 $(shell pkg-config --static --libs x11 xrandr xi xxf86vm glfw3)

# INC = -I include -I AntTweakBar -I portAudio -I rtmidi

INC = -I include -I AntTweakBar

$(TARGET): $(OBJECTS) stk-4.6.0/src/Release/*.o
	@echo "Linking objects......"
	@echo "$(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<"; $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning up this mess......";
	@echo "$(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
