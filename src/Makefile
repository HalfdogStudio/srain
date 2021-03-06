# Makefile
#
# Complie C source codes and output object files to $(BUILD_DIR), then links
# them together to get a executable $(TARGET).

CC = gcc
DEFS = -D PACKAGE='"$(PACKAGE)"' \
	   -D PACKAGE_NAME='"$(PACKAGE_NAME)"' \
	   -D PACKAGE_VERSION='"$(PACKAGE_VERSION)"' \
	   -D PACKAGE_BUILD='"$(PACKAGE_BUILD)"' \
	   -D PACKAGE_AUTHOR='"$(PACKAGE_AUTHOR)"' \
	   -D PACKAGE_EMAIL='"$(PACKAGE_EMAIL)"' \
	   -D PACKAGE_DESC='"$(PACKAGE_DESC)"' \
	   -D PACKAGE_WEBSITE='"$(PACKAGE_WEBSITE)"' \
	   -D PACKAGE_DATA_DIR='"$(PACKAGE_DATA_DIR)"' \
	   -D PACKAGE_CONFIG_DIR='"$(PACKAGE_CONFIG_DIR)"' \
	   -D PACKAGE_COPYRIGHT_DATES='"$(PACKAGE_COPYRIGHT_DATES)"'
CFLAGS = -std=gnu99 -O2 -Wall -I$(PWD)/src/inc -Wno-deprecated-declarations
GTK3FLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK3LIBS = $(shell pkg-config --libs gtk+-3.0)
PY3FLAGS = $(shell pkg-config --cflags python3)
PY3LIBS = $(shell pkg-config --libs python3)
CURLFLAGS = $(shell pkg-config --cflags libcurl)
CURLLIBS = $(shell pkg-config --libs libcurl)
LIBNOTIFYFLAGS = $(shell pkg-config --cflags libnotify)
LIBNOTIFYLIBS = $(shell pkg-config --libs libnotify)
LIBCONFIGFLAGS = $(shell pkg-config --cflags libconfig)
LIBCONFIGLIBS = $(shell pkg-config --libs libconfig)

CFLAGS += $(DEFS) $(GTK3FLAGS)  $(GTK3LIBS) $(PY3FLAGS) $(PY3LIBS) \
		  $(CURLFLAGS) $(CURLLIBS) $(LIBIRCCLINETFLAGS) $(LIBIRCCLINETLIBS) \
		  $(LIBNOTIFYFLAGS) $(LIBNOTIFYLIBS) $(LIBCONFIGFLAGS) $(LIBCONFIGLIBS)

SRCS = $(wildcard *.c */*.c $(BUILD_DIR)/resources.c)
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))

default:
	make $(TARGET)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/%.o: */%.c
	$(CC) $(CFLAGS) -c $^ -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@
