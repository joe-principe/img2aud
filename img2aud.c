#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct pnm_header_t pnm_header_t;
typedef struct wav_header_t wav_header_t;

struct pnm_header_t
{
    char pnm_type;
    unsigned int width;
    unsigned int height;
    short max_val;
    unsigned char *data;
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
    unsigned char *data;
};

int
main(int argc, char **argv)
{
    signed short tmp;
    int i, bps;
    FILE *in_fp, *out_fp;
    char *in_filename = NULL, *out_filename = NULL;
    char pnm_type[2];
    pnm_header_t pnm_header;
    wav_header_t wav_header;

    if (argc < 3) {
        fprintf(stderr, "Usage: img2aud [image input] [audio output]\n");
        exit(EXIT_FAILURE);
    } /* if */

    in_filename = argv[1];
    out_filename = argv[2];

    /* Load an image */
    if ((in_fp = fopen(in_filename, "r")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for reading\n",
                in_filename);
        exit(EXIT_FAILURE);
    } /* if */

    /* Consume the first three characters */
    pnm_type[0] = (char)fgetc(in_fp);
    pnm_type[1] = (char)fgetc(in_fp);
    fgetc(in_fp);

    if (pnm_type[0] != 'P' && !isdigit(pnm_type[1])) {
        fprintf(stderr, "Error: Unrecognized pnm file type: %c%c\n",
                pnm_type[0], pnm_type[1]);
        exit(EXIT_FAILURE);
    } /* if */

    pnm_header.pnm_type = pnm_type[1];

    fscanf(in_fp, "%d", &pnm_header.width);
    fscanf(in_fp, "%d", &pnm_header.height);

    switch (pnm_type[1]) {
        /* PBM ASCII */
        case '1':
            pnm_header.data = malloc(sizeof(pnm_header.data)
                                     * pnm_header.width * pnm_header.height);
            
            if (pnm_header.data == NULL) {
                fprintf(stderr, "Error: Could not allocate enough memory for"
                        " image data at line %d\n", __LINE__);
                exit (EXIT_FAILURE);
            } /* if */

            for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                fscanf(in_fp, "%hhu", &pnm_header.data[i]);
            } /* for */
            break;

        /* PBM Binary */
        case '4':
            pnm_header.data = malloc(sizeof(pnm_header.data)
                                     * pnm_header.width * pnm_header.height);
            
            if (pnm_header.data == NULL) {
                fprintf(stderr, "Error: Could not allocate enough memory for"
                        " image data at line %d\n", __LINE__);
                exit (EXIT_FAILURE);
            } /* if */

            /* Consume the whitespace before the first pixel */
            fgetc(in_fp);
            for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                pnm_header.data[i] = (unsigned char)fgetc(in_fp);
            } /* for */
            break;

        /* PGM ASCII */
        case '2':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = malloc(sizeof(pnm_header.data)
                                         * pnm_header.width
                                         * pnm_header.height);
            
                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                    fscanf(in_fp, "%hhu", &pnm_header.data[i]);
                } /* for */
            } /* if */
            else {
                pnm_header.data = malloc(sizeof(pnm_header.data) * 2
                                         * pnm_header.width
                                         * pnm_header.height);
            
                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height; i += 2) {
                    fscanf(in_fp, "%hd", &tmp);
                    pnm_header.data[i]     = tmp & 0xFF00;
                    pnm_header.data[i + 1] = tmp & 0x00FF;
                } /* for */
            } /* else */
            break;

        /* PGM Binary */
        case '5':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = malloc(sizeof(pnm_header.data)
                                         * pnm_header.width
                                         * pnm_header.height);
                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                    pnm_header.data[i] = fgetc(in_fp);
                } /* for */
            } /* if */
            else {
                pnm_header.data = malloc(sizeof(pnm_header.data) * 2
                                         * pnm_header.width
                                         * pnm_header.height);

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height * 2; i++) {
                    pnm_header.data[i] = fgetc(in_fp);
                } /* for */
            } /* else */
            break;

        /* PPM ASCII */
        case '3':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = malloc(sizeof(pnm_header.data) * 3
                                         * pnm_header.width
                                         * pnm_header.height);

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height * 3; i++) {
                    fscanf(in_fp, "%hhu", &pnm_header.data[i]);
                } /* for */
            } /* if */
            else {
                pnm_header.data = malloc(sizeof(pnm_header.data) * 3 * 2
                                         * pnm_header.width
                                         * pnm_header.height);

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height * 3 * 2;
                     i += 2) {
                    fscanf(in_fp, "%hd", &tmp);
                    pnm_header.data[i]     = tmp & 0xFF00;
                    pnm_header.data[i + 1] = tmp & 0x00FF;
                } /* for */
            } /* else */
            break;

        /* PPM Binary */
        case '6':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = malloc(sizeof(pnm_header.data) * 3
                                         * pnm_header.width
                                         * pnm_header.height);

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height * 3; i++) {
                    pnm_header.data[i] = fgetc(in_fp);
                } /* for */
            } /* if */
            else {
                pnm_header.data = malloc(sizeof(pnm_header.data) * 3 * 2
                                         * pnm_header.width
                                         * pnm_header.height);

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                } /* if */

                for (i = 0; i < pnm_header.width * pnm_header.height * 3 * 2;
                     i += 2) {
                    pnm_header.data[i]     = fgetc(in_fp);
                    pnm_header.data[i + 1] = fgetc(in_fp);
                } /* for */
            } /* else */
            break;

        default:
            break;
    } /* switch */

    fclose(in_fp);
    
    /* Save as audio */
    if ((out_fp = fopen(out_filename, "w")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing\n",
                out_filename);
        exit(EXIT_FAILURE);
    } /* if */
    
    wav_header.chunk_id = "RIFF";
    wav_header.format = "WAVE";
    wav_header.subchunk_1_id = "fmt ";
    wav_header.subchunk_1_size = 16;
    wav_header.audio_format = 1;
    wav_header.subchunk_2_id = "data";

    /* TODO: Allow user to specify the file settings (eg, stereo) */
    wav_header.num_channels = 1;
    wav_header.sample_rate = 44100;
    bps = 8;
    wav_header.bits_per_sample = bps;
    wav_header.byte_rate = wav_header.sample_rate * wav_header.num_channels
                           * wav_header.bits_per_sample / 8;
    wav_header.block_align = wav_header.num_channels
                             * wav_header.bits_per_sample / 8;
    wav_header.subchunk_2_size = width * height * wav_header.num_channels
                                 * wav_header.bits_per_sample / 8;
    wav_header.chunk_size = 36 + wav_header.subchunk_2_size;

    /* TODO: Figure out a good way to decide which data is being used */
    if (bps == 8) {
        wav_header.data.data_8bit = data;
        wav_header.data.data_8bit = pgm_data.data_8bit;
        /* TODO: Multiple audio files for ppm */
    } /* if */
    else if (bps == 16) {
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

    if (bps == 8) {
        fwrite(wav_header.data.data_8bit, sizeof(unsigned char), width * height,
               out_fp);
        /* TODO: Multiple audio files for ppm */
    } /* if */
    else if (bps == 16) {
        fwrite(wav_header.data.data_16bit, sizeof(short), width * height,
               out_fp);
        /* TODO: Multiple audio files for ppm */
    } /* else if */

    /* TODO: Clean up memory allocated for image data */

    fclose(out_fp);

    return 0;
}
/* EOF */
