/* $Header: /usr/people/sam/tiff/libtiff/RCS/tiffio.h,v 1.73 93/08/26 14:59:03 sam Exp $ */

/*
 * Copyright (c) 1988, 1989, 1990, 1991, 1992 Sam Leffler
 * Copyright (c) 1991, 1992 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and 
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 * 
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 * 
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF 
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 */

#ifndef _TIFFIO_
#define	_TIFFIO_

/*
 * TIFF I/O Library Definitions.
 */
#include "tiff.h"

#ifndef ___PROTO
#define ___PROTO(x) ()
#endif

/*
 * TIFF is defined as an incomplete type to hide the
 * library's internal data structures from clients.
 */
typedef	struct tiff TIFF;

/*
 * The following typedefs define the intrinsic size of
 * data types used in the *exported* interfaces.  These
 * definitions depend on the proper definition of types
 * in tiff.h.  Note also that the varargs interface used
 * pass tag types and values uses the types defined in
 * tiff.h directly.
 *
 * NB: ttag_t is unsigned int and not unsigned short because
 *     ANSI C requires that the type before the ellipsis be a
 *     promoted type (i.e. one of int, unsigned int, pointer,
 *     or double).
 * NB: tsize_t is int32 and not uint32 because some functions
 *     return -1.
 * NB: toff_t is not off_t for many reasons; TIFFs max out at
 *     32-bit file offsets being the most important
 */
typedef	unsigned int ttag_t;	/* directory tag */
typedef	uint16 tdir_t;		/* directory index */
typedef	uint16 tsample_t;	/* sample number */
typedef	uint32 tstrip_t;	/* strip number */
typedef uint32 ttile_t;		/* tile number */
typedef	int32 tsize_t;		/* i/o size in bytes */
typedef	void* tdata_t;		/* image data ref */
typedef	void* thandle_t;	/* client data handle */
typedef	int32 toff_t;		/* file offset */

#ifndef NULL
#define	NULL	0
#endif

/*
 * Flags to pass to TIFFPrintDirectory to control
 * printing of data structures that are potentially
 * very large.   Bit-or these flags to enable printing
 * multiple items.
 */
#define	TIFFPRINT_NONE		0x0		/* no extra info */
#define	TIFFPRINT_STRIPS	0x1		/* strips/tiles info */
#define	TIFFPRINT_CURVES	0x2		/* color/gray response curves */
#define	TIFFPRINT_COLORMAP	0x4		/* colormap */
#define	TIFFPRINT_JPEGQTABLES	0x100		/* JPEG Q matrices */
#define	TIFFPRINT_JPEGACTABLES	0x200		/* JPEG AC tables */
#define	TIFFPRINT_JPEGDCTABLES	0x200		/* JPEG DC tables */

/*
 * Macros for extracting components from the
 * packed ABGR form returned by TIFFReadRGBAImage.
 */
#define	TIFFGetR(abgr)	((abgr) & 0xff)
#define	TIFFGetG(abgr)	(((abgr) >> 8) & 0xff)
#define	TIFFGetB(abgr)	(((abgr) >> 16) & 0xff)
#define	TIFFGetA(abgr)	(((abgr) >> 24) & 0xff)

#include <stdio.h>
/*
#include <stdarg.h>
*/
#include <stdarg.h>


#if defined(__cplusplus)
extern "C" {
#endif
typedef	void (*TIFFErrorHandler) ___PROTO(( char* module,  char* fmt, va_list));
typedef	tsize_t (*TIFFReadWriteProc) ___PROTO((thandle_t, tdata_t, tsize_t));
typedef	toff_t (*TIFFSeekProc) ___PROTO((thandle_t, toff_t, int));
typedef	int (*TIFFCloseProc) ___PROTO((thandle_t));
typedef	toff_t (*TIFFSizeProc) ___PROTO((thandle_t));
typedef	int (*TIFFMapFileProc) ___PROTO((thandle_t, tdata_t*, toff_t*));
typedef	void (*TIFFUnmapFileProc) ___PROTO((thandle_t, tdata_t, toff_t));

extern	 char* TIFFGetVersion ___PROTO((void));

extern	void TIFFClose ___PROTO((TIFF*));
extern	int TIFFFlush ___PROTO((TIFF*));
extern	int TIFFFlushData ___PROTO((TIFF*));
extern	int TIFFGetField(TIFF*, ttag_t, ...);
extern	int TIFFVGetField ___PROTO((TIFF*, ttag_t, va_list));
extern	int TIFFGetFieldDefaulted(TIFF*, ttag_t, ...);
extern	int TIFFVGetFieldDefaulted ___PROTO((TIFF*, ttag_t, va_list));
extern	int TIFFReadDirectory ___PROTO((TIFF*));
extern	tsize_t TIFFScanlineSize ___PROTO((TIFF*));
extern	tsize_t TIFFStripSize ___PROTO((TIFF*));
extern	tsize_t TIFFVStripSize ___PROTO((TIFF*, uint32));
extern	tsize_t TIFFTileRowSize ___PROTO((TIFF*));
extern	tsize_t TIFFTileSize ___PROTO((TIFF*));
extern	tsize_t TIFFVTileSize ___PROTO((TIFF*, uint32));
extern	int TIFFFileno ___PROTO((TIFF*));
extern	int TIFFGetMode ___PROTO((TIFF*));
extern	int TIFFIsTiled ___PROTO((TIFF*));
extern	int TIFFIsByteSwapped ___PROTO((TIFF*));
extern	uint32 TIFFCurrentRow ___PROTO((TIFF*));
extern	tdir_t TIFFCurrentDirectory ___PROTO((TIFF*));
extern	tstrip_t TIFFCurrentStrip ___PROTO((TIFF*));
extern	ttile_t TIFFCurrentTile ___PROTO((TIFF*));
extern	int TIFFReadBufferSetup ___PROTO((TIFF*, tdata_t, tsize_t));
extern	int TIFFLastDirectory ___PROTO((TIFF*));
extern	int TIFFSetDirectory ___PROTO((TIFF*, tdir_t));
extern	int TIFFSetField(TIFF*, ttag_t, ...);
extern	int TIFFVSetField ___PROTO((TIFF*, ttag_t, va_list));
extern	int TIFFWriteDirectory ___PROTO((TIFF *));
#if defined (c_plusplus) || defined (__cplusplus)
extern	void TIFFPrintDirectory(TIFF*, FILE*, long = 0);
extern	int TIFFReadScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
extern	int TIFFWriteScanline(TIFF*, tdata_t, uint32, tsample_t = 0);
extern	int TIFFReadRGBAImage(TIFF*,
	    unsigned long, unsigned long, unsigned long*, int stop = 0);
#else
extern	void TIFFPrintDirectory ___PROTO((TIFF*, FILE*, long));
extern	int TIFFReadScanline ___PROTO((TIFF*, tdata_t, uint32, tsample_t));
extern	int TIFFWriteScanline ___PROTO((TIFF*, tdata_t, uint32, tsample_t));
extern	int TIFFReadRGBAImage ___PROTO((TIFF*,
	    unsigned long, unsigned long, unsigned long*, int stop));
#endif
extern	TIFF* TIFFOpen ___PROTO(( char*,  char*));
extern	TIFF* TIFFFdOpen ___PROTO((int,  char*,  char*));
extern	TIFF* TIFFClientOpen ___PROTO(( char* name,  char* mode,
	    thandle_t clientdata,
	    TIFFReadWriteProc readproc, TIFFReadWriteProc writeproc,
	    TIFFSeekProc seekproc, TIFFCloseProc closeproc,
	    TIFFSizeProc sizeproc,
	    TIFFMapFileProc mapproc, TIFFUnmapFileProc unmapproc));
extern	 char* TIFFFileName ___PROTO((TIFF*));
extern	void TIFFError(char*, char*, ...);
extern	void TIFFWarning(char*, char*, ...);
extern	TIFFErrorHandler TIFFSetErrorHandler ___PROTO((TIFFErrorHandler handler));
extern	TIFFErrorHandler TIFFSetWarningHandler ___PROTO((TIFFErrorHandler handler));
extern	ttile_t TIFFComputeTile ___PROTO((TIFF*, uint32, uint32, uint32, tsample_t));
extern	int TIFFCheckTile ___PROTO((TIFF*, uint32, uint32, uint32, tsample_t));
extern	ttile_t TIFFNumberOfTiles ___PROTO((TIFF*));
extern	tsize_t TIFFReadTile ___PROTO((TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t));
extern	tsize_t TIFFWriteTile ___PROTO((TIFF*,
	    tdata_t, uint32, uint32, uint32, tsample_t));
extern	tstrip_t TIFFComputeStrip ___PROTO((TIFF*, uint32, tsample_t));
extern	tstrip_t TIFFNumberOfStrips ___PROTO((TIFF*));
extern	tsize_t TIFFReadEncodedStrip ___PROTO((TIFF*, tstrip_t, tdata_t, tsize_t));
extern	tsize_t TIFFReadRawStrip ___PROTO((TIFF*, tstrip_t, tdata_t, tsize_t));
extern	tsize_t TIFFReadEncodedTile ___PROTO((TIFF*, ttile_t, tdata_t, tsize_t));
extern	tsize_t TIFFReadRawTile ___PROTO((TIFF*, ttile_t, tdata_t, tsize_t));
extern	tsize_t TIFFWriteEncodedStrip ___PROTO((TIFF*, tstrip_t, tdata_t, tsize_t));
extern	tsize_t TIFFWriteRawStrip ___PROTO((TIFF*, tstrip_t, tdata_t, tsize_t));
extern	tsize_t TIFFWriteEncodedTile ___PROTO((TIFF*, ttile_t, tdata_t, tsize_t));
extern	tsize_t TIFFWriteRawTile ___PROTO((TIFF*, ttile_t, tdata_t, tsize_t));
extern	void TIFFSetWriteOffset ___PROTO((TIFF*, toff_t));
extern	void TIFFSwabShort ___PROTO((uint16 *));
extern	void TIFFSwabLong ___PROTO((uint32 *));
extern	void TIFFSwabArrayOfShort ___PROTO((uint16 *, unsigned long));
extern	void TIFFSwabArrayOfLong ___PROTO((uint32 *, unsigned long));
extern	void TIFFReverseBits ___PROTO((unsigned char *, unsigned long));
extern	 unsigned char* TIFFGetBitRevTable ___PROTO((int));

extern	void TIFFModeCCITTFax3 ___PROTO((TIFF* tif, int isClassF));	/* XXX */
#if defined(__cplusplus)
}
#endif
#endif /* _TIFFIO_ */
