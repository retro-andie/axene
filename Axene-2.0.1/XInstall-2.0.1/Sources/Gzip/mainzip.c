#include <fcntl.h>
#include <sys/stat.h>
#include "gzip.h"
#include "lzw.h"
#include <unistd.h>
#include "tailor.h"

#ifndef MAX_PATH_LEN
#  define MAX_PATH_LEN   1024 /* max pathname length */
#endif

/* global buffers */

unsigned char	inbuf[INBUFSIZ +INBUF_EXTRA];
unsigned char   outbuf[OUTBUFSIZ+OUTBUF_EXTRA];
unsigned short	d_buf[DIST_BUFSIZE];
unsigned char	window[2L*WSIZE];
unsigned short	tab_prefix[1L<<BITS];

int exit_code = OK;   /* program exit code */
char *progname;       /* program name */
int maxbits = BITS;   /* max bits per code for LZW */
int method = DEFLATED;/* compression method */
int part_nb;          /* number of parts in .gz file */
int last_member;      /* set for .zip and .Z files */
long time_stamp;      /* original time stamp (modification time) */
int no_name = -1;     /* don't save or restore the original file name */
int no_time = -1;     /* don't save or restore the original file time */
int (*work) OF((int infile, int outfile)) = unzip; /* function to call */

long bytes_in;             /* number of input bytes */
long bytes_out;            /* number of output bytes */
long total_in = 0;         /* input bytes for all files */
long total_out = 0;        /* output bytes for all files */
char ifname[MAX_PATH_LEN]; /* input file name */
char ofname[MAX_PATH_LEN]; /* output file name */
int  remove_ofname = 0;	   /* remove output file on error */
struct stat istat;         /* status for input file */
int  ifd;                  /* input file descriptor */
int  ofd;                  /* output file descriptor */
unsigned insize;           /* valid bytes in inbuf */
unsigned inptr;            /* index of next byte to be processed in inbuf */
unsigned outcnt;           /* bytes in output buffer */

static int get_method();
int gunzip();

int gunzip(filename)
char	*filename;
{
 ifd = open(filename, O_RDONLY);
 inptr = 0;
 insize = 0;
 if (ifd < 0)
  {
   perror(filename);
   return -1;
  }
 get_method(ifd);
 ofd = 1;
  
 for (;;)
  {
   if ((*work)(ifd, ofd) != OK) 
    {
     method = -1;		/* force cleanup */
     break;
    }
   if (last_member || inptr == insize) 
    break;			/* end of file */

   method = get_method(ifd);
   if (method < 0) 
    break;			/* error message already emitted */
   bytes_out = 0;		/* required for length check */
  }
 close(ifd);
 return 0;
}

int gunzip_fd(fd)
int	fd;
{
 ifd = fd;
 inptr = 0;
 insize = 0;
 get_method(ifd);
 ofd = 1;
  
 for (;;)
  {
   if ((*work)(ifd, ofd) != OK) 
    {
     method = -1;		/* force cleanup */
     break;
    }
   if (last_member || inptr == insize) 
    break;			/* end of file */

   method = get_method(ifd);
   if (method < 0) 
    break;			/* error message already emitted */
   bytes_out = 0;		/* required for length check */
  }
 return 0;
}


static int get_method(in)
int	in;        /* input file descriptor */
{
 unsigned char flags;		/* compression flags */
 char magic[2];			/* magic header */
 unsigned long stamp;		/* time stamp */

 magic[0] = (char)get_byte();
 magic[1] = (char)get_byte();
 method = -1;			/* unknown yet */
 part_nb++;			/* number of parts in gzip file */
 last_member = 0;
 /* assume multiple members in gzip file except for record oriented I/O */

 if (memcmp(magic, GZIP_MAGIC, 2) == 0
     || memcmp(magic, OLD_GZIP_MAGIC, 2) == 0)
  {
   method = (int)get_byte();
   if (method != DEFLATED)
    {
     fprintf(stderr,
	     "%s: %s: unknown method %d -- get newer version of gzip\n",
	     progname, ifname, method);
     exit_code = ERROR;
     return -1;
    }
   work = unzip;
   flags  = (uch)get_byte();

   if ((flags & ENCRYPTED) != 0)
    {
     fprintf(stderr,
	     "%s: %s is encrypted -- get newer version of gzip\n",
	     progname, ifname);
     exit_code = ERROR;
     return -1;
    }
   if ((flags & CONTINUATION) != 0)
    {
     fprintf(stderr,
	     "%s: %s is a a multi-part gzip file -- get newer version of gzip\n",
	     progname, ifname);
     exit_code = ERROR;
     return -1;
    }
   if ((flags & RESERVED) != 0)
    {
     fprintf(stderr,
	     "%s: %s has flags 0x%x -- get newer version of gzip\n",
	     progname, ifname, flags);
     exit_code = ERROR;
     return -1;
    }
   stamp  = (ulg)get_byte();
   stamp |= ((ulg)get_byte()) << 8;
   stamp |= ((ulg)get_byte()) << 16;
   stamp |= ((ulg)get_byte()) << 24;
   if (stamp != 0 && !no_time) 
    time_stamp = stamp;

   (void)get_byte();		/* Ignore extra flags for the moment */
   (void)get_byte();		/* Ignore OS type for the moment */

   if ((flags & CONTINUATION) != 0)
    {
     unsigned part = (unsigned)get_byte();
     part |= ((unsigned)get_byte())<<8;
#if DEBUG
     printf(stderr,"%s: %s: part number %u\n",
	    progname, ifname, part);
#endif
    }
   if ((flags & EXTRA_FIELD) != 0)
    {
     unsigned len = (unsigned)get_byte();
     len |= ((unsigned)get_byte())<<8;
#if DEBUG
     fprintf(stderr,"%s: %s: extra field of %u bytes ignored\n",
	     progname, ifname, len);
#endif
     while (len--) 
      (void)get_byte();
    }

   /* Get original file name if it was truncated */
   if ((flags & ORIG_NAME) != 0)
    {
     if (no_name || part_nb > 1)
      {
       /* Discard the old name */
       char c;			/* dummy used for NeXTstep 3.0 cc optimizer bug */
       do 
	{
	 c=get_byte();
	} while (c != 0);
      } 
     else 
      {
       /* Copy the base name. Keep a directory prefix intact. */
       char *p = basename(ofname);
#if MAKE_LEGAL_NAME != 0
       char *base = p;
#endif
       for (;;) 
	{
	 *p = (char)get_char();
	 if (*p++ == '\0') break;
	 if (p >= ofname+sizeof(ofname)) 
	  {
	   error("corrupted input -- file name too large\n");
	  }
	}
       /* If necessary, adapt the name to local OS conventions: */
       MAKE_LEGAL_NAME(base);
      }/* no_name || to_stdout */
    }/* ORIG_NAME */

   /* Discard file comment if any */
   if ((flags & COMMENT) != 0)
    {
     while (get_char() != 0)	/* null */ ;
    }
   if (part_nb == 1) 
    {
#if 0
     header_bytes = inptr + 2*sizeof(long);/* include crc and size */
#endif
    }

  } 
 else 
  if (memcmp(magic, PKZIP_MAGIC, 2) == 0 && inptr == 2
      && memcmp((char*)inbuf, PKZIP_MAGIC, 4) == 0) 
   {
    /* To simplify the code, we support a zip file when alone only.
    * We are thus guaranteed that the entire local header fits in inbuf.
    */
    inptr = 0;
    work = unzip;
    if (check_zipfile(in) != OK) 
     return -1;
    /* check_zipfile may get ofname from the local header */
    last_member = 1;

   } 
  else 
   if (memcmp(magic, PACK_MAGIC, 2) == 0) 
    {
     work = unpack;
     method = PACKED;
    } 
   else 
    if (memcmp(magic, LZW_MAGIC, 2) == 0) 
     {
      work = unlzw;
      method = COMPRESSED;
      last_member = 1;
     } 
    else 
     if (memcmp(magic, LZH_MAGIC, 2) == 0) 
      {
       work = unlzh;
       method = LZHED;
       last_member = 1;
      } 

 if (method >= 0) 
  return method;

 if (part_nb == 1)
  {
   fprintf(stderr, "\n%s: %s: not in gzip format\n", progname, ifname);
   exit_code = ERROR;
   return -1;
  } 
 else
  {
   return -2;
  }
}

void abort_gzip()
{
 if (remove_ofname) 
 {
  close(ofd);
  unlink (ofname);
 }
 exit(ERROR);
}
