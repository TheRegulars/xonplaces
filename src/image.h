
#ifndef IMAGE_H
#define IMAGE_H

extern int image_width, image_height;

typedef enum save_img_e {
    SAVEIMG_JPEG,
    SAVEIMG_PNG,
    SAVEIMG_BMP
} saveimg_t;

struct saveimg_jpeg_params {
    int quality;
};

struct saveimg_png_params {
    qboolean has_alpha;
};

typedef union {
    struct saveimg_jpeg_params jpeg;
    struct saveimg_png_params png;
} saveimg_params_t;


// swizzle components (even converting number of components) and flip images
// (warning: input must be different than output due to non-linear read/write)
// (tip: component indices can contain values | 0x80000000 to tell it to
// store them directly into output, so 255 | 0x80000000 would write 255)
void Image_CopyMux(unsigned char *outpixels, const unsigned char *inpixels, int inputwidth, int inputheight, qboolean inputflipx, qboolean inputflipy, qboolean inputflipdiagonal, int numoutputcomponents, int numinputcomponents, int *outputinputcomponentindices);

// converts 8bit image data to BGRA, in can not be the same as out
void Image_Copy8bitBGRA(const unsigned char *in, unsigned char *out, int pixels, const unsigned int *pal);

void Image_StripImageExtension (const char *in, char *out, size_t size_out);

// loads a texture, as pixel data
unsigned char *loadimagepixelsbgra (const char *filename, qboolean complain, qboolean allowFixtrans, qboolean convertsRGB, int *miplevel);

// loads a texture, as a texture
rtexture_t *loadtextureimage (rtexturepool_t *pool, const char *filename, qboolean complain, int flags, qboolean allowFixtrans, qboolean sRGB);

// writes an upside down BGR image into a TGA
qboolean Image_WriteTGABGR_preflipped (const char *filename, int width, int height, const unsigned char *data);

// writes a BGRA image into a TGA file
qboolean Image_WriteTGABGRA (const char *filename, int width, int height, const unsigned char *data);

// resizes the image (in can not be the same as out)
void Image_Resample32(const void *indata, int inwidth, int inheight, int indepth, void *outdata, int outwidth, int outheight, int outdepth, int quality);

// scales the image down by a power of 2 (in can be the same as out)
void Image_MipReduce32(const unsigned char *in, unsigned char *out, int *width, int *height, int *depth, int destwidth, int destheight, int destdepth);

void Image_HeightmapToNormalmap_BGRA(const unsigned char *inpixels, unsigned char *outpixels, int width, int height, int clamp, float bumpscale);

#define Image_LinearFloatFromsRGBFloat(c) (((c) <= 0.04045f) ? (c) * (1.0f / 12.92f) : (float)pow(((c) + 0.055f)*(1.0f/1.055f), 2.4f))
#define Image_sRGBFloatFromLinearFloat(c) (((c) < 0.0031308f) ? (c) * 12.92f : 1.055f * (float)pow((c), 1.0f/2.4f) - 0.055f)
#define Image_LinearFloatFromsRGB(c) Image_LinearFloatFromsRGBFloat((c) * (1.0f / 255.0f))
#define Image_sRGBFloatFromLinear(c) Image_sRGBFloatFromLinearFloat((c) * (1.0f / 255.0f))
#define Image_sRGBFloatFromLinear_Lightmap(c) Image_sRGBFloatFromLinearFloat((c) * (2.0f / 255.0f)) * 0.5f

void Image_MakeLinearColorsFromsRGB(unsigned char *pout, const unsigned char *pin, int numpixels);
void Image_MakesRGBColorsFromLinear_Lightmap(unsigned char *pout, const unsigned char *pin, int numpixels);

void Image_Init();
void Image_Shutdown();

#ifndef DEDICATED_SERVER
unsigned char* Load_SDL_Image_BGRA(const char* filename, const char* type);
unsigned char* Load_SDL_Image_MEM_BGRA(const unsigned char *raw, int filesize, const char* type);

// returns 0 on success, -1 on error
int Image_SaveIMG (const char *filename, int width, int height, unsigned char *data,
                   saveimg_t format, saveimg_params_t *params);

#endif // DEDICATED_SERVER

#endif

