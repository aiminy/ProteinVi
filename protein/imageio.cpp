#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include <jpeglib.h>
#include <jconfig.h>
#include <png.h>
#include "imageio.h"

#define TRANSFORM PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND \
| PNG_TRANSFORM_PACKING

#define TRANSFORM_NA PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_EXPAND \
| PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_ALPHA


/**
 * open png file, read file into RGBA char*
 */
unsigned char* open_png(char* filename, int &w, int &h, int &ch, int hasAlpha){
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned int sig_read = 0;
  png_uint_32 width, height;
  //int bit_depth, color_type, interlace_type;
  FILE *fp;
  
  if ((fp = fopen(filename, "rb")) == NULL){
    return NULL;
  }
  
  /* Create and initialize the png_struct with the desired error handler
   * functions.  If you want to use the default stderr and longjump method,
   * you can supply NULL for the last three parameters.  We also supply the
   * the compiler header file version, so that we know if the application
   * was compiled with a compatible version of the library.  REQUIRED
   */
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL, NULL, NULL);
  
  if (png_ptr == NULL){
    fclose(fp);
    return NULL;
  }
  
  /* Allocate/initialize the memory for image information.  REQUIRED. */
  info_ptr = png_create_info_struct(png_ptr);
  if (info_ptr == NULL){
    fclose(fp);
    png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
    return NULL;
  }

  /* Set up the input control if you are using standard C streams */
  png_init_io(png_ptr, fp);
  if (hasAlpha)
    png_read_png(png_ptr, info_ptr, TRANSFORM, png_voidp_NULL);
  else
    png_read_png(png_ptr, info_ptr, TRANSFORM_NA, png_voidp_NULL);
  /* At this point you have read the entire image */
  png_byte** img = png_get_rows(png_ptr, info_ptr);
  
  w = png_get_image_width(png_ptr, info_ptr);
  h = png_get_image_height(png_ptr, info_ptr);
  ch = png_get_channels(png_ptr, info_ptr);
  
  unsigned char* buffer = (unsigned char*)malloc(w * h * ch);
  for (int i = 0; i < h; i++){
    memcpy(&buffer[i * w * ch], img[i], w * ch);
    delete [] img[i];
    img[i] = NULL;
  }
  
  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
  
  fclose(fp);
  return buffer;
}

/** write RGBA char* to file
 *  return 0 upon success
 */

int write_png(char *file_name, unsigned char* buffer, int width, int height, int hasAlpha){
   FILE *fp;
   png_structp png_ptr;
   png_infop info_ptr;
   png_colorp palette;

   /* open the file */
   fp = fopen(file_name, "wb");
   if (fp == NULL)
     return -1;

   //allocate struct
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

   if (png_ptr == NULL){
      fclose(fp);
      return -1;
   }

   /* Allocate/initialize the image information data.  REQUIRED */
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL){
     fclose(fp);
     png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
     return -1;
   }

   /* set up the output control if you are using standard C streams */
   png_init_io(png_ptr, fp);
   
   png_set_IHDR(png_ptr, info_ptr, width, height, 8, (hasAlpha) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
   
   png_set_gAMA(png_ptr, info_ptr, GAMA);
   
   /* otherwise, if we are dealing with a color image then */
   
   png_color_8 sig_bit;
   sig_bit.red = 8;
   sig_bit.green = 8;
   sig_bit.blue = 8;
   sig_bit.alpha = hasAlpha ? 8 : 0;
   
   png_set_sBIT(png_ptr, info_ptr, &sig_bit);
   
   png_text_struct text_ptr[1]; 
   text_ptr[0].key = "Author";
   text_ptr[0].text = "Clearsky";
   text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
   png_set_text(png_ptr, info_ptr, text_ptr, 1);

   /* ... set palette colors ... */
   palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH
             * sizeof (png_color));

   png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);

   /* Write the file header information.  REQUIRED */
   png_write_info(png_ptr, info_ptr);

   int d = hasAlpha ? 4 : 3;
   png_byte** row_pointers =
     (png_byte**)malloc(width * height * d);
   for (int k = 0; k < height; k++)
     row_pointers[k] = (png_byte*)buffer + k*width*d;
   
   png_write_image(png_ptr, row_pointers);

   /* It is REQUIRED to call this to finish writing the rest of the file */
   png_write_end(png_ptr, info_ptr);
   
   png_destroy_write_struct(&png_ptr, &info_ptr);

   /* close the file */
   fclose(fp);
   delete [] row_pointers;
   delete [] palette;
   /* that's it */
   return 0;
}

unsigned char* decompress(char* file_name, int &w, int &h, int& d){
    
  FILE				*fp;		// File pointer
  struct jpeg_decompress_struct	cinfo;		// Decompressor info
  struct jpeg_error_mgr		jerr;		// Error handler info
  JSAMPROW			row;		// Sample row pointer
  unsigned char* array;
  
  if ((fp = fopen("img.jpeg", "rb")) == NULL) return 0;

  cinfo.err = jpeg_std_error(&jerr);
  jerr.error_exit = 0; //jpeg_error_handler;

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, 1);

  cinfo.quantize_colors      = (boolean)FALSE;
  cinfo.out_color_space      = JCS_RGB;
  cinfo.out_color_components = 3;
  cinfo.output_components    = 3;

  jpeg_calc_output_dimensions(&cinfo);

  w = cinfo.output_width;
  h = cinfo.output_height;
  d = cinfo.output_components;

  array = (unsigned char*)malloc(w * h * d);

  jpeg_start_decompress(&cinfo);

  while (cinfo.output_scanline < cinfo.output_height)
  {
    row = (JSAMPROW)(array +
                     cinfo.output_scanline * cinfo.output_width *
                     cinfo.output_components);
    jpeg_read_scanlines(&cinfo, &row, (JDIMENSION)1);
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(fp);
 
  return array;
}

void compress(unsigned char* array, char* file_name, int w, int h, int d, int quality){
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  
  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
 
  FILE * outfile;

  if ((outfile = fopen(file_name, "wb")) == NULL) {
    fprintf(stderr, "can't open file to write\n" );
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);
  cinfo.image_width = w;
  cinfo.image_height = h;
  cinfo.input_components = d;
  cinfo.in_color_space = JCS_RGB;
  
  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE); /** 0-100 */

  jpeg_start_compress(&cinfo, TRUE);
  
  JSAMPROW row_pointer[1];
  int row_stride = w * d;
  while (cinfo.next_scanline < cinfo.image_height){
    row_pointer[0] = &array[cinfo.next_scanline * row_stride];
    jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
  
  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  fclose(outfile);
}
