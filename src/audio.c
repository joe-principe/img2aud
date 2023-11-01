#include <stdio.h>
#include <stdlib.h>

#include "../include/audio.h"

/* Initializes the wave header with certain parameters */
void 
init_wav_header(wav_header_t *wav_header, pnm_header_t *pnm_header)
{
    wav_header->chunk_id = "RIFF";
    wav_header->format = "WAVE";
    wav_header->subchunk_1_id = "fmt ";
    wav_header->subchunk_1_size = 16;
    wav_header->audio_format = 1;
    wav_header->subchunk_2_id = "data";

    /* TODO: Allow user to specify the file settings (eg, stereo) */
    wav_header->num_channels = 1;
    wav_header->sample_rate = 44100;
    wav_header->bits_per_sample = 8;
    wav_header->byte_rate = wav_header->sample_rate * wav_header->num_channels
                           * wav_header->bits_per_sample / 8;
    wav_header->block_align = wav_header->num_channels
                             * wav_header->bits_per_sample / 8;
    wav_header->subchunk_2_size = pnm_header->width * pnm_header->height
                                 * pnm_header->num_channels
                                 * wav_header->num_channels
                                 * wav_header->bits_per_sample / 8;
    wav_header->chunk_size = 36 + wav_header->subchunk_2_size;
    wav_header->data = calloc(pnm_header->width * pnm_header->height
                             * pnm_header->num_channels
                             * wav_header->num_channels,
                             wav_header->bits_per_sample / 8);
}

/* Sets the wave audio data */
void 
set_wav_data(wav_header_t *wav_header, pnm_header_t *pnm_header)
{
    unsigned int i;
    signed short stmp;
    /* 
     * TODO: Add support for more than two audio channels. Will this ever
     * actually come up? Probably not, but it'd be nice to support upto
     * eight-channel audio as well
     */

    /* 
     * That'd be kind of a waste, but what do I care? This project is just to
     * flex my programming muscles, lol
     */

    if (wav_header->bits_per_sample == 8) {
        /* 
         * For 8 bits per channel to 8 bits per sample, simply copy the data
         */
        if (pnm_header->bits_per_pixel == 8) {
            for (i = 0; i < pnm_header->width * pnm_header->height
                 * pnm_header->num_channels; i++) {

                wav_header->data[wav_header->num_channels * i] =
                    pnm_header->data[i];
                
                /* If there are 2 audio channels, set the right channel equal to
                 * the left channel */
                if (wav_header->num_channels == 2) {
                    wav_header->data[2 * i + 1] = wav_header->data[2 * i];
                }
            }
        }
        /* 
         * For 16 bits per channel to 8 bits per sample, constrain the
         * data down to 8 bits by dividing the original data by 256
         */
        else {
            for (i = 0; i < pnm_header->width * pnm_header->height * 2; i += 2) {
                wav_header->data[i / 2] =
                    (pnm_header->data[i] | pnm_header->data[i + 1]) / 256;

                if (wav_header->num_channels == 2) {
                    wav_header->data[i + 1] = wav_header->data[i / 2];
                }
            }
        }
    }
    else if (wav_header->bits_per_sample == 16) {
        /* 
         * For 8 bits per channel to 16 bits per sample, first expand the data
         * up to 16 bits by multiplying the original data by 256. Next, subtract
         * by 2^15 to center around 0
         */
        if (pnm_header->bits_per_pixel == 8) {
            for (i = 0; i < pnm_header->width * pnm_header->height
                 * pnm_header->num_channels; i++) {

                stmp = pnm_header->data[i] * 256 - 32768;
                wav_header->data[2 * wav_header->num_channels * i] =
                    (unsigned char)(stmp & 0xFF00);
                wav_header->data[2 * wav_header->num_channels * i + 1] =
                    (unsigned char)(stmp & 0x00FF);

                if (wav_header->num_channels == 2) {
                    wav_header->data[4 * i + 2] = wav_header->data[4 * i];
                    wav_header->data[4 * i + 3] = wav_header->data[4 * i + 1];
                }
            }
        }

        /* 
         * For 16 bits per channel to 16 bits per sample, simply copy the data
         */
        else {
            for (i = 0; i < pnm_header->width * pnm_header->height
                 * pnm_header->num_channels; i += 2) {

                wav_header->data[wav_header->num_channels * i] =
                    pnm_header->data[i];
                wav_header->data[wav_header->num_channels * i + 1] =
                    pnm_header->data[i + 1];

                if (wav_header->num_channels == 2) {
                    wav_header->data[2 * i + 2] = wav_header->data[2 * i];
                    wav_header->data[2 * i + 3] = wav_header->data[2 * i + 1];
                }
            }
        }
    }

}

/* Writes the info in the wave header out to disk */
void 
write_wav_header(wav_header_t *wav_header, pnm_header_t *pnm_header, FILE *fp)
{
    fwrite(wav_header->chunk_id, sizeof(char), 4, fp);
    fwrite(&wav_header->chunk_size, sizeof(unsigned int), 1, fp);
    fwrite(wav_header->format, sizeof(char), 4, fp);
    fwrite(wav_header->subchunk_1_id, sizeof(char), 4, fp);
    fwrite(&wav_header->subchunk_1_size, sizeof(unsigned int), 1, fp);
    fwrite(&wav_header->audio_format, sizeof(unsigned short), 1, fp);
    fwrite(&wav_header->num_channels, sizeof(unsigned short), 1, fp);
    fwrite(&wav_header->sample_rate, sizeof(unsigned int), 1, fp);
    fwrite(&wav_header->byte_rate, sizeof(unsigned int), 1, fp);
    fwrite(&wav_header->block_align, sizeof(unsigned short), 1, fp);
    fwrite(&wav_header->bits_per_sample, sizeof(unsigned short), 1, fp);
    fwrite(wav_header->subchunk_2_id, sizeof(char), 4, fp);
    fwrite(&wav_header->subchunk_2_size, sizeof(unsigned int), 1, fp);
    fwrite(wav_header->data, sizeof(unsigned char),
           pnm_header->width * pnm_header->height * pnm_header->num_channels
           * wav_header->num_channels, fp);
}
/* EOF */
