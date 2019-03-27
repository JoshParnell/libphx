NPROC := $(shell nproc)
NJOBS := $(shell echo $$(( $(NPROC) * 2 )))
MAKEFLAGS = -j$(NJOBS)

CC := clang++
LINK := clang++
PROJECT := libphx

SRCPATH := src
OUTFILE := $(PROJECT)64.so
OBJPATH = ./obj
OUTPATH = ./bin/$(OUTFILE)

SOURCES := $(shell find $(SRCPATH)/ -type f -name '*.cpp')
OBJECTS := $(patsubst $(SRCPATH)/%.cpp, $(OBJPATH)/%.o, $(SOURCES))
DEPENDS := $(patsubst $(SRCPATH)/%.cpp, $(OBJPATH)/%.d, $(SOURCES))

LIBS := -lGL
LIBS += -lGLEW
LIBS += -ldl
LIBS += -lfreetype
LIBS += -lluajit-5.1
LIBS += -llz4
LIBS += -lfmod
LIBS += -lfmodstudio
LIBS += `sdl2-config --libs`

CFLAGS = `sdl2-config --cflags`
CFLAGS += -Wall -Wformat
CFLAGS += -fno-exceptions
CFLAGS += -ffast-math
CFLAGS += -fpic
CFLAGS += -O3 -msse -msse2 -msse3 -msse4
CFLAGS += -DDEBUG=0
CFLAGS += -g
CFLAGS += -Iinclude/
CFLAGS += -Iext/
CFLAGS += -Iext/bullet/
CFLAGS += -std=c++11
CFLAGS += -Wno-unused-variable
CFLAGS += -Wno-unknown-pragmas

LFLAGS := -Wl,-rpath /usr/local/lib
LFLAGS += -L/usr/lib
LFLAGS += -L/usr/local/lib
LFLAGS += -rdynamic
LFLAGS += -shared

all: $(OUTPATH)
	@echo Build complete.

bindings:
	luajit ./tool/genffi.lua include $(PROJECT) ./script

clean:
	rm -f $(OUTPATH)
	rm -rf $(OBJPATH)

#install: $(OUTPATH)
#  luajit ../tool/genffi.lua include $(PROJECT) ../script
#  cp $(OUTPATH) /usr/local/lib/$(OUTFILE)
#  rm -rf ../ext/include/$(PROJECT)
#  cp -r include ../ext/include/$(PROJECT)
#  ldconfig

$(OUTPATH): $(OBJECTS)
	@echo [LINK] $(OUTPATH)
	@$(LINK) -o $(OUTPATH) $(OBJECTS) $(CFLAGS) $(LFLAGS) $(LIBS)

$(OBJPATH)/%.o: $(SRCPATH)/%.cpp
	@mkdir -p $(OBJPATH)
	@echo [CC] $<
	@$(CC) -MD -MP $(CFLAGS) -o $@ -c $<

-include $(DEPENDS)
