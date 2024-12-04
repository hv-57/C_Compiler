CPP = g++
CPPFLAGS = -g -O3 -Wall -std=c++23

LINKER = ld
LINKERFLAGS = -r

BISON = bison
FLEX = flex

TARGET = sclp
SCAN = $(TARGET).l
PARSE = $(TARGET).y

OBJECTS = scan.o parse.tab.o ast.o tac.o rtl.o asm.o function.o main.o

$(TARGET): $(OBJECTS) Makefile
	$(CPP) $(CPPFLAGS) $(OBJECTS) -o $(TARGET)

main.o: main.cpp
	$(CPP) $(CPPFLAGS) -c $<

function.o: function.cpp function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

ast.o: ast_build.o ast_print.o
	$(LINKER) $(LINKERFLAGS) $^ -o $@

ast_build.o: ast_build.cpp function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

ast_print.o: ast_print.cpp function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

tac.o: tac_build.o tac_print.o	
	$(LINKER) $(LINKERFLAGS) $^ -o $@

tac_build.o: tac_build.cpp function.h ast.h tac.h rtl.h	
	$(CPP) $(CPPFLAGS) -c $<	

tac_print.o: tac_print.cpp function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

rtl.o: rtl_build.o rtl_print.o
	$(LINKER) $(LINKERFLAGS) $^ -o $@

rtl_build.o: rtl_build.cpp function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

rtl_print.o: rtl_print.cpp function.h ast.h tac.h rtl.h	
	$(CPP) $(CPPFLAGS) -c $<

asm.o: asm.cpp function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

scan.o: scan.c parse.tab.h
	$(CPP) $(CPPFLAGS) -c $<

parse.tab.o: parse.tab.c parse.tab.h function.h ast.h tac.h rtl.h
	$(CPP) $(CPPFLAGS) -c $<

scan.c: $(SCAN) parse.tab.h
	$(FLEX) -o $@ $<

parse.tab.c parse.tab.h: $(PARSE)
	$(BISON) -b parse -dv $<

clean:
	@rm -f scan.c
	@rm -f parse.tab.c parse.tab.h parse.output
	@rm -f ast_build.o ast_print.o
	@rm -f tac_build.o tac_print.o
	@rm -f rtl_build.o rtl_print.o
	@rm -f $(OBJECTS) $(TARGET)
