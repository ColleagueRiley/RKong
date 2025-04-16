CC = gcc

LIBS :=-lgdi32
EXT = .exe
STATIC =

WARNINGS = -Wall -Werror -Wextra
OS_DIR = \\

SRC = source/*.c
OUTPUT = rkong

ifneq (,$(filter $(CC),winegcc x86_64-w64-mingw32-gcc i686-w64-mingw32-gcc))
	STATIC = --static
    detected_OS := WindowsCross
	OS_DIR = /
	ifeq ($(CC),x86_64-w64-mingw32-gcc)
		CC = x86_64-w64-mingw32-gcc
	else
		CC = i686-w64-mingw32-gcc
	endif
else
	ifeq '$(findstring ;,$(PATH))' ';'
		detected_OS := Windows
	else
		detected_OS := $(shell uname 2>/dev/null || echo Unknown)
		detected_OS := $(patsubst CYGWIN%,Cygwin,$(detected_OS))
		detected_OS := $(patsubst MSYS%,MSYS,$(detected_OS))
		detected_OS := $(patsubst MINGW%,MSYS,$(detected_OS))
	endif
endif

ifeq ($(detected_OS),Windows)
	LIBS := -lgdi32 
	EXT = .exe
	OS_DIR = \\
	LIBS += -mwindows
endif
ifeq ($(detected_OS),Darwin)        # Mac OS X
	LIBS := -lm -framework Cocoa -framework IOKit $(STATIC)  -framework CoreVideo
	EXT = 
	WARNINGS = 
	OS_DIR = /
endif
ifeq ($(detected_OS),Linux)
    LIBS := -lXrandr -lX11 -lm $(STATIC)
	EXT =
	OS_DIR = /
endif

ifneq (,$(filter $(CC),cl))
	OS_DIR = \\
endif

ifneq (,$(filter $(CC),/opt/msvc/bin/x64/cl.exe /opt/msvc/bin/x86/cl.exe))
	OS_DIR = /
endif

LINK_GL1 = 
LINK_GL3 =
LINK_GL2 = 

ifneq (,$(filter $(CC),emcc))
	LINK_GL1 = -s LEGACY_GL_EMULATION -D LEGACY_GL_EMULATION -sGL_UNSAFE_OPTS=0
	LINK_GL3 = -s FULL_ES3 
	LINK_GL2 = -s FULL_ES2	
	EXPORTED_JS = -s EXPORTED_RUNTIME_METHODS="['stringToNewUTF8']" 
	LIBS = -s WASM=1 -s ASYNCIFY -s USE_WEBGL2=1 -s GL_SUPPORT_EXPLICIT_SWAP_CONTROL=1 $(EXPORTED_JS)
	LIBS += -s EXPORTED_FUNCTIONS="['_malloc', '_main']" 
	EXT = .js
	CC=emcc

	LIBS += --preload-file ./
endif

LIBS += -I./include -lm
OUTPUT_ARG = -o $(OUTPUT)$(EXT)

ifneq (,$(filter $(CC),cl /opt/msvc/bin/x64/cl.exe /opt/msvc/bin/x86/cl.exe))
	WARNINGS =
	OUTPUT_ARG =
	STATIC = /Fe:$(OUTPUT) /static /I include
	LIBS = $(STATIC) /link /SUBSYSTEM:WINDOWS
	EXT = .exe
endif

all: $(SRC) 
	$(CC) $(STATIC) $(SRC)  $(LINK_GL1) $(LIBS) $(OUTPUT_ARG)

clean:
	rm -f *.exe $(OUTPUT) *.o *.obj 

debug: $(SRC) 
	$(CC) $(STATIC) $(SRC) $(LINK_GL1) $(LIBS) -D RGFW_DEBUG $(OUTPUT_ARG)
ifeq (,$(filter $(CC),emcc))
	.$(OS_DIR)$(OUTPUT)$(EXT)
endif
