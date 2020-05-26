# $ make

CXX = g++
SRCDIR = src
BUILDDIR = build
TARGET = bin/SwarmMusic

SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o)) $(BUILDDIR)/glad.o
CXXFLAGS = -std=c++14 -O3 -fomit-frame-pointer -march=native -m64 -Wall -pipe -DFX -DXMESA -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__
# LIB = -lGL -lGLU -lglut -lX11 -lm -lAntTweakBar -lrt -lm -lasound -L lib -L rtmidi -pthread -lportaudio -lrtmidi

LIB = -lGL -lGLU -lglut -lX11 -lm -lasound -L lib -pthread -lstk -lglfw3 $(shell pkg-config --static --libs x11 xrandr xi xxf86vm glfw3)

INC = -I include -I ext


all: glad bin/SwarmMusic

$(TARGET): $(OBJECTS) stk-4.6.0/src/Release/Plucked.o stk-4.6.0/src/Release/RtAudio.o stk-4.6.0/src/Release/Skini.o
	@echo "Linking objects......"
	g++ -Iinclude/ -Iext/ -c -o $(BUILDDIR)/glad.o $(SRCDIR)/glad.c -lglfw -ldl
	@echo "$(CXX) $^ -o $(TARGET) $(LIB)"; $(CXX) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo "$(CXX) $(CXXFLAGS) $(INC)-c -o $@ $<"; $(CXX) $(CXXFLAGS) $(INC) -c -o $@ $<

glad: $(SRCDIR)/glad.c
	@mkdir -p $(BUILDDIR)
	@echo "g++ -Iext/ -c -o $(BUILDDIR)/glad.o $(SRCDIR)/glad.c -lglfw -ldl"; g++ -Iext/ -c -o $(BUILDDIR)/glad.o $(SRCDIR)/glad.c -lglfw -ldl

clean:
	@echo "Cleaning up this mess......";
	@echo "$(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

.PHONY: clean
