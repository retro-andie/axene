# Makefile

OBJ= main.o ../Encrypt/Encrypt.o Decrypt.o

all: main

main:	$(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)

.c.o:
	$(CC) $(CPPFLAGS) $(CFLAGS) -I../Headers -c $< -o $@

clean:
	rm -f main *.o *~

