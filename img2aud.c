#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pnm_header_t pnm_header_t;
typedef struct wav_header_t wav_header_t;

struct pnm_header_t
{
    char pnm_type;
    unsigned int width;
    unsigned int height;
    unsigned short max_val;
    short bits_per_pixel;
    short num_channels;
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
    unsigned short tmp;
    signed short stmp;
    int i, bps;
    FILE *in_fp, *out_fp;
    char *in_filename = NULL, *out_filename = NULL;
    char pnm_type[2];
    pnm_header_t pnm_header;
    wav_header_t wav_header;

    if (argc < 3) {
        fprintf(stderr, "Usage: img2aud [image input] [audio output]\n");
        exit(EXIT_FAILURE);
    }

    in_filename = argv[1];
    out_filename = argv[2];

    /* Load an image */
    if ((in_fp = fopen(in_filename, "r")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for reading\n",
                in_filename);
        exit(EXIT_FAILURE);
    }

    /* Consume the first three characters */
    pnm_type[0] = (char)fgetc(in_fp);
    pnm_type[1] = (char)fgetc(in_fp);
    fgetc(in_fp);

    if (pnm_type[0] != 'P' && !isdigit(pnm_type[1])) {
        fprintf(stderr, "Error: Unrecognized pnm file type: %c%c\n",
                pnm_type[0], pnm_type[1]);
        exit(EXIT_FAILURE);
    }

    pnm_header.pnm_type = pnm_type[1];

    fscanf(in_fp, "%d", &pnm_header.width);
    fscanf(in_fp, "%d", &pnm_header.height);
    pnm_header.max_val = 1;

    switch (pnm_type[1]) {
        /* PBM ASCII */
        case '1':
            pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                     sizeof(pnm_header.data));
            
            if (pnm_header.data == NULL) {
                fprintf(stderr, "Error: Could not allocate enough memory for"
                        " image data at line %d\n", __LINE__);
                exit (EXIT_FAILURE);
            }

            pnm_header.bits_per_pixel = 8;
            pnm_header.num_channels = 1;

            for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                fscanf(in_fp, "%hhu", &pnm_header.data[i]);
            }
            break;

        /* PBM Binary */
        case '4':
            pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                     sizeof(pnm_header.data));
            
            if (pnm_header.data == NULL) {
                fprintf(stderr, "Error: Could not allocate enough memory for"
                        " image data at line %d\n", __LINE__);
                exit (EXIT_FAILURE);
            }

            pnm_header.bits_per_pixel = 8;
            pnm_header.num_channels = 1;

            /* Consume the whitespace before the first pixel */
            fgetc(in_fp);
            for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                pnm_header.data[i] = (unsigned char)fgetc(in_fp);
            }
            break;

        /* PGM ASCII */
        case '2':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                         sizeof(pnm_header.data));
            
                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 8;
                pnm_header.num_channels = 1;

                for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                    fscanf(in_fp, "%hhu", &pnm_header.data[i]);
                }
            }
            else {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 2, sizeof(pnm_header.data));
            
                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 16;
                pnm_header.num_channels = 1;

                for (i = 0; i < pnm_header.width * pnm_header.height; i += 2) {
                    fscanf(in_fp, "%hd", &tmp);
                    pnm_header.data[i]     = tmp & 0xFF00;
                    pnm_header.data[i + 1] = tmp & 0x00FF;
                }
            }
            break;

        /* PGM Binary */
        case '5':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                         sizeof(pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 8;
                pnm_header.num_channels = 1;

                for (i = 0; i < pnm_header.width * pnm_header.height; i++) {
                    pnm_header.data[i] = fgetc(in_fp);
                }
            }
            else {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 2, sizeof(pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 16;
                pnm_header.num_channels = 1;

                for (i = 0; i < pnm_header.width * pnm_header.height * 2; i++) {
                    pnm_header.data[i] = fgetc(in_fp);
                }
            }
            break;

        /* PPM ASCII */
        case '3':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 3, sizeof(pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 8;
                pnm_header.num_channels = 3;

                for (i = 0; i < pnm_header.width * pnm_header.height * 3; i++) {
                    fscanf(in_fp, "%hhu", &pnm_header.data[i]);
                }
            }
            else {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 3 * 2, sizeof(pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }
                
                pnm_header.bits_per_pixel = 16;
                pnm_header.num_channels = 3;

                for (i = 0; i < pnm_header.width * pnm_header.height * 3 * 2;
                     i += 2) {
                    fscanf(in_fp, "%hd", &tmp);
                    pnm_header.data[i]     = tmp & 0xFF00;
                    pnm_header.data[i + 1] = tmp & 0x00FF;
                }
            }
            break;

        /* PPM Binary */
        case '6':
            fscanf(in_fp, "%hd", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 3, sizeof(pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 8;
                pnm_header.num_channels = 3;

                for (i = 0; i < pnm_header.width * pnm_header.height * 3; i++) {
                    pnm_header.data[i] = fgetc(in_fp);
                }
            }
            else {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 3 * 2, sizeof(pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 16;
                pnm_header.num_channels = 3;

                for (i = 0; i < pnm_header.width * pnm_header.height * 3 * 2;
                     i += 2) {
                    pnm_header.data[i]     = fgetc(in_fp);
                    pnm_header.data[i + 1] = fgetc(in_fp);
                }
            }
            break;

        default:
            break;
    }

    fclose(in_fp);
    
    /* Save as audio */
    if ((out_fp = fopen(out_filename, "w")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing\n",
                out_filename);
        exit(EXIT_FAILURE);
    }
    
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
    wav_header.subchunk_2_size = pnm_header.width * pnm_header.height
                                 * wav_header.num_channels
                                 * wav_header.bits_per_sample / 8;
    wav_header.chunk_size = 36 + wav_header.subchunk_2_size;
    wav_header.data = calloc(pnm_header.width * pnm_header.height
                             * pnm_header.num_channels
                             * wav_header.num_channels,
                             wav_header.bits_per_sample / 8);

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

    /* TODO: Add support for more than two audio channels. Will this ever
     * actually come up? Probably not, but it'd be nice to support upto
     * eight-channel audio as well */
    /* That'd be kind of a waste, but what do I care? This project is just to
     * flex my programming muscles, lol */

    if (bps == 8) {
        /* For 8 bits per channel to 8 bits per sample, simply switch the
         * endianness */
        if (pnm_header.bits_per_pixel == 8) {
            for (i = 0; i < pnm_header.width * pnm_header.height
                 * pnm_header.num_channels; i++) {

                wav_header.data[wav_header.num_channels * i] =
                    (pnm_header.data[i] >> 4) | (pnm_header.data[i] << 4);
                
                /* If there are 2 audio channels, set the right channel equal to
                 * the left channel */
                if (wav_header.num_channels == 2) {
                    wav_header.data[2 * i + 1] = wav_header.data[2 * i];
                }
            }
        }
        /* For 16 bits per channel to 8 bits per sample, first constrain the
         * data down to 8 bits by dividing the original data by 256. Next,
         * switch the endianness */ 
        else {
            for (i = 0; i < pnm_header.width * pnm_header.height * 2; i += 2) {
                tmp = (pnm_header.data[i] | pnm_header.data[i + 1]) / 256;
                wav_header.data[i / 2] = (tmp >> 8) | (tmp << 8);

                if (wav_header.num_channels == 2) {
                    wav_header.data[i + 1] = wav_header.data[i / 2];
                }
            }
        }
    }
    else if (bps == 16) {
        /* For 8 bits per channel to 16 bits per sample, first expand the data
         * up to 16 bits by multiplying the original data by 256. Next, subtract
         * by 2^15 to center around 0. Finally, switch the endianness */
        if (pnm_header.bits_per_pixel == 8) {
            for (i = 0; i < pnm_header.width * pnm_header.height
                 * pnm_header.num_channels; i++) {

                stmp = pnm_header.data[i] * 256 - 32768;
                wav_header.data[2 * wav_header.num_channels * i] =
                    stmp & 0x00FF;
                wav_header.data[2 * wav_header.num_channels * i + 1] =
                    stmp & 0xFF00;

                if (wav_header.num_channels == 2) {
                    wav_header.data[4 * i + 2] = wav_header.data[4 * i];
                    wav_header.data[4 * i + 3] = wav_header.data[4 * i + 1];
                }
            }
        }

        /* For 16 bits per channel to 16 bits per sample, simply switch the
         * endianness */
        else {
            for (i = 0; i < pnm_header.width * pnm_header.height
                 * pnm_header.num_channels; i += 2) {

                wav_header.data[wav_header.num_channels * i] =
                    pnm_header.data[i + 1];
                wav_header.data[wav_header.num_channels * i + 1] =
                    pnm_header.data[i];

                if (wav_header.num_channels == 2) {
                    wav_header.data[2 * i + 2] = wav_header.data[2 * i];
                    wav_header.data[2 * i + 3] = wav_header.data[2 * i + 1];
                }
            }
        }
    }

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
        fwrite(wav_header.data, sizeof(unsigned char),
               pnm_header.width * pnm_header.height, out_fp);
    }
    else if (bps == 16) {
        fwrite(wav_header.data, sizeof(short),
               pnm_header.width * pnm_header.height, out_fp);
    }

    free(pnm_header.data);
    free(wav_header.data);
    fclose(out_fp);

    return 0;
}
/* EOF */
