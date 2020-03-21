/*---------------------------------------------------------------------------

   wpng - simple PNG-writing program                             writepng.c

  ---------------------------------------------------------------------------

      Copyright (c) 1998-2000 Greg Roelofs.  All rights reserved.

      This software is provided "as is," without warranty of any kind,
      express or implied.  In no event shall the author or contributors
      be held liable for any damages arising in any way from the use of
      this software.

      Permission is granted to anyone to use this software for any purpose,
      including commercial applications, and to alter it and redistribute
      it freely, subject to the following restrictions:

      1. Redistributions of source code must retain the above copyright
         notice, disclaimer, and this list of conditions.
      2. Redistributions in binary form must reproduce the above copyright
         notice, disclaimer, and this list of conditions in the documenta-
         tion and/or other materials provided with the distribution.
      3. All advertising materials mentioning features or use of this
         software must display the following acknowledgment:

            This product includes software developed by Greg Roelofs
            and contributors for the book, "PNG: The Definitive Guide,"
            published by O'Reilly and Associates.

  ---------------------------------------------------------------------------*/


#include <stdlib.h>     /* for exit() prototype */


#include "PNGWriter.h"   /* typedefs, common macros, public prototypes */


/* local prototype */



PNGWriter::PNGWriter() {


    gamma = 0;
    width = 0;
    height = 0;
    modtime = 0;
    infile = NULL;
    outfile = NULL;
    png_ptr = NULL;
    info_ptr = NULL;
    image_data = NULL;
    strcpy(title, "");
    strcpy(author, "");
    desc = NULL;
    strcpy(copyright, "");
    strcpy(email, "");
    strcpy(url, "");
    color_type = PNG_COLOR_TYPE_GRAY;
    bit_depth = 8;
    interlace_type = PNG_INTERLACE_NONE;
    have_bg = 0;
    have_time = 0;
    have_text = 0;
    bg_red = 0;
    bg_green = 0;
    bg_blue = 0;


    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL,
      NULL, NULL);
    if (!png_ptr)
        return;   /* out of memory */

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, NULL);
        return;   /* out of memory */
    }
}


int PNGWriter::write(char *fileName, IemTImage<short>& img) {

	outfile = fopen(fileName, "wb");

	width = img.cols();
	height = img.rows();

	bit_depth = 16;

	if(img.chans() == 1) color_type = PNG_COLOR_TYPE_GRAY;
	else if(img.chans() == 3) color_type = PNG_COLOR_TYPE_RGB;

	writepng_init();

	png_byte **row_pointers = new png_byte*[height];

	for(long r = 0; r < height; r++) {

		row_pointers[r] = new png_byte[2 * width];
		
		for(long c = 0, cc = 0; c < width; c++, cc += 2) {
		
			long v = img[0][r][c];
		
			// Save as big endian.
			row_pointers[r][cc] = (v >> 8);
			row_pointers[r][cc+1] = v & 0x00FF;
		}
	}


	writepng_encode_image(row_pointers);


	delete[] row_pointers;


	return 0;
}




/* returns 0 for success, 2 for libpng problem, 4 for out of memory, 11 for
 *  unexpected pnmtype; note that outfile might be stdout */

int PNGWriter::writepng_init() {

    /* setjmp() must be called in every function that calls a PNG-writing
     * libpng function, unless an alternate error handler was installed--
     * but compatible error handlers must either use longjmp() themselves
     * (as in this program) or exit immediately, so here we go: */
    if(setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return 2;
    }


    /* make sure outfile is (re)opened in BINARY mode */
    png_init_io(png_ptr, outfile);


    /* set the compression levels--in general, always want to leave filtering
     * turned on (except for palette images) and allow all of the filters,
     * which is the default; want 32K zlib window, unless entire image buffer
     * is 16K or smaller (unknown here)--also the default; usually want max
     * compression (NOT the default); and remaining compression flags should
     * be left alone */

    png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);
/*
    >> this is default for no filtering; Z_FILTERED is default otherwise:
    png_set_compression_strategy(png_ptr, Z_DEFAULT_STRATEGY);
    >> these are all defaults:
    png_set_compression_mem_level(png_ptr, 8);
    png_set_compression_window_bits(png_ptr, 15);
    png_set_compression_method(png_ptr, 8);
 */

    png_set_IHDR(png_ptr, info_ptr, width, height,
      bit_depth, color_type, interlace_type,
      PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    if(gamma > 0.0)
        png_set_gAMA(png_ptr, info_ptr, gamma);

    if(have_bg) {   /* we know it's RGBA, not gray+alpha */
        png_color_16  background;

        background.red = bg_red;
        background.green = bg_green;
        background.blue = bg_blue;
        png_set_bKGD(png_ptr, info_ptr, &background);
    }

    if (have_time) {
        png_time  modtime;

     //   png_convert_from_time_t(&modtime, modtime);
        png_set_tIME(png_ptr, info_ptr, &modtime);
    }

    if (have_text) {
        png_text  text[6];
        int  num_text = 0;

        if (have_text & TEXT_TITLE) {
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Title";
            text[num_text].text = title;
            ++num_text;
        }
        if (have_text & TEXT_AUTHOR) {
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Author";
            text[num_text].text = author;
            ++num_text;
        }
        if (have_text & TEXT_DESC) {
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Description";
            text[num_text].text = desc;
            ++num_text;
        }
        if (have_text & TEXT_COPY) {
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "Copyright";
            text[num_text].text = copyright;
            ++num_text;
        }
        if (have_text & TEXT_EMAIL) {
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "E-mail";
            text[num_text].text = email;
            ++num_text;
        }
        if (have_text & TEXT_URL) {
            text[num_text].compression = PNG_TEXT_COMPRESSION_NONE;
            text[num_text].key = "URL";
            text[num_text].text = url;
            ++num_text;
        }
        png_set_text(png_ptr, info_ptr, text, num_text);
    }


    /* write all chunks up to (but not including) first IDAT */
    png_write_info(png_ptr, info_ptr);


    /* if we wanted to write any more text info *after* the image data, we
     * would set up text struct(s) here and call png_set_text() again, with
     * just the new data; png_set_tIME() could also go here, but it would
     * have no effect since we already called it above (only one tIME chunk
     * allowed) */


    /* set up the transformations:  for now, just pack low-bit-depth pixels
     * into bytes (one, two or four pixels per byte) */
 //   png_set_packing(png_ptr);
/*  png_set_shift(png_ptr, &sig_bit);  to scale low-bit-depth values */


    return 0;
}



int PNGWriter::writepng_encode_image(png_byte **row_pointers) {

    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */
    if(setjmp(jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        png_ptr = NULL;
        info_ptr = NULL;
        return 2;
    }

    /* and now we just write the whole image; libpng takes care of interlacing
     * for us */
    png_write_image(png_ptr, row_pointers);


    /* since that's it, we also close out the end of the PNG file now--if we
     * had any text or time info to write after the IDATs, second argument
     * would be info_ptr, but we optimize slightly by sending NULL pointer: */
    png_write_end(png_ptr, NULL);

    return 0;
}



/* returns 0 if succeeds, 2 if libpng problem */

int PNGWriter::writepng_encode_row() { /* NON-interlaced only! */


    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */

    if(setjmp(jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        png_ptr = NULL;
        info_ptr = NULL;
        return 2;
    }


    /* image_data points at our one row of image data */
    png_write_row(png_ptr, image_data);

    return 0;
}



/* returns 0 if succeeds, 2 if libpng problem */

int PNGWriter::writepng_encode_finish() {  /* NON-interlaced! */

    /* as always, setjmp() must be called in every function that calls a
     * PNG-writing libpng function */

    if (setjmp(jmpbuf)) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        png_ptr = NULL;
        info_ptr = NULL;
        return 2;
    }


    /* close out PNG file; if we had any text or time info to write after
     * the IDATs, second argument would be info_ptr: */

    png_write_end(png_ptr, NULL);

    return 0;
}



void PNGWriter::writepng_cleanup() {

    if (png_ptr && info_ptr)
        png_destroy_write_struct(&png_ptr, &info_ptr);
}


void PNGWriter::writepng_version_info() {
  fprintf(stderr, "   Compiled with libpng %s; using libpng %s.\n",
    PNG_LIBPNG_VER_STRING, png_libpng_ver);
  fprintf(stderr, "   Compiled with zlib %s; using zlib %s.\n",
    ZLIB_VERSION, zlib_version);
}

