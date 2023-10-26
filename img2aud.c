#include <stdio.h>
#include <stdlib.h>

typedef union pgm_data_t pgm_data_t;
typedef union ppm_data_t ppm_data_t;
typedef union wav_data_t wav_data_t;
typedef struct wav_header_t wav_header_t;

union pgm_data_t
{
    unsigned char *data_8bit;
    short *data_16bit;
};

struct ppm_8bit
{
    unsigned char *red_data;
    unsigned char *blue_data;
    unsigned char *green_data;
};

struct ppm_16bit
{
    short *red_data;
    short *blue_data;
    short *green_data;
};

union ppm_data_t
{
    struct ppm_8bit data_8bit;
    struct ppm_16bit data_16bit;
};

union wav_data_t
{
    unsigned char *data_8bit;
    signed short *data_16bit;
};

struct wav_header_t
{
    char *chunk_id;
    int chunk_size;
    char *format;
    char *subchunk_1_id;
    int subchunk_1_size;
    short audio_format;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    char *subchunk_2_id;
    int subchunk_2_size;
    wav_data_t data;
};

int
main(int argc, char **argv)
{
    int i, width, height, max_val, bps;
    unsigned char *data = NULL;
    pgm_data_t pgm_data;
    ppm_data_t ppm_data;
    wav_header_t wav_header;
    FILE *in_fp, *out_fp;
    const char *in_filename = "input.pgm";
    const char *out_filename = "output.wav";

    /* TODO: Allow the user to enter the name of the file to open */
    /* TODO: Allow loading multiple types of files */

    /* Load an image */
    if ((in_fp = fopen(in_filename, "r")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for reading",
                in_filename);
        exit(EXIT_FAILURE);
    } /* if */

    /* Consume the first three characters */
    /* TODO: Check the pnm file type */
    fgetc(in_fp);
    fgetc(in_fp);
    fgetc(in_fp);
    
    /* PBM */
    fscanf(in_fp, "%d", &width);
    fscanf(in_fp, "%d", &height);

    for (i = 0; i < width * height; i++) {
        fscanf(in_fp, "%c", &data[i]);
    } /* for */

    /* PGM */
    fscanf(in_fp, "%d", &width);
    fscanf(in_fp, "%d", &height);
    fscanf(in_fp, "%d", &max_val);

    if (max_val <= 255) {
        for (i = 0; i < width * height; i++) {
            fscanf(in_fp, "%c", &pgm_data.data_8bit[i]);
        } /* for */
    } /* if */
    else {
        for (i = 0; i < width * height; i++) {
            fscanf(in_fp, "%hd", &pgm_data.data_16bit[i]);
        } /* for */
    } /* else */

    /* PPM */
    fscanf(in_fp, "%d", &width);
    fscanf(in_fp, "%d", &height);
    fscanf(in_fp, "%d", &max_val);

    if (max_val <= 255) {
        for (i = 0; i < width * height; i++) {
            fscanf(in_fp, "%c", &ppm_data.data_8bit.red_data[i]);
            fscanf(in_fp, "%c", &ppm_data.data_8bit.blue_data[i]);
            fscanf(in_fp, "%c", &ppm_data.data_8bit.green_data[i]);
        } /* for */
    } /* if */
    else {
        for (i = 0; i < width * height; i++) {
            fscanf(in_fp, "%hd", &ppm_data.data_16bit.red_data[i]);
            fscanf(in_fp, "%hd", &ppm_data.data_16bit.blue_data[i]);
            fscanf(in_fp, "%hd", &ppm_data.data_16bit.green_data[i]);
        } /* for */
    } /* else */

    /* Save as audio */
    if ((out_fp = fopen(out_filename, "w")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing",
                out_filename);
        exit(EXIT_FAILURE);
    } /* if */
    
    /* TODO: Allow user to specify the file settings (eg, stereo) */
    wav_header.chunk_id = "RIFF";
    wav_header.format = "WAVE";
    wav_header.subchunk_1_id = "fmt ";
    wav_header.subchunk_1_size = 16;
    wav_header.subchunk_2_id = "data";

    wav_header.audio_format = 1;
    wav_header.num_channels = 1;
    wav_header.sample_rate = 44100;
    wav_header.bits_per_sample = 8;
    bps = wav_header.bits_per_sample;
    wav_header.byte_rate = wav_header.sample_rate * wav_header.num_channels
                           * wav_header.bits_per_sample / 8;
    wav_header.block_align = wav_header.num_channels
                             * wav_header.bits_per_sample / 8;
    wav_header.subchunk_2_size = width * height * wav_header.num_channels
                                 * wav_header.bits_per_sample / 8;
    wav_header.chunk_size = 36 + wav_header.subchunk_2_size;

    if (wav_header.bits_per_sample == 8) {
        wav_header.data.data_8bit = data;
        wav_header.data.data_8bit = pgm_data.data_8bit;
        /* TODO: Multiple audio files for ppm */
    } /* if */
    else if (wav_header.bits_per_sample == 16) {
        wav_header.data.data_16bit = (short *)data;
        wav_header.data.data_16bit = pgm_data.data_16bit;
        /* TODO: Multiple audio files for ppm */
    } /* else if */

    /* Convert appropriate fields to little endian */
    wav_header.chunk_size =
          ((wav_header.chunk_size << 24) & 0xFF000000)
        | ((wav_header.chunk_size <<  8) & 0x00FF0000)
        | ((wav_header.chunk_size >>  8) & 0x0000FF00)
        | ((wav_header.chunk_size >> 24) & 0x000000FF);
    wav_header.subchunk_1_size =
          ((wav_header.subchunk_1_size << 24) & 0xFF000000)
        | ((wav_header.subchunk_1_size <<  8) & 0x00FF0000)
        | ((wav_header.subchunk_1_size >>  8) & 0x0000FF00)
        | ((wav_header.subchunk_1_size >> 24) & 0x000000FF);
    wav_header.audio_format =
          (wav_header.audio_format >> 8)
        | (wav_header.audio_format << 8);
    wav_header.num_channels =
          (wav_header.num_channels >> 8)
        | (wav_header.num_channels << 8);
    wav_header.sample_rate =
          ((wav_header.sample_rate << 24) & 0xFF000000)
        | ((wav_header.sample_rate <<  8) & 0x00FF0000)
        | ((wav_header.sample_rate >>  8) & 0x0000FF00)
        | ((wav_header.sample_rate >> 24) & 0x000000FF);
    wav_header.byte_rate =
          ((wav_header.byte_rate << 24) & 0xFF000000)
        | ((wav_header.byte_rate <<  8) & 0x00FF0000)
        | ((wav_header.byte_rate >>  8) & 0x0000FF00)
        | ((wav_header.byte_rate >> 24) & 0x000000FF);
    wav_header.block_align =
          (wav_header.block_align >> 8)
        | (wav_header.block_align << 8);
    wav_header.bits_per_sample =
          (wav_header.bits_per_sample >> 8)
        | (wav_header.bits_per_sample << 8);
    wav_header.subchunk_2_size =
          ((wav_header.subchunk_2_size << 24) & 0xFF000000)
        | ((wav_header.subchunk_2_size <<  8) & 0x00FF0000)
        | ((wav_header.subchunk_2_size >>  8) & 0x0000FF00)
        | ((wav_header.subchunk_2_size >> 24) & 0x000000FF);

    if (bps == 8) {
        for (i = 0; i < width * height; i++) {
            wav_header.data.data_8bit[i] =
                  (wav_header.data.data_8bit[i] >> 8)
                | (wav_header.data.data_8bit[i] << 8);
        } /* for */
        /* TODO: Multiple audio files for ppm */
    } /* if */
    else if (bps == 16) {
        for (i = 0; i < width * height; i++) {
            wav_header.data.data_16bit[i] =
                  ((wav_header.data.data_16bit[i] << 24) & 0xFF000000)
                | ((wav_header.data.data_16bit[i] <<  8) & 0x00FF0000)
                | ((wav_header.data.data_16bit[i] >>  8) & 0x0000FF00)
                | ((wav_header.data.data_16bit[i] >> 24) & 0x000000FF);
        } /* for */
        /* TODO: Multiple audio files for ppm */
    } /* if */

    /* Write out to wave file */
    fwrite(wav_header.chunk_id, sizeof(char), 4, out_fp);
    fwrite(&wav_header.chunk_size, sizeof(int), 1, out_fp);
    fwrite(wav_header.format, sizeof(char), 4, out_fp);
    fwrite(wav_header.subchunk_1_id, sizeof(char), 4, out_fp);
    fwrite(&wav_header.subchunk_1_size, sizeof(int), 1, out_fp);
    fwrite(&wav_header.audio_format, sizeof(short), 1, out_fp);
    fwrite(&wav_header.num_channels, sizeof(short), 1, out_fp);
    fwrite(&wav_header.sample_rate, sizeof(int), 1, out_fp);
    fwrite(&wav_header.byte_rate, sizeof(int), 1, out_fp);
    fwrite(&wav_header.block_align, sizeof(short), 1, out_fp);
    fwrite(&wav_header.bits_per_sample, sizeof(short), 1, out_fp);
    fwrite(wav_header.subchunk_2_id, sizeof(char), 4, out_fp);
    fwrite(&wav_header.subchunk_2_size, sizeof(int), 1, out_fp);

    if (wav_header.bits_per_sample == 8) {
        fwrite(wav_header.data.data_8bit, sizeof(unsigned char), width * height,
               out_fp);
        /* TODO: Multiple audio files for ppm */
    } /* if */
    else if (wav_header.bits_per_sample == 16) {
        fwrite(wav_header.data.data_16bit, sizeof(short), width * height,
               out_fp);
        /* TODO: Multiple audio files for ppm */
    } /* else if */

    return 0;
}
/* EOF */
