/*
 * jpeg.c 		- loading jpeg routine
 *
 * Copyright (C) 1994	Xcalibur <xcalibur@axene.org>
 *
 * This file can be redistributed under the terms of the GNU General
 * Public License
 */

#define NTRACE

#include "jpeg.h"

extern void initial_setup();
extern void d_initial_method_selection();
extern void j_d_defaults();

/*
typedef unsigned char	byte;
*/
static jmp_buf setjmp_buffer;	/* for return to caller */
static external_methods_ptr emethods; /* needed for access to message_parm */

static Xc_JPEG_Internal_Data	The_Data;

/*---------------------------------------------------------------------------*/
/*
METHODDEF void
trace_message (const char *msgtext)
{

  fprintf(stderr, msgtext,
	  emethods->message_parm[0], emethods->message_parm[1],
	  emethods->message_parm[2], emethods->message_parm[3],
	  emethods->message_parm[4], emethods->message_parm[5],
	  emethods->message_parm[6], emethods->message_parm[7]);
  fprintf(stderr, "\n");       
}
*/
/*---------------------------------------------------------------------------*/
METHODDEF void
error_exit (msgtext)
const char *msgtext;
{
  (*emethods->free_all) ();	/* clean up memory allocation & temp files */
  Xc_TRACE(("Error while decompressing jpeg ..."));
  longjmp(setjmp_buffer, 1);	/* return control to outer routine */
}
/*---------------------------------------------------------------------------*/
METHODDEF void
output_init (cinfo)
decompress_info_ptr cinfo;

/* This routine should do any setup required */
{
  if (cinfo->jpeg_color_space == CS_GRAYSCALE)
    {
      The_Data.Depth 		= Xc_JPEG_8;
    }
  else
    {
      The_Data.Depth 		= Xc_JPEG_24;
    }
  The_Data.Position = 0;
}
/*---------------------------------------------------------------------------*/
METHODDEF void
put_color_map256 (cinfo, num_colors, colormap)
decompress_info_ptr cinfo;
int 		num_colors;
JSAMPARRAY 	colormap;
/* Write the color map */
{
  int BitsPerPixel, ColorMapSize, InitCodeSize;
  int i;

  Xc_HISTORY(("Put Colormap"));
/*
  if (num_colors > 256)
    ERREXIT(cinfo->emethods, "GIF can only handle 256 colors");
*/
  BitsPerPixel = 1;
  while (num_colors > (1 << BitsPerPixel))
    BitsPerPixel++;
  ColorMapSize = 1 << BitsPerPixel;
  if (BitsPerPixel <= 1)
    InitCodeSize = 2;
  else
    InitCodeSize = BitsPerPixel;
  /* Write the Logical Screen Descriptor */
  /* Write the Global Color Map */
  /* If the color map is more than 8 bits precision, */
  /* we reduce it to 8 bits by shifting */
  for (i=0; i < ColorMapSize; i++) {
    if (i < num_colors) {
      if (colormap != NULL) {
	if (cinfo->out_color_space == CS_RGB) {
	  /* Normal case: RGB color map */
	  The_Data.The_Image->pal_r[i] = GETJSAMPLE(colormap[0][i]) << 8;
	  The_Data.The_Image->pal_v[i] = GETJSAMPLE(colormap[1][i]) << 8;
	  The_Data.The_Image->pal_b[i] = GETJSAMPLE(colormap[2][i]) << 8;
	} else {
	  Xc_TRACE(("Generate a Grayscale Colormap"));
	  /* Grayscale "color map": possible if quantizing grayscale image */
	  The_Data.The_Image->pal_r[i] = GETJSAMPLE(colormap[0][i]) << 8;
	  The_Data.The_Image->pal_v[i] = GETJSAMPLE(colormap[0][i]) << 8;
	  The_Data.The_Image->pal_b[i] = GETJSAMPLE(colormap[0][i]) << 8;
	}
      } else {
	/* Create a gray-scale map of num_colors values, range 0..255 */
	/* put_33bytes((i * 255 + (num_colors-1)/2) / (num_colors-1)); */
	Xc_WARNING(("Colormap for Jpeg file is NULL !!!"));
      }
    } else {
      /* fill out the map to a power of 2 */
     /* put_3bytes(0); */
    }
  }
  Xc_HISTORY(("Put colormap ok"));
}
/*---------------------------------------------------------------------------*/
METHODDEF void
put_pixel_rows8 (cinfo, num_rows, pixel_data)
decompress_info_ptr cinfo;
int num_rows;
JSAMPIMAGE pixel_data;
{
  register	char		*OutData;
  register 	JSAMPROW 	ptr0;
  register 	long 		col;
  register 	int 		row;

  for (row = 0; row < num_rows; row++) {
    ptr0 = pixel_data[0][row];
    OutData = (char *)The_Data.Data;
    for (col = 0; col < cinfo->image_width; col++) 
	*OutData++ = GETJSAMPLE(*ptr0++);
    (The_Data.Callback)(&The_Data.Data, The_Data.Callback_Data);
  }
}
/*---------------------------------------------------------------------------*/
METHODDEF void
put_pixel_rows24 (cinfo, num_rows, pixel_data)
decompress_info_ptr cinfo;
int num_rows;
JSAMPIMAGE pixel_data;
{
  register JSAMPROW ptr0, ptr1, ptr2;
  register long col;
  register int row;
  register unsigned char	*Buffer;
  
  for (row = 0; row < num_rows; row++) {
    ptr0 = pixel_data[0][row];
    ptr1 = pixel_data[1][row];
    ptr2 = pixel_data[2][row];
    Buffer = The_Data.Data;
    for (col = 0; col < cinfo->image_width; col++) {
      *Buffer++ = GETJSAMPLE(*ptr0);      ptr0++;
      *Buffer++ = GETJSAMPLE(*ptr1);      ptr1++;
      *Buffer++ = GETJSAMPLE(*ptr2);      ptr2++;
    }
    (The_Data.Callback)(&The_Data.Data, The_Data.Callback_Data);
  }
}
/*---------------------------------------------------------------------------*/
METHODDEF void
output_term (cinfo)
decompress_info_ptr cinfo;
{

}
/*---------------------------------------------------------------------------*/
METHODDEF void
d_ui_method_selection (cinfo)
decompress_info_ptr cinfo;
{
  if (cinfo->jpeg_color_space == CS_GRAYSCALE)
    {
      Xc_TRACE(("gray gray gray ..."));
      cinfo->out_color_space = CS_GRAYSCALE;
      cinfo->methods->put_color_map = put_color_map256;
      cinfo->methods->put_pixel_rows = put_pixel_rows8;
    }
  /* select output routines */
  cinfo->methods->output_init = output_init;

  if (The_Data.Depth == Xc_JPEG_24)
    {
      cinfo->methods->put_color_map = NULL;
      cinfo->methods->put_pixel_rows = put_pixel_rows24;
    }

  cinfo->methods->output_term = output_term;
}
/*---------------------------------------------------------------------------*/
GLOBAL int
read_JPEG_file (filename, mode)
char	*filename;
int	mode;
{
  struct Decompress_info_struct cinfo;
  struct Decompress_methods_struct dc_methods;
  struct External_methods_struct e_methods;

  if ((cinfo.input_file = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }
  
  cinfo.output_file = NULL;	/* if no actual output file involved */

  cinfo.methods = &dc_methods;	/* links to method structs */
  cinfo.emethods = &e_methods;

  emethods = &e_methods;	/* save struct addr for possible access */
  e_methods.error_exit = error_exit; /* supply error-exit routine */
  e_methods.trace_message = error_exit; /* supply trace-message routine */
  e_methods.trace_level = 0;	/* default = no tracing */
  e_methods.num_warnings = 0;	/* no warnings emitted yet */
  e_methods.first_warning_level = 0; /* display first corrupt-data warning */
  e_methods.more_warning_level = 3; /* but suppress additional ones */

  if (setjmp(setjmp_buffer)) {
    fclose(cinfo.input_file);
    Xc_TRACE(("In setjmp, return error ..."));
    return (XC_FILE_FORMAT_ERROR);
  }
  jselmemmgr(&e_methods);	/* select std memory allocation routines */

  dc_methods.d_ui_method_selection = d_ui_method_selection;

  j_d_defaults(&cinfo, TRUE);

  /* Set up to read a JFIF or baseline-JPEG file. */
  /* This is the only JPEG file format currently supported. */

  jselrjfif(&cinfo);

  /* Here we go! */
  if (mode == Xc_UNCOMPRESS_MODE)
    {
#ifndef NTRACE
      if (cinfo.jpeg_color_space == CS_GRAYSCALE)
	  Xc_TRACE(("GRAYSCALE JPEG ...  GRAYSCALE JPEG ..."));
      Xc_TRACE(("DECOMPRESS FILE ... DECOMPRESS FILE ..."));
#endif	
      jpeg_decompress(&cinfo);
    }
  else
    {
#ifndef NTRACE
      if (mode == Xc_TEST_MODE)
	Xc_TRACE(("Test jpeg file in (Read_Jpeg_File) : %s",filename));
#endif      
      cinfo.total_passes = 0;
      cinfo.completed_passes = 0;
      
      (*(cinfo.methods->read_file_header)) (&cinfo);
      Xc_TRACE(("File header ok"));
      if (! ((*(cinfo.methods->read_scan_header)) (&cinfo)))
	{
	  fclose(cinfo.input_file);
	  return (XC_FILE_FORMAT_ERROR);
	}
      else
	{
	  if (mode == Xc_HEADER_MODE)
	    {
	      output_init(&cinfo); 
	      if (The_Data.The_Image != NULL)
		{
		  The_Data.The_Image->width = cinfo.image_width;
		  The_Data.The_Image->height = cinfo.image_height;
		  if (cinfo.jpeg_color_space == CS_GRAYSCALE)
		    {
		      The_Data.The_Image->depth	= Xc_JPEG_8;
		      The_Data.The_Image->nb_color = 256;
		    }
		  else
		    {
		      The_Data.The_Image->depth	= Xc_JPEG_24;
		      The_Data.The_Image->nb_color = -1;
		    }
		}
	    }
	  fclose(cinfo.input_file);
	  return (XC_NO_ERROR);
	}
    }

  fclose(cinfo.input_file);

  /* You might want to test e_methods.num_warnings to see if bad data was
   * detected.  In this example, we just blindly forge ahead.
   */
#ifndef NTRACE
  if (e_methods.num_warnings != 0)
    Xc_TRACE(("there were %d data corrupted", e_methods.num_warnings));
#endif
  
  return XC_NO_ERROR;			/* indicate success */

}
/*---------------------------------------------------------------------------*/
int	Uncompress_Jpeg_Image(The_Image, line_buf, call_func, call_data)
c_Image		*The_Image;
unsigned char 	*line_buf;
boolean		(*call_func)  ___PROTO((unsigned char **pbuf, void *data));
void		*call_data;

{
  Xc_HISTORY(("Uncompress jpeg image ..."));
  The_Data.The_Image = The_Image; 
  The_Data.Callback = call_func;
  The_Data.Callback_Data = call_data;
  The_Data.Data = line_buf;

  return (read_JPEG_file(The_Image->filename, Xc_UNCOMPRESS_MODE));
}
/*---------------------------------------------------------------------------*/
boolean	Test_Jpeg_Image(image_filename)
char	*image_filename;
{
  Xc_HISTORY(("Test File ..."));
  The_Data.The_Image = NULL;
  if (read_JPEG_file(image_filename, Xc_TEST_MODE) != XC_NO_ERROR)
    {
      Xc_TRACE(("File is not a Jpeg file ..."));
      return (FALSE);
    }
  return (TRUE);
}
/*---------------------------------------------------------------------------*/
error Get_Jpeg_Header(The_Image)
c_Image 	*The_Image;
{
  int		Ii;

  Xc_HISTORY(("Get Header"));
  The_Data.The_Image = The_Image;
  Ii  = read_JPEG_file(The_Image->filename, Xc_HEADER_MODE);
  
  if (The_Data.Depth == Xc_JPEG_8)
    {
      /* allouer la memoire pour la palette dans l'objet image ... */

      The_Image->pal_r = (unsigned short *)
	Xc_malloc("pal_r",sizeof(unsigned short)*256);
      The_Image->pal_v = (unsigned short *)
	Xc_malloc("pal_v",sizeof(unsigned short)*256);
      The_Image->pal_b = (unsigned short *)
	Xc_malloc("pal_b",sizeof(unsigned short)*256);

      for (Ii = 0; Ii < 256; Ii++)
	{
	  The_Data.The_Image->pal_r[Ii] = Ii << 8;
	  The_Data.The_Image->pal_v[Ii] = Ii << 8;
	  The_Data.The_Image->pal_b[Ii] = Ii << 8;
	}
    }
  else
    {
      The_Image->pal_r = NULL;
      The_Image->pal_v = NULL;
      The_Image->pal_b = NULL;
    }
  Xc_HISTORY(("Get Header ok"));
  return (XC_NO_ERROR);
}
