CXX = g++
CXXFLAGS = -Ofast -mavx -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -DNDEBUG -D_EJUDGE_CLIENT_SIDE -Wno-parentheses
IFLAGS = -I./include -I./lib
OBJDIR = ./obj
SRCDIR = ./src
LIBDIR = ./lib
# SRC = $(SRCDIR)/*.cpp
# OBJ = $(OBJDIR)/*.o $(LIBDIR)/*.o
SRC = $(wildcard $(SRCDIR)/*.cpp) $(LIBDIR)/*.cpp
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
ASM = $(wildcard $(SRCDIR)/*.s)
ASMO = $(patsubst $(SRCDIR)/%.s, $(OBJDIR)/%.o, $(ASM))
CXXFLAGS += $(IFLAGS)

all : $(OBJ) $(ASMO)
	@$(CXX) $(IFLAGS) $(CFLAGS) $^ -o hash_table -mavx -march=haswell

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(IFLAGS) $(CXXFLAGS) -c $^ -o $@ -mavx -march=haswell

$(ASMO) : $(ASM)
	@nasm -g -F dwarf -f elf64 $(ASM) -o $(ASMO)


.PHONY: clean
clean:
	@rm -f $(OBJDIR)/*.o ./python_scripts/*.html ./python_scripts/*.png ./python_scripts/*.data ./*.data ./graphviz/*.dot ./graphviz/*.png *.exe hash_table
