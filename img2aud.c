#include <stdio.h>
#include <stdlib.h>

typedef union pgm_data_t pgm_data_t;
typedef union ppm_data_t ppm_data_t;

union pgm_data_t
{
    char *data_8bit;
    short *data_16bit;
};

struct ppm_8bit
{
    char *red_data;
    char *blue_data;
    char *green_data;
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


int
main(int argc, char **argv)
{
    int i, width, height, max_val;
    char *data = NULL;
    pgm_data_t pgm_data;
    ppm_data_t ppm_data;
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
    
    /* TODO: Look up WAVE audio format */

    return 0;
}
/* EOF */
