#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/image.h"
#include "../include/audio.h"

int
main(int argc, char **argv)
{
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

    /* Open the image file */
    if ((in_fp = fopen(in_filename, "r")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for reading\n",
                in_filename);
        exit(EXIT_FAILURE);
    }

    set_pnm_type(&pnm_header, pnm_type, in_fp);

    fscanf(in_fp, "%u", &pnm_header.width);
    fscanf(in_fp, "%u", &pnm_header.height);
    pnm_header.max_val = 1;

    init_pnm_header(&pnm_header, in_fp);

    /* Open the audio file */
    if ((out_fp = fopen(out_filename, "w")) == NULL) {
        fprintf(stderr, "Error: Could not open file %s for writing\n",
                out_filename);
        exit(EXIT_FAILURE);
    }

    init_wav_header(&wav_header, &pnm_header);
    set_wav_data(&wav_header, &pnm_header);
    write_wav_header(&wav_header, &pnm_header, out_fp);

    free(pnm_header.data);
    pnm_header.data = NULL;

    free(wav_header.data);
    wav_header.data = NULL;

    fclose(in_fp);
    in_fp = NULL;
    
    fclose(out_fp);
    out_fp = NULL;

    return 0;
}
/* EOF */
