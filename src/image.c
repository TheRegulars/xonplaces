
#include "quakedef.h"
#include "image.h"
#include "r_shadow.h"

#ifndef DEDICATED_SERVER
#include <SDL.h>
#include <SDL_image.h>
#endif // DEDICATED_SERVER

int        image_width;
int        image_height;


#ifndef DEDICATED_SERVER
static void Image_CopyAlphaFromBlueBGRA(unsigned char *outpixels, const unsigned char *inpixels, int w, int h)
{
    int i, n;
    n = w * h;
    for(i = 0; i < n; ++i)
        outpixels[4*i+3] = inpixels[4*i]; // blue channel
}

#endif // DEDICATED_SERVER

#if 1
// written by LordHavoc in a readable way, optimized by Vic, further optimized by LordHavoc (the non-special index case), readable version preserved below this
void Image_CopyMux(unsigned char *outpixels, const unsigned char *inpixels, int inputwidth, int inputheight, qboolean inputflipx, qboolean inputflipy, qboolean inputflipdiagonal, int numoutputcomponents, int numinputcomponents, int *outputinputcomponentindices)
{
    int index, c, x, y;
    const unsigned char *in, *line;
    int row_inc = (inputflipy ? -inputwidth : inputwidth) * numinputcomponents, col_inc = (inputflipx ? -1 : 1) * numinputcomponents;
    int row_ofs = (inputflipy ? (inputheight - 1) * inputwidth * numinputcomponents : 0), col_ofs = (inputflipx ? (inputwidth - 1) * numinputcomponents : 0);

    for (c = 0; c < numoutputcomponents; c++)
        if (outputinputcomponentindices[c] & 0x80000000)
            break;
    if (c < numoutputcomponents)
    {
        // special indices used
        if (inputflipdiagonal)
        {
            for (x = 0, line = inpixels + col_ofs; x < inputwidth; x++, line += col_inc)
                for (y = 0, in = line + row_ofs; y < inputheight; y++, in += row_inc, outpixels += numoutputcomponents)
                    for (c = 0; c < numoutputcomponents; c++)
                        outpixels[c] = ((index = outputinputcomponentindices[c]) & 0x80000000) ? index : in[index];
        }
        else
        {
            for (y = 0, line = inpixels + row_ofs; y < inputheight; y++, line += row_inc)
                for (x = 0, in = line + col_ofs; x < inputwidth; x++, in += col_inc, outpixels += numoutputcomponents)
                    for (c = 0; c < numoutputcomponents; c++)
                        outpixels[c] = ((index = outputinputcomponentindices[c]) & 0x80000000) ? index : in[index];
        }
    }
    else
    {
        // special indices not used
        if (inputflipdiagonal)
        {
            for (x = 0, line = inpixels + col_ofs; x < inputwidth; x++, line += col_inc)
                for (y = 0, in = line + row_ofs; y < inputheight; y++, in += row_inc, outpixels += numoutputcomponents)
                    for (c = 0; c < numoutputcomponents; c++)
                        outpixels[c] = in[outputinputcomponentindices[c]];
        }
        else
        {
            for (y = 0, line = inpixels + row_ofs; y < inputheight; y++, line += row_inc)
                for (x = 0, in = line + col_ofs; x < inputwidth; x++, in += col_inc, outpixels += numoutputcomponents)
                    for (c = 0; c < numoutputcomponents; c++)
                        outpixels[c] = in[outputinputcomponentindices[c]];
        }
    }
}
#else
// intentionally readable version
void Image_CopyMux(unsigned char *outpixels, const unsigned char *inpixels, int inputwidth, int inputheight, qboolean inputflipx, qboolean inputflipy, qboolean inputflipdiagonal, int numoutputcomponents, int numinputcomponents, int *outputinputcomponentindices)
{
    int index, c, x, y;
    const unsigned char *in, *inrow, *incolumn;
    if (inputflipdiagonal)
    {
        for (x = 0;x < inputwidth;x++)
        {
            for (y = 0;y < inputheight;y++)
            {
                in = inpixels + ((inputflipy ? inputheight - 1 - y : y) * inputwidth + (inputflipx ? inputwidth - 1 - x : x)) * numinputcomponents;
                for (c = 0;c < numoutputcomponents;c++)
                {
                    index = outputinputcomponentindices[c];
                    if (index & 0x80000000)
                        *outpixels++ = index;
                    else
                        *outpixels++ = in[index];
                }
            }
        }
    }
    else
    {
        for (y = 0;y < inputheight;y++)
        {
            for (x = 0;x < inputwidth;x++)
            {
                in = inpixels + ((inputflipy ? inputheight - 1 - y : y) * inputwidth + (inputflipx ? inputwidth - 1 - x : x)) * numinputcomponents;
                for (c = 0;c < numoutputcomponents;c++)
                {
                    index = outputinputcomponentindices[c];
                    if (index & 0x80000000)
                        *outpixels++ = index;
                    else
                        *outpixels++ = in[index];
                }
            }
        }
    }
}
#endif

// note: pal must be 32bit color
void Image_Copy8bitBGRA(const unsigned char *in, unsigned char *out, int pixels, const unsigned int *pal)
{
    int *iout = (int *)out;
    while (pixels >= 8)
    {
        iout[0] = pal[in[0]];
        iout[1] = pal[in[1]];
        iout[2] = pal[in[2]];
        iout[3] = pal[in[3]];
        iout[4] = pal[in[4]];
        iout[5] = pal[in[5]];
        iout[6] = pal[in[6]];
        iout[7] = pal[in[7]];
        in += 8;
        iout += 8;
        pixels -= 8;
    }
    if (pixels & 4)
    {
        iout[0] = pal[in[0]];
        iout[1] = pal[in[1]];
        iout[2] = pal[in[2]];
        iout[3] = pal[in[3]];
        in += 4;
        iout += 4;
    }
    if (pixels & 2)
    {
        iout[0] = pal[in[0]];
        iout[1] = pal[in[1]];
        in += 2;
        iout += 2;
    }
    if (pixels & 1)
        iout[0] = pal[in[0]];
}

void Image_StripImageExtension (const char *in, char *out, size_t size_out)
{
    const char *ext;

    if (size_out == 0)
        return;

    ext = FS_FileExtension(in);
    if (ext && (!strcmp(ext, "tga") || !strcmp(ext, "pcx") || !strcmp(ext, "lmp") || !strcmp(ext, "png") || !strcmp(ext, "jpg") || !strcmp(ext, "wal")))
        FS_StripExtension(in, out, size_out);
    else
        strlcpy(out, in, size_out);
}

static unsigned char image_linearfromsrgb[256];
static unsigned char image_srgbfromlinear_lightmap[256];

void Image_MakeLinearColorsFromsRGB(unsigned char *pout, const unsigned char *pin, int numpixels)
{
    int i;
    // this math from http://www.opengl.org/registry/specs/EXT/texture_sRGB.txt
    if (!image_linearfromsrgb[255])
        for (i = 0;i < 256;i++)
            image_linearfromsrgb[i] = (unsigned char)floor(Image_LinearFloatFromsRGB(i) * 255.0f + 0.5f);
    for (i = 0;i < numpixels;i++)
    {
        pout[i*4+0] = image_linearfromsrgb[pin[i*4+0]];
        pout[i*4+1] = image_linearfromsrgb[pin[i*4+1]];
        pout[i*4+2] = image_linearfromsrgb[pin[i*4+2]];
        pout[i*4+3] = pin[i*4+3];
    }
}

void Image_MakesRGBColorsFromLinear_Lightmap(unsigned char *pout, const unsigned char *pin, int numpixels)
{
    int i;
    // this math from http://www.opengl.org/registry/specs/EXT/texture_sRGB.txt
    if (!image_srgbfromlinear_lightmap[255])
        for (i = 0;i < 256;i++)
            image_srgbfromlinear_lightmap[i] = (unsigned char)floor(bound(0.0f, Image_sRGBFloatFromLinear_Lightmap(i), 1.0f) * 255.0f + 0.5f);
    for (i = 0;i < numpixels;i++)
    {
        pout[i*4+0] = image_srgbfromlinear_lightmap[pin[i*4+0]];
        pout[i*4+1] = image_srgbfromlinear_lightmap[pin[i*4+1]];
        pout[i*4+2] = image_srgbfromlinear_lightmap[pin[i*4+2]];
        pout[i*4+3] = pin[i*4+3];
    }
}

typedef struct imageformat_s
{
    const char *formatstring;
    const char *type;
}
imageformat_t;

imageformat_t imageformats_nopath[] =
{
    {"override/%s.tga", "TGA"},
    {"override/%s.png", "PNG"},
    {"override/%s.jpg", "JPG"},
    {"textures/%s.tga", "TGA"},
    {"textures/%s.png", "PNG"},
    {"textures/%s.jpg", "JPG"},
    {"%s.tga", "TGA"},
    {"%s.png", "PNG"},
    {"%s.jpg", "JPG"},
    {"%s.pcx", "PCX"},
    {NULL, NULL}
};

imageformat_t imageformats_textures[] =
{
    {"%s.tga", "TGA"},
    {"%s.png", "PNG"},
    {"%s.jpg", "JPG"},
    {"%s.pcx", "PCX"},
    {NULL, NULL}
};

imageformat_t imageformats_gfx[] =
{
    {"%s.tga", "TGA"},
    {"%s.png", "PNG"},
    {"%s.jpg", "JPG"},
    {"%s.pcx", "PCX"},
    {NULL, NULL}
};

imageformat_t imageformats_other[] =
{
    {"%s.tga", "TGA"},
    {"%s.png", "PNG"},
    {"%s.jpg", "JPG"},
    {"%s.pcx", "PCX"},
    {NULL, NULL}
};

#ifndef DEDICATED_SERVER

unsigned char *loadimagepixelsbgra (const char *filename, qboolean complain, qboolean allowFixtrans, qboolean convertsRGB, int *miplevel)
{
    imageformat_t *firstformat, *format;
    unsigned char *data = NULL, *data2 = NULL;
    char basename[MAX_QPATH], name[MAX_QPATH], name2[MAX_QPATH], *c;
    char vabuf[1024];
    //if (developer_memorydebug.integer)
    //    Mem_CheckSentinelsGlobal();
    if (developer_texturelogging.integer)
        Log_Printf("textures.log", "%s\n", filename);
    Image_StripImageExtension(filename, basename, sizeof(basename)); // strip filename extensions to allow replacement by other types
    // replace *'s with #, so commandline utils don't get confused when dealing with the external files
    for (c = basename;*c;c++)
        if (*c == '*')
            *c = '#';
    name[0] = 0;
    if (strchr(basename, '/'))
    {
        int i;
        for (i = 0;i < (int)sizeof(name)-1 && basename[i] != '/';i++)
            name[i] = basename[i];
        name[i] = 0;
    }
    if (!strcasecmp(name, "textures"))
        firstformat = imageformats_textures;
    else if (!strcasecmp(name, "gfx"))
        firstformat = imageformats_gfx;
    else if (!strchr(basename, '/'))
        firstformat = imageformats_nopath;
    else
        firstformat = imageformats_other;
    // now try all the formats in the selected list
    for (format = firstformat;format->formatstring;format++)
    {
        dpsnprintf (name, sizeof(name), format->formatstring, basename);
        int mymiplevel = miplevel ? *miplevel : 0;
        image_width = 0;
        image_height = 0;
        data = Load_SDL_Image_BGRA(name, format->type);
        if (data)
        {
            if(strcasecmp(format->type, "JPG") == 0) // jpeg can't do alpha, so let's simulate it by loading another jpeg
            {
                dpsnprintf (name2, sizeof(name2), format->formatstring, va(vabuf, sizeof(vabuf), "%s_alpha", basename));

                int mymiplevel2 = miplevel ? *miplevel : 0;
                int image_width_save = image_width;
                int image_height_save = image_height;
                data2 = Load_SDL_Image_BGRA(name2, "JPG");
                if(data2 && mymiplevel == mymiplevel2 && image_width == image_width_save && image_height == image_height_save) {
                    Image_CopyAlphaFromBlueBGRA(data, data2, image_width, image_height);
                }
                image_width = image_width_save;
                image_height = image_height_save;
                if(data2)
                    Mem_Free(data2);
            }
            if (developer_loading.integer)
                Con_DPrintf("loaded image %s (%dx%d)\n", name, image_width, image_height);
            if(miplevel)
                *miplevel = mymiplevel;
            //if (developer_memorydebug.integer)
            //    Mem_CheckSentinelsGlobal();
            if (convertsRGB)
                Image_MakeLinearColorsFromsRGB(data, data, image_width * image_height);
            return data;
        }
        else
            Con_DPrintf("Error loading image %s (file loaded but decode failed)\n", name);
    }
    if (complain)
    {
        Con_Printf("Couldn't load %s using ", filename);
        for (format = firstformat;format->formatstring;format++)
        {
            dpsnprintf (name, sizeof(name), format->formatstring, basename);
            Con_Printf(format == firstformat ? "\"%s\"" : (format[1].formatstring ? ", \"%s\"" : " or \"%s\".\n"), format->formatstring);
        }
    }

    // texture loading can take a while, so make sure we're sending keepalives
    CL_KeepaliveMessage(false);

    //if (developer_memorydebug.integer)
    //    Mem_CheckSentinelsGlobal();
    return NULL;
}

extern cvar_t gl_picmip;
rtexture_t *loadtextureimage (rtexturepool_t *pool, const char *filename, qboolean complain, int flags, qboolean allowFixtrans, qboolean sRGB)
{
    unsigned char *data;
    rtexture_t *rt;
    int miplevel = R_PicmipForFlags(flags);
    if (!(data = loadimagepixelsbgra (filename, complain, allowFixtrans, false, &miplevel)))
        return 0;
    rt = R_LoadTexture2D(pool, filename, image_width, image_height, data, sRGB ? TEXTYPE_SRGB_BGRA : TEXTYPE_BGRA, flags, miplevel, NULL);
    Mem_Free(data);
    return rt;
}

#endif // DEDICATED_SERVER

qboolean Image_WriteTGABGR_preflipped (const char *filename, int width, int height, const unsigned char *data)
{
    qboolean ret;
    unsigned char buffer[18];
    const void *buffers[2];
    fs_offset_t sizes[2];

    memset (buffer, 0, 18);
    buffer[2] = 2;        // uncompressed type
    buffer[12] = (width >> 0) & 0xFF;
    buffer[13] = (width >> 8) & 0xFF;
    buffer[14] = (height >> 0) & 0xFF;
    buffer[15] = (height >> 8) & 0xFF;
    buffer[16] = 24;    // pixel size

    buffers[0] = buffer;
    sizes[0] = 18;
    buffers[1] = data;
    sizes[1] = width*height*3;
    ret = FS_WriteFileInBlocks(filename, buffers, sizes, 2);

    return ret;
}

qboolean Image_WriteTGABGRA (const char *filename, int width, int height, const unsigned char *data)
{
    int y;
    unsigned char *buffer, *out;
    const unsigned char *in, *end;
    qboolean ret;

    buffer = (unsigned char *)Mem_Alloc(tempmempool, width*height*4 + 18);

    memset (buffer, 0, 18);
    buffer[2] = 2;        // uncompressed type
    buffer[12] = (width >> 0) & 0xFF;
    buffer[13] = (width >> 8) & 0xFF;
    buffer[14] = (height >> 0) & 0xFF;
    buffer[15] = (height >> 8) & 0xFF;

    for (y = 3;y < width*height*4;y += 4)
        if (data[y] < 255)
            break;

    if (y < width*height*4)
    {
        // save the alpha channel
        buffer[16] = 32;    // pixel size
        buffer[17] = 8; // 8 bits of alpha

        // flip upside down
        out = buffer + 18;
        for (y = height - 1;y >= 0;y--)
        {
            memcpy(out, data + y * width * 4, width * 4);
            out += width*4;
        }
    }
    else
    {
        // save only the color channels
        buffer[16] = 24;    // pixel size
        buffer[17] = 0; // 8 bits of alpha

        // truncate bgra to bgr and flip upside down
        out = buffer + 18;
        for (y = height - 1;y >= 0;y--)
        {
            in = data + y * width * 4;
            end = in + width * 4;
            for (;in < end;in += 4)
            {
                *out++ = in[0];
                *out++ = in[1];
                *out++ = in[2];
            }
        }
    }
    ret = FS_WriteFile (filename, buffer, out - buffer);

    Mem_Free(buffer);

    return ret;
}

static void Image_Resample32LerpLine (const unsigned char *in, unsigned char *out, int inwidth, int outwidth)
{
    int        j, xi, oldx = 0, f, fstep, endx, lerp;
    fstep = (int) (inwidth*65536.0f/outwidth);
    endx = (inwidth-1);
    for (j = 0,f = 0;j < outwidth;j++, f += fstep)
    {
        xi = f >> 16;
        if (xi != oldx)
        {
            in += (xi - oldx) * 4;
            oldx = xi;
        }
        if (xi < endx)
        {
            lerp = f & 0xFFFF;
            *out++ = (unsigned char) ((((in[4] - in[0]) * lerp) >> 16) + in[0]);
            *out++ = (unsigned char) ((((in[5] - in[1]) * lerp) >> 16) + in[1]);
            *out++ = (unsigned char) ((((in[6] - in[2]) * lerp) >> 16) + in[2]);
            *out++ = (unsigned char) ((((in[7] - in[3]) * lerp) >> 16) + in[3]);
        }
        else // last pixel of the line has no pixel to lerp to
        {
            *out++ = in[0];
            *out++ = in[1];
            *out++ = in[2];
            *out++ = in[3];
        }
    }
}

#define LERPBYTE(i) r = resamplerow1[i];out[i] = (unsigned char) ((((resamplerow2[i] - r) * lerp) >> 16) + r)
static void Image_Resample32Lerp(const void *indata, int inwidth, int inheight, void *outdata, int outwidth, int outheight)
{
    int i, j, r, yi, oldy, f, fstep, lerp, endy = (inheight-1), inwidth4 = inwidth*4, outwidth4 = outwidth*4;
    unsigned char *out;
    const unsigned char *inrow;
    unsigned char *resamplerow1;
    unsigned char *resamplerow2;
    out = (unsigned char *)outdata;
    fstep = (int) (inheight*65536.0f/outheight);

    resamplerow1 = (unsigned char *)Mem_Alloc(tempmempool, outwidth*4*2);
    resamplerow2 = resamplerow1 + outwidth*4;

    inrow = (const unsigned char *)indata;
    oldy = 0;
    Image_Resample32LerpLine (inrow, resamplerow1, inwidth, outwidth);
    Image_Resample32LerpLine (inrow + inwidth4, resamplerow2, inwidth, outwidth);
    for (i = 0, f = 0;i < outheight;i++,f += fstep)
    {
        yi = f >> 16;
        if (yi < endy)
        {
            lerp = f & 0xFFFF;
            if (yi != oldy)
            {
                inrow = (unsigned char *)indata + inwidth4*yi;
                if (yi == oldy+1)
                    memcpy(resamplerow1, resamplerow2, outwidth4);
                else
                    Image_Resample32LerpLine (inrow, resamplerow1, inwidth, outwidth);
                Image_Resample32LerpLine (inrow + inwidth4, resamplerow2, inwidth, outwidth);
                oldy = yi;
            }
            j = outwidth - 4;
            while(j >= 0)
            {
                LERPBYTE( 0);
                LERPBYTE( 1);
                LERPBYTE( 2);
                LERPBYTE( 3);
                LERPBYTE( 4);
                LERPBYTE( 5);
                LERPBYTE( 6);
                LERPBYTE( 7);
                LERPBYTE( 8);
                LERPBYTE( 9);
                LERPBYTE(10);
                LERPBYTE(11);
                LERPBYTE(12);
                LERPBYTE(13);
                LERPBYTE(14);
                LERPBYTE(15);
                out += 16;
                resamplerow1 += 16;
                resamplerow2 += 16;
                j -= 4;
            }
            if (j & 2)
            {
                LERPBYTE( 0);
                LERPBYTE( 1);
                LERPBYTE( 2);
                LERPBYTE( 3);
                LERPBYTE( 4);
                LERPBYTE( 5);
                LERPBYTE( 6);
                LERPBYTE( 7);
                out += 8;
                resamplerow1 += 8;
                resamplerow2 += 8;
            }
            if (j & 1)
            {
                LERPBYTE( 0);
                LERPBYTE( 1);
                LERPBYTE( 2);
                LERPBYTE( 3);
                out += 4;
                resamplerow1 += 4;
                resamplerow2 += 4;
            }
            resamplerow1 -= outwidth4;
            resamplerow2 -= outwidth4;
        }
        else
        {
            if (yi != oldy)
            {
                inrow = (unsigned char *)indata + inwidth4*yi;
                if (yi == oldy+1)
                    memcpy(resamplerow1, resamplerow2, outwidth4);
                else
                    Image_Resample32LerpLine (inrow, resamplerow1, inwidth, outwidth);
                oldy = yi;
            }
            memcpy(out, resamplerow1, outwidth4);
        }
    }

    Mem_Free(resamplerow1);
    resamplerow1 = NULL;
    resamplerow2 = NULL;
}

static void Image_Resample32Nolerp(const void *indata, int inwidth, int inheight, void *outdata, int outwidth, int outheight)
{
    int i, j;
    unsigned frac, fracstep;
    // relies on int being 4 bytes
    int *inrow, *out;
    out = (int *)outdata;

    fracstep = inwidth*0x10000/outwidth;
    for (i = 0;i < outheight;i++)
    {
        inrow = (int *)indata + inwidth*(i*inheight/outheight);
        frac = fracstep >> 1;
        j = outwidth - 4;
        while (j >= 0)
        {
            out[0] = inrow[frac >> 16];frac += fracstep;
            out[1] = inrow[frac >> 16];frac += fracstep;
            out[2] = inrow[frac >> 16];frac += fracstep;
            out[3] = inrow[frac >> 16];frac += fracstep;
            out += 4;
            j -= 4;
        }
        if (j & 2)
        {
            out[0] = inrow[frac >> 16];frac += fracstep;
            out[1] = inrow[frac >> 16];frac += fracstep;
            out += 2;
        }
        if (j & 1)
        {
            out[0] = inrow[frac >> 16];frac += fracstep;
            out += 1;
        }
    }
}

/*
================
Image_Resample
================
*/
void Image_Resample32(const void *indata, int inwidth, int inheight, int indepth, void *outdata, int outwidth, int outheight, int outdepth, int quality)
{
    if (indepth != 1 || outdepth != 1)
    {
        Con_Printf ("Image_Resample: 3D resampling not supported\n");
        return;
    }
    if (quality)
        Image_Resample32Lerp(indata, inwidth, inheight, outdata, outwidth, outheight);
    else
        Image_Resample32Nolerp(indata, inwidth, inheight, outdata, outwidth, outheight);
}

// in can be the same as out
void Image_MipReduce32(const unsigned char *in, unsigned char *out, int *width, int *height, int *depth, int destwidth, int destheight, int destdepth)
{
    const unsigned char *inrow;
    int x, y, nextrow;
    if (*depth != 1 || destdepth != 1)
    {
        Con_Printf ("Image_Resample: 3D resampling not supported\n");
        if (*width > destwidth)
            *width >>= 1;
        if (*height > destheight)
            *height >>= 1;
        if (*depth > destdepth)
            *depth >>= 1;
        return;
    }
    // note: if given odd width/height this discards the last row/column of
    // pixels, rather than doing a proper box-filter scale down
    inrow = in;
    nextrow = *width * 4;
    if (*width > destwidth)
    {
        *width >>= 1;
        if (*height > destheight)
        {
            // reduce both
            *height >>= 1;
            for (y = 0;y < *height;y++, inrow += nextrow * 2)
            {
                for (in = inrow, x = 0;x < *width;x++)
                {
                    out[0] = (unsigned char) ((in[0] + in[4] + in[nextrow  ] + in[nextrow+4]) >> 2);
                    out[1] = (unsigned char) ((in[1] + in[5] + in[nextrow+1] + in[nextrow+5]) >> 2);
                    out[2] = (unsigned char) ((in[2] + in[6] + in[nextrow+2] + in[nextrow+6]) >> 2);
                    out[3] = (unsigned char) ((in[3] + in[7] + in[nextrow+3] + in[nextrow+7]) >> 2);
                    out += 4;
                    in += 8;
                }
            }
        }
        else
        {
            // reduce width
            for (y = 0;y < *height;y++, inrow += nextrow)
            {
                for (in = inrow, x = 0;x < *width;x++)
                {
                    out[0] = (unsigned char) ((in[0] + in[4]) >> 1);
                    out[1] = (unsigned char) ((in[1] + in[5]) >> 1);
                    out[2] = (unsigned char) ((in[2] + in[6]) >> 1);
                    out[3] = (unsigned char) ((in[3] + in[7]) >> 1);
                    out += 4;
                    in += 8;
                }
            }
        }
    }
    else
    {
        if (*height > destheight)
        {
            // reduce height
            *height >>= 1;
            for (y = 0;y < *height;y++, inrow += nextrow * 2)
            {
                for (in = inrow, x = 0;x < *width;x++)
                {
                    out[0] = (unsigned char) ((in[0] + in[nextrow  ]) >> 1);
                    out[1] = (unsigned char) ((in[1] + in[nextrow+1]) >> 1);
                    out[2] = (unsigned char) ((in[2] + in[nextrow+2]) >> 1);
                    out[3] = (unsigned char) ((in[3] + in[nextrow+3]) >> 1);
                    out += 4;
                    in += 4;
                }
            }
        }
        else
            Con_Printf ("Image_MipReduce: desired size already achieved\n");
    }
}

void Image_HeightmapToNormalmap_BGRA(const unsigned char *inpixels, unsigned char *outpixels, int width, int height, int clamp, float bumpscale)
{
    int x, y, x1, x2, y1, y2;
    const unsigned char *b, *row[3];
    int p[5];
    unsigned char *out;
    float ibumpscale, n[3];
    ibumpscale = (255.0f * 6.0f) / bumpscale;
    out = outpixels;
    for (y = 0, y1 = height-1;y < height;y1 = y, y++)
    {
        y2 = y + 1;if (y2 >= height) y2 = 0;
        row[0] = inpixels + (y1 * width) * 4;
        row[1] = inpixels + (y  * width) * 4;
        row[2] = inpixels + (y2 * width) * 4;
        for (x = 0, x1 = width-1;x < width;x1 = x, x++)
        {
            x2 = x + 1;if (x2 >= width) x2 = 0;
            // left, right
            b = row[1] + x1 * 4;p[0] = (b[0] + b[1] + b[2]);
            b = row[1] + x2 * 4;p[1] = (b[0] + b[1] + b[2]);
            // above, below
            b = row[0] + x  * 4;p[2] = (b[0] + b[1] + b[2]);
            b = row[2] + x  * 4;p[3] = (b[0] + b[1] + b[2]);
            // center
            b = row[1] + x  * 4;p[4] = (b[0] + b[1] + b[2]);
            // calculate a normal from the slopes
            n[0] = p[0] - p[1];
            n[1] = p[3] - p[2];
            n[2] = ibumpscale;
            VectorNormalize(n);
            // turn it into a dot3 rgb vector texture
            out[2] = (int)(128.0f + n[0] * 127.0f);
            out[1] = (int)(128.0f + n[1] * 127.0f);
            out[0] = (int)(128.0f + n[2] * 127.0f);
            out[3] = (p[4]) / 3;
            out += 4;
        }
    }
}

#ifndef DEDICATED_SERVER
void Image_Init() {
    // TODO: check result
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
}

void Image_Shutdown() {
    IMG_Quit();
}

// NULL type means autodeteted type
static unsigned char* Load_SDL_Image_RW(SDL_RWops* rw, const char* type) {
    unsigned char *imagedata = NULL;
    SDL_Surface *surface;

    surface = IMG_LoadTyped_RW(rw, 1, type);

    if (surface == NULL) {
        return NULL;
    } else if (surface->format->format != SDL_PIXELFORMAT_BGRA32) {
        // if surface not in BGRA32 then convert it
        SDL_Surface *temp_surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_BGRA32, 0);
        SDL_FreeSurface(surface);
        surface = temp_surface;
    }

    // TODO: Fix this
    image_height = surface->h;
    image_width = surface->w;

    imagedata = (unsigned char *)Mem_Alloc(tempmempool, surface->w * surface->h * 4);
    memcpy(imagedata, surface->pixels, surface->w * surface->h * 4);
    SDL_FreeSurface(surface);
    return imagedata;
}

unsigned char* Load_SDL_Image_BGRA(const char* filename, const char* type) {

    SDL_RWops *rw = FS_SDL_OpenVirtualFile(filename, true);

    if (rw == NULL) {
        return NULL;
    } else {
        return Load_SDL_Image_RW(rw, type);
    }
}

unsigned char* Load_SDL_Image_MEM_BGRA(const unsigned char *raw, int filesize, const char* type) {
    SDL_RWops *rw = SDL_RWFromConstMem((const void*)raw, filesize);
    if (rw == NULL) {
        return NULL;
    } else {
        return Load_SDL_Image_RW(rw, type);
    }
}

int Image_SaveIMG(
    const char *filename, int width, int height, unsigned char *data,
    saveimg_t format, saveimg_params_t *params) {

    int depth, pitch, result;
    Uint32 rmask, gmask, bmask, amask;

    SDL_RWops *rw = FS_SDL_OpenRealFile(filename, "wb", true);
    if (rw == NULL) {
        return -1;
    }

    if (format == SAVEIMG_PNG && params->png.has_alpha == true) {
        depth = 32;
        pitch = 4 * width;
        amask = 0xff000000;
        rmask = 0x00ff0000;
        gmask = 0x0000ff00;
        bmask = 0x000000ff;
    } else {
        depth = 24;
        pitch = 3 * width;
        rmask = 0xff0000;
        gmask = 0x00ff00;
        bmask = 0x0000ff;
        amask = 0x000000;
    }

    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)data, width, height, depth, pitch,
                                                    rmask, gmask, bmask, amask);

    if (surface == NULL) {
        return -1;
    }


    result = 0;
    switch(format) {
        case SAVEIMG_JPEG:
            result = IMG_SaveJPG_RW(surface, rw, 1, params->jpeg.quality);
            break;
        case SAVEIMG_PNG:
            result = IMG_SavePNG_RW(surface, rw, 1);
            break;
        case SAVEIMG_BMP:
            result = SDL_SaveBMP_RW(surface, rw, 1);
            break;
    }
    SDL_FreeSurface(surface);
    return result;
}

#else

rtexture_t *loadtextureimage (rtexturepool_t *pool, const char *filename, qboolean complain, int flags, qboolean allowFixtrans, qboolean sRGB) {
    return NULL;
}

unsigned char *loadimagepixelsbgra (const char *filename, qboolean complain, qboolean allowFixtrans, qboolean convertsRGB, int *miplevel) {
    return NULL;
}

void Image_Init() {
}

void Image_Shutdown() {
}
#endif // DEDICATED_SERVER
