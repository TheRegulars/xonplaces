/*
    Copyright (C) 2003-2005  Mathieu Olivier

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
#include "snd_main.h"
#include "snd_ogg.h"
#include "snd_wav.h"

#define OV_EXCLUDE_STATIC_CALLBACKS
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>

/*
=================================================================

    Ogg Vorbis decoding

=================================================================
*/

typedef struct
{
    unsigned char *buffer;
    ogg_int64_t ind, buffsize;
} ov_decode_t;

static size_t ovcb_read (void *ptr, size_t size, size_t nb, void *datasource)
{
    ov_decode_t *ov_decode = (ov_decode_t*)datasource;
    size_t remain, len;

    remain = ov_decode->buffsize - ov_decode->ind;
    len = size * nb;
    if (remain < len)
        len = remain - remain % size;

    memcpy (ptr, ov_decode->buffer + ov_decode->ind, len);
    ov_decode->ind += len;

    return len / size;
}

static int ovcb_seek (void *datasource, ogg_int64_t offset, int whence)
{
    ov_decode_t *ov_decode = (ov_decode_t*)datasource;

    switch (whence)
    {
        case SEEK_SET:
            break;
        case SEEK_CUR:
            offset += ov_decode->ind;
            break;
        case SEEK_END:
            offset += ov_decode->buffsize;
            break;
        default:
            return -1;
    }
    if (offset < 0 || offset > ov_decode->buffsize)
        return -1;

    ov_decode->ind = offset;
    return 0;
}

static int ovcb_close (void *ov_decode)
{
    return 0;
}

static long ovcb_tell (void *ov_decode)
{
    return ((ov_decode_t*)ov_decode)->ind;
}

// Per-sfx data structure
typedef struct
{
    unsigned char    *file;
    size_t            filesize;
} ogg_stream_persfx_t;

// Per-channel data structure
typedef struct
{
    OggVorbis_File    vf;
    ov_decode_t        ov_decode;
    int                bs;
    int                buffer_firstframe;
    int                buffer_numframes;
    unsigned char    buffer[STREAM_BUFFERSIZE*4];
} ogg_stream_perchannel_t;


static const ov_callbacks callbacks = {ovcb_read, ovcb_seek, ovcb_close, ovcb_tell};

/*
====================
OGG_GetSamplesFloat
====================
*/
static void OGG_GetSamplesFloat (channel_t *ch, sfx_t *sfx, int firstsampleframe, int numsampleframes, float *outsamplesfloat)
{
    ogg_stream_perchannel_t *per_ch = (ogg_stream_perchannel_t *)ch->fetcher_data;
    ogg_stream_persfx_t *per_sfx = (ogg_stream_persfx_t *)sfx->fetcher_data;
    int f = sfx->format.width * sfx->format.channels; // bytes per frame in the buffer
    short *buf;
    int i, len;
    int newlength, done, ret;

    // if this channel does not yet have a channel fetcher, make one
    if (per_ch == NULL)
    {
        // allocate a struct to keep track of our file position and buffer
        per_ch = (ogg_stream_perchannel_t *)Mem_Alloc(snd_mempool, sizeof(*per_ch));
        // begin decoding the file
        per_ch->ov_decode.buffer = per_sfx->file;
        per_ch->ov_decode.ind = 0;
        per_ch->ov_decode.buffsize = per_sfx->filesize;
        if (ov_open_callbacks(&per_ch->ov_decode, &per_ch->vf, NULL, 0, callbacks) < 0)
        {
            // this never happens - this function succeeded earlier on the same data
            Mem_Free(per_ch);
            return;
        }
        per_ch->bs = 0;
        per_ch->buffer_firstframe = 0;
        per_ch->buffer_numframes = 0;
        // attach the struct to our channel
        ch->fetcher_data = (void *)per_ch;
    }

    // if the request is too large for our buffer, loop...
    while (numsampleframes * f > (int)sizeof(per_ch->buffer))
    {
        done = sizeof(per_ch->buffer) / f;
        OGG_GetSamplesFloat(ch, sfx, firstsampleframe, done, outsamplesfloat);
        firstsampleframe += done;
        numsampleframes -= done;
        outsamplesfloat += done * sfx->format.channels;
    }

    // seek if the request is before the current buffer (loop back)
    // seek if the request starts beyond the current buffer by at least one frame (channel was zero volume for a while)
    // do not seek if the request overlaps the buffer end at all (expected behavior)
    if (per_ch->buffer_firstframe > firstsampleframe || per_ch->buffer_firstframe + per_ch->buffer_numframes < firstsampleframe)
    {
        // we expect to decode forward from here so this will be our new buffer start
        per_ch->buffer_firstframe = firstsampleframe;
        per_ch->buffer_numframes = 0;
        ret = ov_pcm_seek(&per_ch->vf, (ogg_int64_t)firstsampleframe);
        if (ret != 0)
        {
            // LordHavoc: we can't Con_Printf here, not thread safe...
            //Con_Printf("OGG_FetchSound: ov_pcm_seek(..., %d) returned %d\n", firstsampleframe, ret);
            return;
        }
    }

    // decompress the file as needed
    if (firstsampleframe + numsampleframes > per_ch->buffer_firstframe + per_ch->buffer_numframes)
    {
        // first slide the buffer back, discarding any data preceding the range we care about
        int offset = firstsampleframe - per_ch->buffer_firstframe;
        int keeplength = per_ch->buffer_numframes - offset;
        if (keeplength > 0)
            memmove(per_ch->buffer, per_ch->buffer + offset * sfx->format.width * sfx->format.channels, keeplength * sfx->format.width * sfx->format.channels);
        per_ch->buffer_firstframe = firstsampleframe;
        per_ch->buffer_numframes -= offset;
        // decompress as much as we can fit in the buffer
        newlength = sizeof(per_ch->buffer) - per_ch->buffer_numframes * f;
        done = 0;
        while (newlength > done && (ret = ov_read(&per_ch->vf, (char *)per_ch->buffer + per_ch->buffer_numframes * f + done, (int)(newlength - done), mem_bigendian, 2, 1, &per_ch->bs)) > 0)
            done += ret;
        // clear the missing space if any
        if (done < newlength)
            memset(per_ch->buffer + done, 0, newlength - done);
        // we now have more data in the buffer
        per_ch->buffer_numframes += done / f;
    }

    // convert the sample format for the caller
    buf = (short *)((char *)per_ch->buffer + (firstsampleframe - per_ch->buffer_firstframe) * f);
    len = numsampleframes * sfx->format.channels;
    for (i = 0;i < len;i++)
        outsamplesfloat[i] = buf[i] * (1.0f / 32768.0f);
}


/*
====================
OGG_StopChannel
====================
*/
static void OGG_StopChannel(channel_t *ch)
{
    ogg_stream_perchannel_t *per_ch = (ogg_stream_perchannel_t *)ch->fetcher_data;
    if (per_ch != NULL)
    {
        // release the vorbis decompressor
        ov_clear(&per_ch->vf);
        Mem_Free(per_ch);
    }
}


/*
====================
OGG_FreeSfx
====================
*/
static void OGG_FreeSfx(sfx_t *sfx)
{
    ogg_stream_persfx_t *per_sfx = (ogg_stream_persfx_t *)sfx->fetcher_data;
    // free the complete file we were keeping around
    Mem_Free(per_sfx->file);
    // free the file information structure
    Mem_Free(per_sfx);
}


static const snd_fetcher_t ogg_fetcher = {OGG_GetSamplesFloat, OGG_StopChannel, OGG_FreeSfx};

static void OGG_DecodeTags(vorbis_comment *vc, unsigned int *start, unsigned int *length, unsigned int numsamples, double *peak, double *gaindb)
{
    const char *startcomment = NULL, *lengthcomment = NULL, *endcomment = NULL, *thiscomment = NULL;

    *start = numsamples;
    *length = numsamples;
    *peak = 0.0;
    *gaindb = 0.0;

    if(!vc)
        return;

    thiscomment = vorbis_comment_query(vc, "REPLAYGAIN_TRACK_PEAK", 0);
    if(thiscomment)
        *peak = atof(thiscomment);
    thiscomment = vorbis_comment_query(vc, "REPLAYGAIN_TRACK_GAIN", 0);
    if(thiscomment)
        *gaindb = atof(thiscomment);
    
    startcomment = vorbis_comment_query(vc, "LOOP_START", 0); // DarkPlaces, and some Japanese app
    if(startcomment)
    {
        endcomment = vorbis_comment_query(vc, "LOOP_END", 0);
        if(!endcomment)
            lengthcomment = vorbis_comment_query(vc, "LOOP_LENGTH", 0);
    }
    else
    {
        startcomment = vorbis_comment_query(vc, "LOOPSTART", 0); // RPG Maker VX
        if(startcomment)
        {
            lengthcomment = vorbis_comment_query(vc, "LOOPLENGTH", 0);
            if(!lengthcomment)
                endcomment = vorbis_comment_query(vc, "LOOPEND", 0);
        }
        else
        {
            startcomment = vorbis_comment_query(vc, "LOOPPOINT", 0); // Sonic Robo Blast 2
        }
    }

    if(startcomment)
    {
        *start = (unsigned int) bound(0, atof(startcomment), numsamples);
        if(endcomment)
            *length = (unsigned int) bound(0, atof(endcomment), numsamples);
        else if(lengthcomment)
            *length = (unsigned int) bound(0, *start + atof(lengthcomment), numsamples);
    }
}

/*
====================
OGG_LoadVorbisFile

Load an Ogg Vorbis file into memory
====================
*/
qboolean OGG_LoadVorbisFile(const char *filename, sfx_t *sfx)
{
    unsigned char *data;
    fs_offset_t filesize;
    ov_decode_t ov_decode;
    OggVorbis_File vf;
    vorbis_info *vi;
    vorbis_comment *vc;
    double peak, gaindb;

    // Return if already loaded
    if (sfx->fetcher != NULL)
        return true;

    // Load the file completely
    data = FS_LoadFile(filename, snd_mempool, false, &filesize);
    if (data == NULL)
        return false;

    if (developer_loading.integer >= 2)
        Con_Printf("Loading Ogg Vorbis file \"%s\"\n", filename);

    // Open it with the VorbisFile API
    ov_decode.buffer = data;
    ov_decode.ind = 0;
    ov_decode.buffsize = filesize;
    if (ov_open_callbacks(&ov_decode, &vf, NULL, 0, callbacks) < 0)
    {
        Con_Printf("error while opening Ogg Vorbis file \"%s\"\n", filename);
        Mem_Free(data);
        return false;
    }

    // Get the stream information
    vi = ov_info(&vf, -1);
    if (vi->channels < 1 || vi->channels > 2)
    {
        Con_Printf("%s has an unsupported number of channels (%i)\n",
                    sfx->name, vi->channels);
        ov_clear (&vf);
        Mem_Free(data);
        return false;
    }

    sfx->format.speed = vi->rate;
    sfx->format.channels = vi->channels;
    sfx->format.width = 2;  // We always work with 16 bits samples

    sfx->total_length = ov_pcm_total(&vf, -1);

    if (snd_streaming.integer && (snd_streaming.integer >= 2 || sfx->total_length > max(sizeof(ogg_stream_perchannel_t), snd_streaming_length.value * sfx->format.speed)))
    {
        // large sounds use the OGG fetcher to decode the file on demand (but the entire file is held in memory)
        ogg_stream_persfx_t* per_sfx;
        if (developer_loading.integer >= 2)
            Con_Printf("Ogg sound file \"%s\" will be streamed\n", filename);
        per_sfx = (ogg_stream_persfx_t *)Mem_Alloc(snd_mempool, sizeof(*per_sfx));
        sfx->memsize += sizeof (*per_sfx);
        per_sfx->file = data;
        per_sfx->filesize = filesize;
        sfx->memsize += filesize;
        sfx->fetcher_data = per_sfx;
        sfx->fetcher = &ogg_fetcher;
        sfx->flags |= SFXFLAG_STREAMED;
        vc = ov_comment(&vf, -1);
        OGG_DecodeTags(vc, &sfx->loopstart, &sfx->total_length, sfx->total_length, &peak, &gaindb);
        ov_clear(&vf);
    }
    else
    {
        // small sounds are entirely loaded and use the PCM fetcher
        char *buff;
        ogg_int64_t len;
        ogg_int64_t done;
        int bs;
        long ret;
        if (developer_loading.integer >= 2)
            Con_Printf ("Ogg sound file \"%s\" will be cached\n", filename);
        len = sfx->total_length * sfx->format.channels * sfx->format.width;
        sfx->flags &= ~SFXFLAG_STREAMED;
        sfx->memsize += len;
        sfx->fetcher = &wav_fetcher;
        sfx->fetcher_data = Mem_Alloc(snd_mempool, (size_t)len);
        buff = (char *)sfx->fetcher_data;
        done = 0;
        bs = 0;
        while ((ret = ov_read(&vf, &buff[done], (int)(len - done), mem_bigendian, 2, 1, &bs)) > 0)
            done += ret;
        vc = ov_comment(&vf, -1);
        OGG_DecodeTags(vc, &sfx->loopstart, &sfx->total_length, sfx->total_length, &peak, &gaindb);
        ov_clear(&vf);
        Mem_Free(data);
    }

    if(peak)
    {
        sfx->volume_mult = min(1.0f / peak, exp(gaindb * 0.05f * log(10.0f)));
        sfx->volume_peak = peak;
        if (developer_loading.integer >= 2)
            Con_Printf ("Ogg sound file \"%s\" uses ReplayGain (gain %f, peak %f)\n", filename, sfx->volume_mult, sfx->volume_peak);
    }
    else if(gaindb != 0)
    {
        sfx->volume_mult = min(1.0f / peak, exp(gaindb * 0.05f * log(10.0f)));
        sfx->volume_peak = 1.0; // if peak is not defined, we won't trust it
        if (developer_loading.integer >= 2)
            Con_Printf ("Ogg sound file \"%s\" uses ReplayGain (gain %f, peak not defined and assumed to be %f)\n", filename, sfx->volume_mult, sfx->volume_peak);
    }

    return true;
}
