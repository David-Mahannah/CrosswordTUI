# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Iinclude -Wall

# Linker flags for libraries
LDFLAGS = -lncurses

# Name of your executable
TARGET = crosswordtui

# Source files directory
SRCDIR = src

# Object files directory
OBJDIR = obj

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# Standard build target
all: $(TARGET)

# Linking the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compiling source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET)

# PHONY targets
.PHONY: all clean

