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
    unsigned short bits_per_pixel;
    unsigned short num_channels;
    unsigned char *data;
};

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

int
main(int argc, char **argv)
{
    unsigned short tmp;
    signed short stmp;
    unsigned int i, bps, nc;
    FILE *in_fp = NULL, *out_fp = NULL;
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

    fscanf(in_fp, "%u", &pnm_header.width);
    fscanf(in_fp, "%u", &pnm_header.height);
    pnm_header.max_val = 1;

    switch (pnm_type[1]) {
        /* PBM ASCII */
        case '1':
            pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                     sizeof(*pnm_header.data));
            
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
                                     sizeof(*pnm_header.data));
            
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
            fscanf(in_fp, "%hu", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                         sizeof(*pnm_header.data));
            
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
                                         * 2, sizeof(*pnm_header.data));
            
                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header.bits_per_pixel = 16;
                pnm_header.num_channels = 1;

                for (i = 0; i < pnm_header.width * pnm_header.height; i += 2) {
                    fscanf(in_fp, "%hu", &tmp);
                    pnm_header.data[i]     =
                        (unsigned char)((tmp & 0xFF00) >> 8);
                    pnm_header.data[i + 1] =
                        (unsigned char)(tmp & 0x00FF);
                }
            }
            break;

        /* PGM Binary */
        case '5':
            fscanf(in_fp, "%hu", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height,
                                         sizeof(*pnm_header.data));

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
                                         * 2, sizeof(*pnm_header.data));

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
            fscanf(in_fp, "%hu", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 3, sizeof(*pnm_header.data));

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
                                         * 3 * 2, sizeof(*pnm_header.data));

                if (pnm_header.data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d\n", __LINE__);
                    exit(EXIT_FAILURE);
                }
                
                pnm_header.bits_per_pixel = 16;
                pnm_header.num_channels = 3;

                for (i = 0; i < pnm_header.width * pnm_header.height * 3 * 2;
                     i += 2) {
                    fscanf(in_fp, "%hu", &tmp);
                    pnm_header.data[i]     =
                        (unsigned char)((tmp & 0xFF00) >> 8);
                    pnm_header.data[i + 1] =
                        (unsigned char)(tmp & 0x00FF);
                }
            }
            break;

        /* PPM Binary */
        case '6':
            fscanf(in_fp, "%hu", &pnm_header.max_val);

            if (pnm_header.max_val <= 255) {
                pnm_header.data = calloc(pnm_header.width * pnm_header.height
                                         * 3, sizeof(*pnm_header.data));

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
                                         * 3 * 2, sizeof(*pnm_header.data));

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
    in_fp = NULL;
    
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
    nc = 1;
    wav_header.num_channels = nc;
    wav_header.sample_rate = 44100;
    bps = 8;
    wav_header.bits_per_sample = bps;
    wav_header.byte_rate = wav_header.sample_rate * wav_header.num_channels
                           * wav_header.bits_per_sample / 8;
    wav_header.block_align = wav_header.num_channels
                             * wav_header.bits_per_sample / 8;
    wav_header.subchunk_2_size = pnm_header.width * pnm_header.height
                                 * pnm_header.num_channels
                                 * wav_header.num_channels
                                 * wav_header.bits_per_sample / 8;
    wav_header.chunk_size = 36 + wav_header.subchunk_2_size;
    wav_header.data = calloc(pnm_header.width * pnm_header.height
                             * pnm_header.num_channels
                             * wav_header.num_channels,
                             wav_header.bits_per_sample / 8);

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

                wav_header.data[nc * i] =
                    (pnm_header.data[i] >> 4) | (pnm_header.data[i] << 4);
                
                /* If there are 2 audio channels, set the right channel equal to
                 * the left channel */
                if (nc == 2) {
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

                if (nc == 2) {
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
                wav_header.data[2 * nc * i] =
                    (unsigned char)(stmp & 0x00FF);
                wav_header.data[2 * nc * i + 1] =
                    (unsigned char)((stmp & 0xFF00) >> 8);

                if (nc == 2) {
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

                wav_header.data[nc * i] = pnm_header.data[i + 1];
                wav_header.data[nc * i + 1] = pnm_header.data[i];

                if (nc == 2) {
                    wav_header.data[2 * i + 2] = wav_header.data[2 * i];
                    wav_header.data[2 * i + 3] = wav_header.data[2 * i + 1];
                }
            }
        }
    }

    /* Write out to wave file */
    fwrite(wav_header.chunk_id, sizeof(char), 4, out_fp);
    fwrite(&wav_header.chunk_size, sizeof(unsigned int), 1, out_fp);
    fwrite(wav_header.format, sizeof(char), 4, out_fp);
    fwrite(wav_header.subchunk_1_id, sizeof(char), 4, out_fp);
    fwrite(&wav_header.subchunk_1_size, sizeof(unsigned int), 1, out_fp);
    fwrite(&wav_header.audio_format, sizeof(unsigned short), 1, out_fp);
    fwrite(&wav_header.num_channels, sizeof(unsigned short), 1, out_fp);
    fwrite(&wav_header.sample_rate, sizeof(unsigned int), 1, out_fp);
    fwrite(&wav_header.byte_rate, sizeof(unsigned int), 1, out_fp);
    fwrite(&wav_header.block_align, sizeof(unsigned short), 1, out_fp);
    fwrite(&wav_header.bits_per_sample, sizeof(unsigned short), 1, out_fp);
    fwrite(wav_header.subchunk_2_id, sizeof(char), 4, out_fp);
    fwrite(&wav_header.subchunk_2_size, sizeof(unsigned int), 1, out_fp);
    fwrite(wav_header.data, sizeof(unsigned char),
           pnm_header.width * pnm_header.height * pnm_header.num_channels * nc,
           out_fp);

    free(pnm_header.data);
    pnm_header.data = NULL;

    free(wav_header.data);
    wav_header.data = NULL;

    fclose(out_fp);
    out_fp = NULL;

    return 0;
}
/* EOF */
