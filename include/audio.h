#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>

#include "image.h"

typedef struct wav_header_t wav_header_t;

struct wav_header_t
{
    char *chunk_id;
    unsigned int chunk_size;
    char *format;
    char *subchunk_1_id;
    unsigned int subchunk_1_size;
    unsigned short audio_format;
    unsigned short num_channels;
    unsigned int sample_rate;
    unsigned int byte_rate;
    unsigned short block_align;
    unsigned short bits_per_sample;
    char *subchunk_2_id;
    unsigned int subchunk_2_size;
    unsigned char *data;
};

/**
 * Initializes the wave header with certain parameters
 * @param wav_header The wave header
 */
void init_wav_header(wav_header_t *wav_header, pnm_header_t *pnm_header);

/**
 * Sets the wave audio data
 * @param wav_header The wave header
 */
void set_wav_data(wav_header_t *wav_header, pnm_header_t *pnm_header);

/**
 * Writes the info in the wave header out to disk
 * @param wav_header The wave header
 * @param pnm_header The pnm header
 * @param fp A pointer to the file to which to write
 */
void write_wav_header(wav_header_t *wav_header, pnm_header_t *pnm_header,
                      FILE *fp);

#endif
/* EOF */
