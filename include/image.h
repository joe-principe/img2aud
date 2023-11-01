#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>

typedef struct pnm_header_t pnm_header_t;

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

/**
 * Sets the type of pnm type
 * @param pnm_header The header containing all the pnm information
 * @param pnm_type An array of two characters that should contain P and a number
 * @param fp The file pointer from which to read
 * @note Currently only supports P1 through P6. No support for P7 or PF
 */
void set_pnm_type(pnm_header_t *pnm_header, char *pnm_type, FILE *fp);

/**
 * Initializes the pnm header
 * @param pnm_header The pnm header
 * @param fp The file pointer from which to read
 */
void init_pnm_header(pnm_header_t *pnm_header, FILE *fp);

#endif
/* EOF */
