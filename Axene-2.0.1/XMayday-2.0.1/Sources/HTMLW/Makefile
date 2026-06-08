#HtmlW : Html Widget - NCSA Moasic

LIB = ../lib/HTMLW.a
SHELL=	/bin/sh
OBJ = HTML.o HTMLlists.o HTMLformat.o HTMLparse.o HTMLimages.o HTMLwidgets.o \
 HTML-PSformat.o HTMLjot.o HTMLtable.o list.o 

all: showenv $(LIB)

.c.o:
	@../Utils/echonl.sh "LIB $(LIB) Compile: "$@" "
	@$(CC) $(CPPFLAGS) $(CFLAGS) -I. -I../Headers -c $<
	@echo "done."

$(LIB): $(OBJ)
	@echo "LIB $(LIB) Archiving: $(OBJ)"
	@$(AR) $(ARFLAGS) $@ $(OBJ) > /dev/null
	@echo "LIBRARY $(LIB) built."

clean:
	rm -f $(LIB) $(OBJ) *~ #*

showenv:
	@echo -------------- LIB Compilation options -------------------------------------
	@echo "CFLAGS='$(CFLAGS)'"
	@echo "CPPFLAGS='$(CPPFLAGS)'"
	@echo "ARFLAGS='$(ARFLAGS)'"
	@echo "export CFLAGS CPPFLAGS ARFLAGS"
	@echo ----------------------------------------------------------------------------

#------ Dependances ------

$(OBJ): HTML.h HTMLP.h

