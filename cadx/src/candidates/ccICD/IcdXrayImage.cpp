#include "IcdXrayImage.h"

//
// read a KESPR image from a file into this object
//
// I use c-style file i/o here instead of c++-style i/o. Sue me.
//
void IcdXrayImage::kesprRead(const std::string &filename)
{
    FILE *fp = fopen(filename.c_str(), "rb");

      // complain if file can't be read
    if(!fp)
		throw std::string("Image (kespr) file could not be found");

    char header[2048];
    fread(header, 2048, 1, fp);

    int rows, cols;

      // These "magic numbers" are header byte offsets provided by
      // B. Senn, 4/2003.
    sscanf(header+608, "%d", &rows);
    sscanf(header+624, "%d", &cols);
    sscanf(header+640, "%lf", &pix_spacing);

      // allocate space for image
    short *space = new short[rows * cols];

      // read image data
    fread(space, 2, rows * cols, fp);

#ifdef WIN32
      // correct for byte ordering problem on NT
      // (i.e. swap low-order and high-order bytes of each 16-bit word)
    char *byte_space = (char *)space;
    for(int i=0; i<rows*cols*2; i+=2)
    {
        char temp;
        temp=byte_space[i];
        byte_space[i]=byte_space[i+1];
        byte_space[i+1] = temp;
    }    
#endif

      // create underlying IEM plane. The 'true' here means that IEM
      // will take over memory management of the 'space' buffer, so 
      // no need to worry about memory leaks.
    IemPlane plane(IemShort, rows, cols, space, true);
    xray_image = plane;

    fclose(fp);
}
