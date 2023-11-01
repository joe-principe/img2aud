#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/image.h"

/* Sets the pnm type */
void 
set_pnm_type(pnm_header_t *pnm_header, char *pnm_type, FILE *fp)
{
    pnm_type[0] = (char)fgetc(fp);
    pnm_type[1] = (char)fgetc(fp);
    fgetc(fp);

    if (pnm_type[0] != 'P' && !isdigit(pnm_type[1])) {
        fprintf(stderr, "Error: Unrecognized pnm file type: %c%c\n",
                pnm_type[0], pnm_type[1]);
        exit(EXIT_FAILURE);
    }

    pnm_header->pnm_type = pnm_type[1];
}

/* Initializes the pnm header */
void
init_pnm_header(pnm_header_t *pnm_header, FILE *fp)
{
    unsigned int i;
    unsigned short tmp;

    switch (pnm_header->pnm_type) {
        /* PBM ASCII */
        case '1':
            pnm_header->data = calloc(pnm_header->width * pnm_header->height,
                                      sizeof(*pnm_header->data));
            
            if (pnm_header->data == NULL) {
                fprintf(stderr, "Error: Could not allocate enough memory for"
                        " image data at line %d in file %s\n", __LINE__,
                        __FILE__);
                exit (EXIT_FAILURE);
            }

            pnm_header->bits_per_pixel = 8;
            pnm_header->num_channels = 1;

            for (i = 0; i < pnm_header->width * pnm_header->height; i++) {
                fscanf(fp, "%hhu", &pnm_header->data[i]);
            }
            break;

        /* PBM Binary */
        case '4':
            pnm_header->data = calloc(pnm_header->width * pnm_header->height,
                                      sizeof(*pnm_header->data));
            
            if (pnm_header->data == NULL) {
                fprintf(stderr, "Error: Could not allocate enough memory for"
                        " image data at line %d in file %s\n", __LINE__,
                        __FILE__);
                exit (EXIT_FAILURE);
            }

            pnm_header->bits_per_pixel = 8;
            pnm_header->num_channels = 1;

            /* Consume the whitespace before the first pixel */
            fgetc(fp);
            for (i = 0; i < pnm_header->width * pnm_header->height; i++) {
                pnm_header->data[i] = (unsigned char)fgetc(fp);
            }
            break;

        /* PGM ASCII */
        case '2':
            fscanf(fp, "%hu", &pnm_header->max_val);

            if (pnm_header->max_val <= 255) {
                pnm_header->data =
                    calloc(pnm_header->width * pnm_header->height,
                           sizeof(*pnm_header->data));
            
                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 8;
                pnm_header->num_channels = 1;

                for (i = 0; i < pnm_header->width * pnm_header->height; i++) {
                    fscanf(fp, "%hhu", &pnm_header->data[i]);
                }
            }
            else {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height
                                          * 2, sizeof(*pnm_header->data));
            
                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 16;
                pnm_header->num_channels = 1;

                for (i = 0; i < pnm_header->width * pnm_header->height; i += 2) {
                    fscanf(fp, "%hu", &tmp);
                    pnm_header->data[i]     =
                        (unsigned char)((tmp & 0xFF00) >> 8);
                    pnm_header->data[i + 1] =
                        (unsigned char)(tmp & 0x00FF);
                }
            }
            break;

        /* PGM Binary */
        case '5':
            fscanf(fp, "%hu", &pnm_header->max_val);

            if (pnm_header->max_val <= 255) {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height,
                                          sizeof(*pnm_header->data));

                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 8;
                pnm_header->num_channels = 1;

                for (i = 0; i < pnm_header->width * pnm_header->height; i++) {
                    pnm_header->data[i] = fgetc(fp);
                }
            }
            else {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height
                                          * 2, sizeof(*pnm_header->data));

                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 16;
                pnm_header->num_channels = 1;

                for (i = 0; i < pnm_header->width * pnm_header->height * 2;
                     i++) {
                    pnm_header->data[i] = fgetc(fp);
                }
            }
            break;

        /* PPM ASCII */
        case '3':
            fscanf(fp, "%hu", &pnm_header->max_val);

            if (pnm_header->max_val <= 255) {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height
                                          * 3, sizeof(*pnm_header->data));

                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 8;
                pnm_header->num_channels = 3;

                for (i = 0; i < pnm_header->width * pnm_header->height * 3;
                     i++) {
                    fscanf(fp, "%hhu", &pnm_header->data[i]);
                }
            }
            else {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height
                                          * 3 * 2, sizeof(*pnm_header->data));

                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }
                
                pnm_header->bits_per_pixel = 16;
                pnm_header->num_channels = 3;

                for (i = 0; i < pnm_header->width * pnm_header->height * 3 * 2;
                     i += 2) {
                    fscanf(fp, "%hu", &tmp);
                    pnm_header->data[i]     =
                        (unsigned char)((tmp & 0xFF00) >> 8);
                    pnm_header->data[i + 1] =
                        (unsigned char)(tmp & 0x00FF);
                }
            }
            break;

        /* PPM Binary */
        case '6':
            fscanf(fp, "%hu", &pnm_header->max_val);

            if (pnm_header->max_val <= 255) {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height
                                          * 3, sizeof(*pnm_header->data));

                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 8;
                pnm_header->num_channels = 3;

                for (i = 0; i < pnm_header->width * pnm_header->height * 3;
                     i++) {
                    pnm_header->data[i] = fgetc(fp);
                }
            }
            else {
                pnm_header->data = calloc(pnm_header->width * pnm_header->height
                                          * 3 * 2, sizeof(*pnm_header->data));

                if (pnm_header->data == NULL) {
                    fprintf(stderr, "Error: Could not allocate enough memory"
                            " for image data at line %d in file %s\n", __LINE__,
                            __FILE__);
                    exit(EXIT_FAILURE);
                }

                pnm_header->bits_per_pixel = 16;
                pnm_header->num_channels = 3;

                for (i = 0; i < pnm_header->width * pnm_header->height * 3 * 2;
                     i += 2) {
                    pnm_header->data[i]     = fgetc(fp);
                    pnm_header->data[i + 1] = fgetc(fp);
                }
            }
            break;

        default:
            break;
    }
}
/* EOF */
