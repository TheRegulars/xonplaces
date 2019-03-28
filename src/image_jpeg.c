/*
    Copyright (C) 2002  Mathieu Olivier

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to:

        Free Software Foundation, Inc.
        59 Temple Place - Suite 330
        Boston, MA  02111-1307, USA

*/


#include "quakedef.h"
#include "image.h"
#include "image_jpeg.h"

cvar_t sv_writepicture_quality = {CVAR_SAVE, "sv_writepicture_quality", "10", "WritePicture quality offset (higher means better quality, but slower)"};
cvar_t r_texture_jpeg_fastpicmip = {CVAR_SAVE, "r_texture_jpeg_fastpicmip", "1", "perform gl_picmip during decompression for JPEG files (faster)"};

// jboolean is unsigned char instead of int on Win32
#ifdef WIN32
typedef unsigned char jboolean;
#else
typedef int jboolean;
#endif

#include <jpeglib.h>

static jmp_buf error_in_jpeg;
static qboolean jpeg_toolarge;

// Our own output manager for JPEG compression
typedef struct
{
    struct jpeg_destination_mgr pub;

    qfile_t* outfile;
    unsigned char* buffer;
    size_t bufsize; // used if outfile is NULL
} my_destination_mgr;
typedef my_destination_mgr* my_dest_ptr;

/*
=================================================================

    JPEG decompression

=================================================================
*/

static void JPEG_ErrorExit (j_common_ptr cinfo)
{
    ((struct jpeg_decompress_struct*)cinfo)->err->output_message (cinfo);
    longjmp(error_in_jpeg, 1);
}



/*
=================================================================

  JPEG compression

=================================================================
*/

#define JPEG_OUTPUT_BUF_SIZE 4096

static void JPEG_Mem_InitDestination (j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr)cinfo->dest;
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
}

static jboolean JPEG_Mem_EmptyOutputBuffer (j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr)cinfo->dest;
    jpeg_toolarge = true;
    dest->pub.next_output_byte = dest->buffer;
    dest->pub.free_in_buffer = dest->bufsize;
    return true;
}

static void JPEG_Mem_TermDestination (j_compress_ptr cinfo)
{
    my_dest_ptr dest = (my_dest_ptr)cinfo->dest;
    dest->bufsize = dest->pub.next_output_byte - dest->buffer;
}
static void JPEG_MemDest (j_compress_ptr cinfo, void* buf, size_t bufsize)
{
    my_dest_ptr dest;

    // First time for this JPEG object?
    if (cinfo->dest == NULL)
        cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT, sizeof(my_destination_mgr));

    dest = (my_dest_ptr)cinfo->dest;
    dest->pub.init_destination = JPEG_Mem_InitDestination;
    dest->pub.empty_output_buffer = JPEG_Mem_EmptyOutputBuffer;
    dest->pub.term_destination = JPEG_Mem_TermDestination;
    dest->outfile = NULL;

    dest->buffer = (unsigned char *) buf;
    dest->bufsize = bufsize;
}


static size_t JPEG_try_SaveImage_to_Buffer (struct jpeg_compress_struct *cinfo, char *jpegbuf, size_t jpegsize, int quality, int width, int height, unsigned char *data)
{
    unsigned char *scanline;
    unsigned int linesize;

    jpeg_toolarge = false;
    JPEG_MemDest (cinfo, jpegbuf, jpegsize);

    // Set the parameters for compression
    cinfo->image_width = width;
    cinfo->image_height = height;
    cinfo->in_color_space = JCS_RGB;
    cinfo->input_components = 3;
    jpeg_set_defaults (cinfo);
    jpeg_set_quality (cinfo, quality, FALSE);

    cinfo->comp_info[0].h_samp_factor = 2;
    cinfo->comp_info[0].v_samp_factor = 2;
    cinfo->comp_info[1].h_samp_factor = 1;
    cinfo->comp_info[1].v_samp_factor = 1;
    cinfo->comp_info[2].h_samp_factor = 1;
    cinfo->comp_info[2].v_samp_factor = 1;
    cinfo->optimize_coding = 1;

    jpeg_start_compress (cinfo, true);

    // Compress each scanline
    linesize = width * 3;
    while (cinfo->next_scanline < cinfo->image_height)
    {
        scanline = &data[cinfo->next_scanline * linesize];

        jpeg_write_scanlines (cinfo, &scanline, 1);
    }

    jpeg_finish_compress (cinfo);

    if(jpeg_toolarge)
        return 0;

    return ((my_dest_ptr) cinfo->dest)->bufsize;
}

size_t JPEG_SaveImage_to_Buffer (char *jpegbuf, size_t jpegsize, int width, int height, unsigned char *data)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    int quality;
    int quality_guess;
    size_t result;

    if(setjmp(error_in_jpeg))
        goto error_caught;
    cinfo.err = jpeg_std_error (&jerr);
    cinfo.err->error_exit = JPEG_ErrorExit;

    jpeg_create_compress (&cinfo);

#if 0
    // used to get the formula below
    {
        char buf[1048576];
        unsigned char *img;
        int i;

        img = Mem_Alloc(tempmempool, width * height * 3);
        for(i = 0; i < width * height * 3; ++i)
            img[i] = rand() & 0xFF;

        for(i = 0; i <= 100; ++i)
        {
            Con_Printf("! %d %d %d %d\n", width, height, i, (int) JPEG_try_SaveImage_to_Buffer(&cinfo, buf, sizeof(buf), i, width, height, img));
        }

        Mem_Free(img);
    }
#endif

    //quality_guess = (int)((100 * jpegsize - 41000) / (width*height) + 2); // fits random data
    quality_guess   = (int)((256 * jpegsize - 81920) / (width*height) - 8); // fits Nexuiz's/Xonotic's map pictures

    quality_guess = bound(0, quality_guess, 100);
    quality = bound(0, quality_guess + sv_writepicture_quality.integer, 100); // assume it can do 10 failed attempts

    while(!(result = JPEG_try_SaveImage_to_Buffer(&cinfo, jpegbuf, jpegsize, quality, width, height, data)))
    {
        --quality;
        if(quality < 0)
        {
            Con_Printf("couldn't write image at all, probably too big\n");
            return 0;
        }
    }
    jpeg_destroy_compress (&cinfo);
    Con_DPrintf("JPEG_SaveImage_to_Buffer: guessed quality/size %d/%d, actually got %d/%d\n", quality_guess, (int)jpegsize, quality, (int)result);

    return result;

error_caught:
    jpeg_destroy_compress (&cinfo);
    return 0;
}

typedef struct CompressedImageCacheItem
{
    char imagename[MAX_QPATH];
    size_t maxsize;
    void *compressed;
    size_t compressed_size;
    struct CompressedImageCacheItem *next;
}
CompressedImageCacheItem;
#define COMPRESSEDIMAGECACHE_SIZE 4096
static CompressedImageCacheItem *CompressedImageCache[COMPRESSEDIMAGECACHE_SIZE];

static void CompressedImageCache_Add(const char *imagename, size_t maxsize, void *compressed, size_t compressed_size)
{
    char vabuf[1024];
    const char *hashkey = va(vabuf, sizeof(vabuf), "%s:%d", imagename, (int) maxsize);
    int hashindex = CRC_Block((unsigned char *) hashkey, strlen(hashkey)) % COMPRESSEDIMAGECACHE_SIZE;
    CompressedImageCacheItem *i;

    if(strlen(imagename) >= MAX_QPATH)
        return; // can't add this
    
    i = (CompressedImageCacheItem*) Z_Malloc(sizeof(CompressedImageCacheItem));
    strlcpy(i->imagename, imagename, sizeof(i->imagename));
    i->maxsize = maxsize;
    i->compressed = compressed;
    i->compressed_size = compressed_size;
    i->next = CompressedImageCache[hashindex];
    CompressedImageCache[hashindex] = i;
}

static CompressedImageCacheItem *CompressedImageCache_Find(const char *imagename, size_t maxsize)
{
    char vabuf[1024];
    const char *hashkey = va(vabuf, sizeof(vabuf), "%s:%d", imagename, (int) maxsize);
    int hashindex = CRC_Block((unsigned char *) hashkey, strlen(hashkey)) % COMPRESSEDIMAGECACHE_SIZE;
    CompressedImageCacheItem *i = CompressedImageCache[hashindex];

    while(i)
    {
        if(i->maxsize == maxsize)
            if(!strcmp(i->imagename, imagename))
                return i;
        i = i->next;
    }
    return NULL;
}

qboolean Image_Compress(const char *imagename, size_t maxsize, void **buf, size_t *size)
{
    unsigned char *imagedata, *newimagedata;
    int maxPixelCount;
    int components[3] = {2, 1, 0};
    CompressedImageCacheItem *i;

    i = CompressedImageCache_Find(imagename, maxsize);
    if(i)
    {
        *size = i->compressed_size;
        *buf = i->compressed;
    }

    // load the image
    imagedata = loadimagepixelsbgra(imagename, true, false, false, NULL);
    if(!imagedata)
        return false;

    // find an appropriate size for somewhat okay compression
    if(maxsize <= 768)
        maxPixelCount = 32 * 32;
    else if(maxsize <= 1024)
        maxPixelCount = 64 * 64;
    else if(maxsize <= 4096)
        maxPixelCount = 128 * 128;
    else
        maxPixelCount = 256 * 256;

    while(image_width * image_height > maxPixelCount)
    {
        int one = 1;
        Image_MipReduce32(imagedata, imagedata, &image_width, &image_height, &one, image_width/2, image_height/2, 1);
    }

    newimagedata = (unsigned char *) Mem_Alloc(tempmempool, image_width * image_height * 3);

    // convert the image from BGRA to RGB
    Image_CopyMux(newimagedata, imagedata, image_width, image_height, false, false, false, 3, 4, components);
    Mem_Free(imagedata);

    // try to compress it to JPEG
    *buf = Z_Malloc(maxsize);
    *size = JPEG_SaveImage_to_Buffer((char *) *buf, maxsize, image_width, image_height, newimagedata);
    Mem_Free(newimagedata);

    if(!*size)
    {
        Z_Free(*buf);
        *buf = NULL;
        Con_Printf("could not compress image %s to %d bytes\n", imagename, (int)maxsize);
        // return false;
        // also cache failures!
    }

    // store it in the cache
    CompressedImageCache_Add(imagename, maxsize, *buf, *size);
    return (*buf != NULL);
}
