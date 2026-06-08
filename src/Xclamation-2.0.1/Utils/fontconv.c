/*
** fontconv.c for Xclamation in Utils/
**
** Copyright (C) 1994-2000 Axene.
** Authors: Stéphane Boisson, Antoine Buat, Robin Castanier and Emmanuel Paris.
** Email: xcalibur@axene.org
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program; if not, write to the Free Software
**    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Started on  Wed Jan 10 13:07:29 1996 Stéphane Boisson
** Last update Thu Jan 11 14:53:32 1996 Stéphane Boisson
*/

#include <stdio.h>
#include <fcntl.h>

/*--- Macros ---*/
#define FALSE 0
#define TRUE (~FALSE)
#define BUFFER_SIZE 4096
#define LINE_SIZE 78
#define MIN(a, b) (((a) > (b))? (b) : (a))

/*--- Chunk types ---*/
#define CHUNK_ASCII 1
#define CHUNK_BINARY 2
#define CHUNK_EOF 3
#define CHUNK_HEAD 0x80
#define CHUNK_XCHEAD 0
#define CHUNK_NONE -1


#ifdef __cplusplus
# define ___PROTO(x) x
#else
# ifdef __STDC__
#  define ___PROTO(x) x
# else
#  define ___PROTO(x) ()
# endif
#endif

extern void *realloc();

/*--- Global variables ---*/
char *progname;
unsigned char buffer[BUFFER_SIZE];
int xcalibur_format = FALSE;

/*--- Prototypes ---*/
int main ___PROTO((int argc, char **argv));
int nothing ___PROTO((int in, int out));
int pfa2pfb ___PROTO((int in, int out));
int pfb2pfa ___PROTO((int in, int out));
int outhex ___PROTO((int fd, int byte));
int outchunk ___PROTO((int fd, int type, int byte));


/* ----------------------------------------------------------------- ** 
** main - Main part                                                  ** 
** ----------------------------------------------------------------- */
int main(argc, argv)
int argc;
char **argv;
{
  int (*convert)___PROTO((int in, int out)); 
  int in, out;

  progname = *argv;

  /*--- Parse options ---*/
  convert = strcmp(*argv++, "pfa2pfb")? pfb2pfa : pfa2pfb;
  for(--argc; (argc > 0) && (**argv == '-'); argc--, argv++)
    switch((*argv)[1])
      {
	/*--- Convert to PFA ---*/
      case 'a':
	convert = pfb2pfa;
	break;

	/*--- Convert to PFB ---*/
      case 'b':
	convert = pfa2pfb;
	break;

	/*--- XCalibur format ---*/
      case 'c':
	xcalibur_format = TRUE;
	break;

	/*--- Usage ---*/
      default:
	fprintf(stderr, "Usage: %s [-a] [-b] [-c] fontfile\n", progname);
	fprintf(stderr, " -a : Convert PFB to PFA\n");
	fprintf(stderr, " -b : Convert PFA to PFB\n");
	fprintf(stderr, " -c : XCalibur PFB format\n");
	exit(1);
      }

  /*--- Convert font ---*/
  in = fileno(stdin);
  out = fileno(stdout);

  if(argc)
    {
      fprintf(stderr, "Convert `%s'...\n", *argv);
      if((in = open(*argv, O_RDONLY)) < 0)
	{
	  perror(*argv);
	  exit(1);
	}
    }
  
  if(convert(in, out) == FALSE)
    fprintf(stderr, "  Error\n");
  else
    fprintf(stderr, "  Ok\n");

  
  if(in != fileno(stdin)) close(in);

  exit(0);
}


/* ----------------------------------------------------------------- ** 
** outhex - Output an hex byte                                       ** 
** ----------------------------------------------------------------- */
int outhex(fd, byte)
int fd;
int byte;
{
  static char hextable[] = "0123456789ABCDEF", line[LINE_SIZE + 1];
  static int pos = 0;
  
  /*--- Flush line ---*/
  if(((byte < 0) || (pos >= LINE_SIZE)) && (pos > 0))
    {
      int written, count;
      
      line[pos++] = '\n';
      for(written = 0; written != pos; written += count)
	if((count = write(fd, line + written, pos - written)) <= 0)
	  {
	    if(count) perror(progname);
	    return FALSE;
	  }
      pos = 0;
      if(byte < 0) return TRUE;
    }

  line[pos++] = hextable[(byte >> 4) & 0xf];
  line[pos++] = hextable[byte & 0xf];
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** outchunk - output byte in a chunk                                 ** 
** ----------------------------------------------------------------- */
int outchunk(fd, type, byte)
int fd;
int type;
int byte;
{
  static unsigned char *chunk = NULL;
  static long maxpos = 0, pos = 0;
  static int ctype = CHUNK_NONE;

  if((ctype != type) && (ctype != CHUNK_NONE))
    {
      int written, count;
      
      if(xcalibur_format == FALSE)
	{
	  chunk[0] = CHUNK_HEAD;
	  chunk[1] = ctype;
	}
      else
	{
	  chunk[0] = CHUNK_XCHEAD;
	  chunk[1] = ctype ^ 0xFF;
	}

      if(ctype != CHUNK_EOF)
	{
	  chunk[2] = pos & 0xFF;
	  chunk[3] = (pos >> 8) & 0xFF;
	  chunk[4] = (pos >> 16) & 0xFF;
	  chunk[5] = (pos >> 24) & 0xFF;

	  if(xcalibur_format == TRUE)
	    {
	      chunk[2] ^= 0xFF;
	      chunk[3] ^= 0xFF;
	      chunk[4] ^= 0xFF;
	      chunk[5] ^= 0xFF;
	    }

	  fprintf(stderr, "  %s chunk %ld bytes\n",
		  ((ctype == CHUNK_ASCII)? "ASCII" : "Binary"), pos);
	  
	  pos += 6;
	}
      else
	{
	  pos = 2;
	  fprintf(stderr, "  EOF chunk\n");
	}

      for(written = 0; written != pos; written += count)
	if((count = write(fd, chunk + written, pos - written)) <= 0)
	  {
	    if(count) perror(progname);
	    return FALSE;
	  }
      pos = 0;
    }

  if((ctype = type) == CHUNK_NONE) return TRUE;

  if(pos == maxpos)
    {
      unsigned char *ptr;

      maxpos += BUFFER_SIZE;
      if((chunk = realloc(chunk, maxpos + 6)) == NULL)
	return FALSE;
    }

  chunk[6 + pos++] = (unsigned char)byte;
  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** nothing - Copy source to target                                   ** 
** ----------------------------------------------------------------- */
int nothing(in, out)
int in;
int out;
{
  long size, written;

  fprintf(stderr, "  Already converted\n");
  while((size = read(in, buffer, BUFFER_SIZE)) > 0)
    for(written = 0; size != written; )
      {
	long count;

	if((count = write(out, buffer + written, size - written)) <= 0)
	  {
	    if(count) perror(progname);
	    return FALSE;
	  }
	written += count;
      }

  if(size < 0)
    {
      perror(progname);
      return FALSE;
    }

  return TRUE;
}


/* ----------------------------------------------------------------- ** 
** pfa2pfb - Convert PFA font format to PFB font format              ** 
** ----------------------------------------------------------------- */
int pfa2pfb(in, out)
int in;
int out;
{
  unsigned char c;
  int match, byte;
  long size;

  if((size = read(in, buffer, 2)) < 2)
    {
      fprintf(stderr, "%s: unexpected end of fontfile\n", progname);
      return FALSE;
    }
   
  /*--- Test if PFA ---*/
  if((buffer[0] == CHUNK_HEAD) && (buffer[1] == CHUNK_ASCII))
    {
      if(write(out, buffer, size) != size)
	{
	  perror(progname);
	  return FALSE;
	}
      return nothing(in, out);
    }

  if((buffer[0] != '%') || (buffer[1] != '!'))
    {
      fprintf(stderr, "%s: unknow font format\n", progname);
      return FALSE;
    }

  if(xcalibur_format) fprintf(stderr, "  Output XCalibur format\n");
 
  /*--- Output first ASCII chunk ---*/
  if(!outchunk(out, CHUNK_ASCII, buffer[0]) ||
     !outchunk(out, CHUNK_ASCII, buffer[1])) return FALSE;
  for(match = 0; match != 6; )
    {
      if(read(in, &c, 1) != 1)
	{
	  perror(progname);
	  return FALSE;
	}

      switch(match)
	{
	case 0:
	  if(c == 'e') match++;
	  break;
	case 1:
	  if(c == 'e') match++; else match = 0;
	  break;
	case 2:
	  if(c == 'x') match++; else match = 0;
	  break;
	case 3:
	  if(c == 'e') match++; else match = 0;
	  break;
	case 4:
	  if(c == 'c') match++; else match = 0;
	  break;
	case 5:
	  if(c <= 32) match++; else match = 0;
	  break;
	}

      if(outchunk(out, CHUNK_ASCII, c) == FALSE) return FALSE;
    }
  
  /*--- Binary chunk ---*/
  match = 0;
  byte = 0;
  while(read(in, &c, 1) == 1)
    {
      if((c >= '0') && (c <= '9'))
	c -= '0';
      else if((c >= 'a') && (c <= 'f'))
	c -= 'a' - 10;
      else if((c >= 'A') && (c <= 'F'))
	c -= 'A' - 10;
      else
	continue;

      if(match)
	{
	  if(outchunk(out, CHUNK_BINARY, byte | (int)c) == FALSE) return FALSE;
	  match = 0;
	}
      else
	{
	  match = 1;

	  if(c == 0)
	    {
	      while(read(in, &c, 1) == 1)
		if(c == '0')
		  match++;
		else if(c > 32)
		  break;
		else if(match == 512)
		  break;
	      
	      if(match == 512) break;
	      
	      while(match > 1)
		{
		  if(outchunk(out, CHUNK_BINARY, 0) == FALSE) return FALSE;
		  match -= 2;
		}
	      
	      if((c >= '0') && (c <= '9'))
		c -= '0';
	      else if((c >= 'a') && (c <= 'f'))
		c -= 'a' - 10;
	      else if((c >= 'A') && (c <= 'F'))
		c -= 'A' - 10;
	      else
		continue;

	      if(match)
		{
		  if(outchunk(out, CHUNK_BINARY, (int)c) == FALSE)
		    return FALSE;
		  match = 0;
		  continue;
		}
	      else
		match = 1;
	    }
	  byte = (int)c << 4;
	}
    }

  /*--- End ascii chunk ---*/
  for(c = 16; c; c--)
    {
      for(size = 32; size; size--)
	if(outchunk(out, CHUNK_ASCII, '0') == FALSE) return FALSE;
      if(outchunk(out, CHUNK_ASCII, '\n') == FALSE) return FALSE;
    }
  
  while(read(in, &c, 1) == 1)
    if(!outchunk(out, CHUNK_ASCII, c)) return FALSE;

  if(!outchunk(out, CHUNK_EOF, 0)) return FALSE;
  if(!outchunk(out, CHUNK_NONE, 0)) return FALSE;
}


/* ----------------------------------------------------------------- ** 
** pfb2pfa - Convert PFB font format to PFA font format              ** 
** ----------------------------------------------------------------- */
int pfb2pfa(in, out)
int in;
int out;
{
  long size;
  int done;

  if((size = read(in, buffer, 6)) < 2)
    {
      fprintf(stderr, "%s: unexpected end of fontfile\n", progname);
      return FALSE;
    }

  /*--- Test if already PFA ---*/
  if((buffer[0] == '%') && (buffer[1] == '!'))
    {
      if(write(out, buffer, size) != size)
	{
	  perror(progname);
	  return FALSE;
	}
      return nothing(in, out);
    }

  if(xcalibur_format) fprintf(stderr, "  Input XCalibur format\n");

  done = FALSE;
  while(42)
    {
      unsigned char type;

      if(xcalibur_format == TRUE)
	{
	  if(buffer[0] != CHUNK_XCHEAD) break;
	  buffer[1] ^= 0xFF;
	}
      else if(buffer[0] != CHUNK_HEAD)
	break;

      type = buffer[1];

      /*--- Test ASCII or Binary chunk ---*/
      if((type == CHUNK_ASCII) || (type == CHUNK_BINARY))
	{
	  long nread, count, i;

	  if(size != 6) break;
	  if(xcalibur_format)
	    {
	      buffer[2] ^= 0xFF;
	      buffer[3] ^= 0xFF;
	      buffer[4] ^= 0xFF;
	      buffer[5] ^= 0xFF;
	    }

	  /*--- Get chunk size ---*/
	  size = buffer[5];
	  size <<= 8;
	  size |= buffer[4];
	  size <<= 8;
	  size |= buffer[3];
	  size <<= 8;
	  size |= buffer[2];

	  fprintf(stderr, "  %s chunk %ld bytes\n",
		  ((type == CHUNK_ASCII)? "ASCII" : "Binary"), size);

	  /*--- Convert chunk ---*/
	  for(nread = 0; nread != size; )
	    {
              count = MIN(BUFFER_SIZE, size - nread);
	      if((count = read(in, buffer, count)) <= 0)
		{
		  if(count == 0)
		    fprintf(stderr, "%s: unexpected end of file\n", progname);
		  else
		    perror(progname);
		  break;
		}

              nread += count;
              if(type == CHUNK_ASCII)
                {
                  register char *ptr;

                  for(i = count, ptr = buffer; i--; ptr++)
                    if(*ptr == '\r') *ptr = '\n';

		  if(write(out, buffer, count) != count)
		    {
		      perror(progname);
		      break;
		    }
                }
              else
		{
                  for(i = 0; i < count; i++)
		    if(outhex(out, (int)buffer[i]) == FALSE) break;
                }
	    }

	  if(type == CHUNK_BINARY) outhex(out, -1);
	}
      else
	{
	  fprintf(stderr, "  %s chunk\n",
		  (type == CHUNK_EOF)? "EOF" : "Unknown");
	  done = (type == CHUNK_EOF)? TRUE : FALSE;
	  break;
	}

      if((size = read(in, buffer, 6)) < 2)
	{
	  fprintf(stderr, "%s: unexpected end of fontfile\n", progname);
	  return FALSE;
	}
    }
  return done;
}

