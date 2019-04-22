NPROC := $(shell nproc)
NJOBS := $(shell echo $$(( $(NPROC) * 2 )))
MAKEFLAGS = -j$(NJOBS)

CC := clang++
LINK := clang++
PROJECT := libphx

SRCPATH := src
OBJPATH := ./obj
OUTDIR	:= ./bin
OUTFILE := $(PROJECT)64.so
OUTPATH := $(OUTDIR)/$(OUTFILE)

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
LIBS += -lBulletCollision
LIBS += -lBulletDynamics
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

.PHONY: all bindings clean install test

all: $(OUTPATH) bindings
	@echo Build complete.

bindings: $(OUTPATH)
	luajit ./tool/genffi.lua include $(PROJECT) ./script/ffi

clean:
	rm -f $(OUTPATH)
	rm -rf $(OBJPATH)
	rm -rf log

install: all
	sudo cp $(OUTPATH) /usr/local/lib/$(OUTFILE)
	sudo ldconfig

test:
	@./bin/luajit ./script/test/TestInit.lua

$(OUTPATH): $(OBJECTS)
	@mkdir -p $(OUTDIR)
	@echo [LINK] $(OUTPATH)
	@$(LINK) -o $(OUTPATH) $(OBJECTS) $(CFLAGS) $(LFLAGS) $(LIBS)

$(OBJPATH)/%.o: $(SRCPATH)/%.cpp
	@mkdir -p $(OBJPATH)
	@echo [CC] $<
	@$(CC) -MD -MP $(CFLAGS) -o $@ -c $<

-include $(DEPENDS)
