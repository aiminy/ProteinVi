
#ifndef __IMAGE_IO
#define __IMAGE_IO

//jpeg codec

/*compress and write to an jpeg image, quality is between 0 and 100
 */
void compress(unsigned char* array, char* file_name, int w, int h, int ch, int quality=80);

/*decompress from a file, user of this function is responsible for reclaiming the image buffer
 */
unsigned char* decompress(char* file_name, int &w, int &h, int& ch);

//png codec

#ifdef __APPLE
  #define GAMA .60
#else
  #define GAMA 1.0
#endif

/*write char* buffer to file, given width, height, 0 if no alpha channel
 */
int write_png(char *file_name, unsigned char* buffer, int width, int height, int hasAlpha);

/*open png file, return image data, also gives back width, height, number of channels and if hasAlpha==0, strip alpha channel
 */
unsigned char* open_png(char* filename, int &w, int &h, int &ch, int hasAlpha);
#endif
