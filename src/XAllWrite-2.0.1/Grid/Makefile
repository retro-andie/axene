OBJ=	Grid.o \
	../ManageWidget/ManageWidget.o
XcLIBS= ../lib/Managers.a
#CPPFLAGS= -I. -I../Headers

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -I../Headers -c $< -o $@

gato:	$(OBJ) minimain.o
	$(CC) $(LDFLAGS) -o $@ minimain.o $(OBJ) $(XcLIBS) $(MOTIF_LIBS)

$(OBJ):	Grid.c

clean:
	rm -f *.o *~ *.a gato
