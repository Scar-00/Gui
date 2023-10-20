CC = clang
AR = ar -rcs
CFLAGS = -std=c11 -O3 -g -Wall -Wextra -Wpedantic -Wstrict-aliasing -Wno-newline-eof -Wno-deprecated-declarations -Wno-unused-parameter -DDLL_BUILD
CFLAGS += -Iinclude
CFLAGS += -Ilib/cglm/include
CFLAGS += -Ilib/stb
CFLAGS += -Ilib/gaia/include
CFLAGS += -Ilib/freetype
CFLAGS += -Ilib/glad/include
CFLAGS += -Ilib/glfw/include

LDFLAGS += lib/gaia/libgaia.lib
LDFLAGS += lib/freetype/freetype.lib
LDFLAGS += lib/glad/src/glad.o
LDFLAGS += lib/glfw/glfw3dll.lib

CP = -MJ $@.json

SRC = $(wildcard src/**/**/*.c) $(wildcard src/**/*.c) $(wildcard src/*.c) #$(wildcard include/Gaia/**/**/*.h) $(wildcard include/Gaia/**/*.h) $(wildcard include/Gaia/*.h)
OBJ = $(SRC:.c=.o)
DATA = $(wildcard src/**/**/*.o.json) $(wildcard src/**/*.o.json) $(wildcard src/*.o.json)

libs:
	cd gaia & make static
	$(CC) -c lib/glad/src/glad.c -o lib/glad/src/glad.o -Ilib/glad/include

static: $(OBJ)
	$(AR) libgui.a $^ $(LDFLAGS)

dynamic: $(OBJ)
	$(CC) -shared -o gui.dll $^ $(LDFLAGS)

data: $(OBJ)
	sed -e '1 s/./[ '\\\n'{/' -e '$$ s/,$$/'\\\n'] /' $(DATA) > compile_commands.json

%.o: %.c
	$(CC) -o $@ -c $< -MJ $@.json $(CFLAGS)

clean:
	rm -rf $(OBJ) libgui.a gui.dll gui.lib gui.exp
