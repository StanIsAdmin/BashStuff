# Makefile adapted from:
# https://sites.google.com/site/michaelsafyan/software-engineering/how-to-write-a-makefile


# - - - - - VARIABLES - - - - -

# CPPFLAGS is the C preprocessor flags, so anything that compiles a C or C++ source file into an object file will use these flags
CPPFLAGS += -std=c++14 -Wpedantic -Wall -Wextra -Winit-self -Winline -Wconversion -Weffc++ -Wstrict-null-sentinel -Wold-style-cast -Wnoexcept -Wctor-dtor-privacy -Woverloaded-virtual -Wconversion -Wsign-promo -Wzero-as-null-pointer-constant 

# LDFLAGS are used when linking
LDFLAGS += 

# Name of the program
program_NAME := bin/polynomial

# List of all files in the src/ directory ending in ".c"
program_C_SRCS := src/$(wildcard *.c)

# List of all files in the src/ directory ending in ".cpp"
program_CXX_SRCS := src/$(wildcard *.cpp)

# Names all C object files that we are going to build (substitution expression)
program_C_OBJS := ${program_C_SRCS:src/=bin/:.c=.o}

# Names all C++ object files that we are going to build
program_CXX_OBJS := ${program_CXX_SRCS:src/=bin/:.cpp=.o}

# L of all the ".o" files, both from C and C++ source files
program_OBJS := $(program_C_OBJS) $(program_CXX_OBJS)

# Headers in these directories will be found with #include <>
program_INCLUDE_DIRS := ./include

# Libraries in these directories will be found by the linker
program_LIBRARY_DIRS :=

# Libraries with these names will be linked in
program_LIBRARIES :=


# Add I$(includedir) for every include directory given in $(program_INCLUDE_DIRS)
CPPFLAGS += $(foreach includedir,$(program_INCLUDE_DIRS),-I$(includedir))

# Add -L$(librarydir) for every library directory given in $(program_LIBRARY_DIRS)
LDFLAGS += $(foreach librarydir,$(program_LIBRARY_DIRS),-L$(librarydir))

# Add -l$(library) for every library given in $(program_LIBRARIES)
LDFLAGS += $(foreach library,$(program_LIBRARIES),-l$(library))


# - - - - - COMPILATION - - - - -

# Phony targets execute their build rules, even if a file with the same name exists
.PHONY: all clean distclean


# First build rule in the makefile is the default (when executing "make")
all: $(program_NAME)


# The program depends on the object files
# The build rule $(LINK.cc) is used to link the object files and output a file with the same name as the program. LINK.cc makes use of CXX,CXXFLAGS, LDFLAGS, etc.
$(program_NAME): $(program_OBJS)
    $(LINK.cc) $(program_OBJS) -o $(program_NAME)


# This target removes the built program and the generated object files. 
# The @ symbol indicates that the line should be run silently, and the - symbol indicates that errors should be ignored
clean:
    @- $(RM) $(program_NAME)
    @- $(RM) $(program_OBJS)


# The distclean target depends on the clean target (so executing distclean will cause clean to be executed), but it also removes configuration files
distclean: clean