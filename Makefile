CC=clang++
C11=-std=c++11
FLAGSBASE= -O3 -W -Wextra -Wcast-qual -Wcast-align -Wfloat-equal -Wshadow -Wpointer-arith -Wunreachable-code -Wchar-subscripts -Wcomment -Wformat -Werror-implicit-function-declaration -Wmain -Wmissing-braces -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wuninitialized -Wreorder -Wundef -Wwrite-strings -Wsign-compare -Wmissing-declarations -Wswitch-default
NAZIBASE= $(FLAGSBASE) -pedantic -Wconversion -Wmissing-noreturn -Wold-style-cast -Weffc++ -Wall -Wunused -Wsign-conversion -Wunused -Wstrict-aliasing -Wstrict-overflow -Wconversion -Wdisabled-optimization
NAZI= $(NAZIBASE)
CFLAGS=$(NAZI)
LDFLAGS=-lgmpxx -lgmp
EXEC=setup rapidex
EXECLATEX=setup rapidexlatex
LEX=flex
YACC=bison

# Definitions of colors

NO_COLOR=\033[m\017
ERROR_COLOR=\033[0;31m
OK_COLOR=\033[0;32m
WARN_COLOR=\033[0;33m
CC_COLOR=\033[1;34m

# Definition of the different messages with their colors

OK_STRING=$(OK_COLOR)[OK]$(NO_COLOR)
ERROR_STRING=$(ERROR_COLOR)[ERRORS]$(NO_COLOR)
WARN_STRING=$(WARN_COLOR)[WARNINGS]$(NO_COLOR)
CC_STRING=$(CC_COLOR)[.CC.]$(NO_COLOR)
RM_STRING=$(CC_COLOR)[.RM.]$(NO_COLOR)
LD_STRING=$(CC_COLOR)[.LD.]$(NO_COLOR)
YACC_STRING=$(CC_COLOR)[YACC]$(NO_COLOR)
FLEX_STRING=$(CC_COLOR)[FLEX]$(NO_COLOR)

# Primitives to print the messages (magic)

AWK_CMD = awk '{ printf "%-40s %-40s\n",$$1, $$2; }'
PRINT_ERROR = printf "$@ $(ERROR_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n" && false
PRINT_WARNING = printf "$@ $(WARN_STRING)\n" | $(AWK_CMD) && printf "$(CMD)\n$$LOG\n"
PRINT_OK = printf "$@ $(OK_STRING)\n" | $(AWK_CMD)
PRINT_CC = printf "$(CC_STRING)" | awk '{ printf "%-5s %-5s",$$1, $$2; }'
PRINT_RM = printf "$(RM_STRING)" | awk '{ printf "%-5s %-5s",$$1, $$2; }'
PRINT_LD = printf "$(LD_STRING)" | awk '{ printf "%-5s %-5s",$$1, $$2; }'
PRINT_FLEX = printf "$(FLEX_STRING)" | awk '{ printf "%-5s %-5s",$$1, $$2; }'
PRINT_YACC = printf "$(YACC_STRING)" | awk '{ printf "%-5s %-5s",$$1, $$2; }'
BUILD_CMD = LOG=$$($(CMD) 2>&1) ; if [ $$? -eq 1 ]; then $(PRINT_ERROR); elif [ "$$LOG" != "" ] ; then $(PRINT_WARNING); else $(PRINT_OK); fi;

# Find sources and build objects and dependences

SRCS := $(shell find src -name '*.c') $(shell find src -name '*.cpp') $(shell find src -name '*.y') $(shell find src -name '*.lex')
OBJS := $(patsubst src/%.c,obj/%.o,$(patsubst src/%.cpp,obj/%.o,$(SRCS)))
DEPS := $(patsubst src/%.c,obj/%.d,$(patsubst src/%.cpp,obj/%.d,$(SRCS)))

.PHONY: clean

all: $(EXEC)

latex: CC=clang++ -D LATEX
latex: $(EXEC)

debug: CC=clang++ -D DEBUG
debug: $(EXEC)

purge: clean all

setup:
	@mkdir -p obj


obj/.o: tests/.cpp
	@$(CC) $(C11) $(CFLAGS) -c $< -o $@ -MD -MT '$@' -MF '$(patsubst obj/.o,obj/.d,$@)'

obj/%.o: CMD = $(CC) $(C11) $(CFLAGS) -c $< -o $@ -MD -MT '$@' -MF '$(patsubst obj/%.o,obj/%.d,$@)'
obj/%.o: src/%.cpp
	@$(PRINT_CC)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)
	
obj/SimplexParser.o: CMD = $(CC) $(C11) -c $< -o $@ -MD -MT '$@' -MF '$(patsubst obj/%.o,obj/%.d,$@)'
obj/SimplexParser.o: parser/SimplexParser.cpp
	@$(PRINT_CC)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

parser/SimplexParser.cpp: CMD = $(YACC) -dv -o $@ $<
parser/SimplexParser.cpp: parser/SimplexParser.y
	@$(PRINT_YACC)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

obj/SimplexLexer.o: CMD = $(CC) $(C11) -Wno-deprecated-register -c $< -o $@ -MD -MT '$@' -MF '$(patsubst obj/%.o,obj/%.d,$@)'
obj/SimplexLexer.o: parser/SimplexLexer.cpp
	@$(PRINT_CC)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

parser/SimplexLexer.cpp: CMD = $(LEX) -o $@ $<
parser/SimplexLexer.cpp: parser/SimplexParser.lex
	@$(PRINT_FLEX)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

obj/driver.o: CMD = $(CC) $(C11) -c $< -o $@ -MD -MT '$@' -MF '$(patsubst obj/%.o,obj/%.d,$@)'
obj/driver.o: parser/driver.cpp
	@$(PRINT_CC)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

obj/lexer.o: CMD = $(CC) $(C11) -c $< -o $@ -MD -MT '$@' -MF '$(patsubst obj/%.o,obj/%.d,$@)'
obj/lexer.o: parser/lexer.cpp
	@$(PRINT_CC)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)


rapidex: CMD = $(CC) obj/.o obj/SimplexParser.o obj/SimplexLexer.o obj/driver.o obj/lexer.o $(OBJS) $(LDFLAGS) -o $@
rapidex: obj/.o obj/SimplexParser.o obj/SimplexLexer.o obj/driver.o obj/lexer.o $(OBJS)
	@$(PRINT_LD)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

rapidexlatex: CMD = $(CC) obj/.o obj/SimplexParser.o obj/SimplexLexer.o obj/driver.o obj/lexer.o $(OBJS) $(LDFLAGS) -o $@
rapidexlatex: obj/.o obj/SimplexParser.o obj/SimplexLexer.o obj/driver.o obj/lexer.o $(OBJS)
	@$(PRINT_LD)
	@mkdir -p $(dir $@)
	@$(BUILD_CMD)

clean:
	@$(PRINT_RM)
	@rm -f obj/*.o
	@rm -f obj/.o
	@rm -f obj/*.d
	@rm -f obj/.d
	@rm -f parser/SimplexParser.cpp
	@rm -f parser/SimplexLexer.cpp
	@rm -f parser/*.hh
	@rm -f parser/*.hpp
	@rm -f parser/*.output
	@rm -f rapidex
	@$(PRINT_OK)

