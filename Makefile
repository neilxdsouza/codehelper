CC=g++ -c
LINK=g++

SOURCES=scope.h scope.C scope.h tree.h tree.C gram.y
DEPS= y_tab.c  lex_yy.c tree.C scope.C
HEADER_DEPS= tree.h scope.h 
LIBS=/borland/bcc55/lib
INCLUDE=/borland/bcc55/Include
#CC=/borland/bcc55/bin/bcc32.exe -P -c -I$(INCLUDE)
#OBJS=y_tab.obj lex_yy.obj scope.obj  tree.obj
OBJS=y_tab.o lex_yy.o scope.o  tree.o
#CC=/Borland/BCC55/Bin/bcc32.exe -P -I/Borland/BCC55/Include/ -L/Borland/BCC55/Lib 
#CC=bcc32.exe -P -Id:/BCC55/Include/ -Ld:/BCC55/Lib 

csassist.exe:	$(OBJS) Makefile  $(HEADER_DEPS)
#	$(CC) -ecsassist.exe $(OBJS)
	$(LINK) -o csassist.exe $(OBJS)

# Neil: I have commented out this rule as you may not have bison/flex installed
#lex_yy.c:	 lex.l y_tab.h tree.h 
#	flex -B -i $<
#	sed "s/#include <unistd.h>//" lex.yy.c > _tmp1
#	mv _tmp1 lex_yy.c
#	mv lex.yy.c _tmp2
#	rm _tmp2

tree.o: tree.C tree.h 
	$(CC) -c   $<


lex_yy.o: lex_yy.c
	$(CC) -c   $<

# Neil: I have commented out this rule as you may not have bison/flex installed
#y_tab.h y_tab.c: gram.y tree.h symtab.h  stmt.h
#	bison -v -y -d $<
#	mv y.tab.c y_tab.c
#	mv y.tab.h y_tab.h

y_tab.o: y_tab.c y_tab.h
	$(CC) -c   y_tab.c

scope.o: scope.C scope.h
	$(CC) -c   scope.C

temp.exe: temp.C
	$(CC) temp.C

.phony: clean

clean:
	rm *.obj

etags: $(SOURCES)
	etags $(SOURCES)
