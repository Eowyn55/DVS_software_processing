/*
 ============================================================================
 Name        : image.c
 Author      : Milica Stojiljkovic and Marko Kostic
 Version     : 1.0
 Description :
 Date        : 21.1.2016.
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include "image.h"

/* Type 1 for uint8_t image, type 2 for int16_t image. */
void image_init_from_file(image_t *im, const char *filename, int type) {
	assert((type == 1) || (type == 2));
	FILE *f;
	f = fopen(filename, "rb");
	assert(f != NULL);
	im->type = type;
	fread(&(im->width), sizeof(char), 4, f);
	fread(&(im->height), sizeof(char), 4, f);
	if (type == 1) {
		im->pixels = (void*) malloc(im->height * sizeof(char*));
		int i = 0;
		for (i = 0; i < im->height; ++i) {
			im->pixels[i] = malloc(im->width * sizeof(unsigned char));
			fread(im->pixels[i], sizeof(unsigned char), im->width, f);
		}
	} else {
		im->pixels = (void*) malloc(im->height * sizeof(signed short*));
		int i = 0;
		for (i = 0; i < im->height; ++i) {
			im->pixels[i] = malloc(im->width * sizeof(signed short));
			fread(im->pixels[i], sizeof(signed short), im->width, f);
		}
	}
	fclose(f);
}

void image_init(image_t *im, unsigned long width, unsigned long height,
		int type) {
	im->width = width;
	im->height = height;
	im->type = type;
	if (type == 1) {
		int i;
		im->pixels = malloc(im->height * sizeof(char*));
		for (i = 0; i < im->height; ++i) {
			im->pixels[i] = malloc(im->width * sizeof(unsigned char));
			memset(im->pixels[i], 0, im->width);
		}
	} else {
		int i;
		im->pixels = malloc(im->height * sizeof(short*));
		for (i = 0; i < im->height; ++i) {
			im->pixels[i] = malloc(im->width * sizeof(signed short));
			memset(im->pixels[i], 0, sizeof(signed short) * im->width);
		}
	}
}

void coeffs_init_from_file(image_t *im, const char *filename, int filterSize) {
	FILE *f;
	f = fopen(filename, "rb");
	assert(f != NULL);
	im->width = filterSize;
	im->height = filterSize;
	im->type = 2;
	im->pixels = (void*) malloc(im->height * sizeof(signed short*));
	int i = 0;
	for (i = 0; i < im->height; ++i) {
		im->pixels[i] = malloc(im->width * sizeof(signed short));
		fread(im->pixels[i], sizeof(signed short), im->width, f);
	}

}

void image_deinit(image_t *im) {
	int i;
	for (i = 0; i < im->height; ++i)
		free(im->pixels[i]);
	free(im->pixels);
}

void image_write_to_file(image_t *im, const char* filename) {
	FILE *f = fopen(filename, "wb");
	assert(f != NULL);
	fwrite((void*) &(im->width), sizeof(unsigned char), 4, f);
	fwrite((void*) &(im->height), sizeof(unsigned char), 4, f);
	if (im->type == 1) {
		int i;
		for (i = 0; i < im->height; ++i)
			fwrite(im->pixels[i], sizeof(unsigned char), im->width, f);
	} else {
		int i;
		for (i = 0; i < im->height; ++i)
			fwrite(im->pixels[i], sizeof(signed short), im->width, f);
	}
	fclose(f);
}

void image_add_block(image_t* dest, image_t* src, unsigned long start_x,
		unsigned long start_y) {
	assert(start_x + src->height < dest->height);
	assert(start_y + src->width < dest->width);

	int i;

	if (dest->type == 1 && src->type == 1) {
		for (i = 0; i < src->height; ++i) {
			memcpy(dest->pixels[i + start_x] + start_y, src->pixels[i], src->width);
		}
	}
}

void copy_row(image_t *im, unsigned dest, unsigned src) {
	memcpy(im->pixels[dest], im->pixels[src], im->type * im->width);
}

void copy_column(image_t *im, unsigned dest, unsigned src) {
	unsigned char **in_pixels = (unsigned char **) im->pixels;
	int i;
	for (i = 0; i < im->height; ++i) {
		in_pixels[i][dest] = in_pixels[i][src];
	}
}

void image_edge_repeat(image_t *im, int filter_order) {
	int n = filter_order / 2;
	int i = 0;
	for (i = 0; i < n; ++i) {
		copy_row(im, i, n);
		copy_row(im, im->height - i - 1, im->height - n - 1);
		copy_column(im, i, n);
		copy_column(im, im->width - i - 1, im->width - n - 1);
	}
}

void fir_filter(image_t *in_image, image_t *coeffs, image_t *out_image) {
	int x, y, coeff_x, coeff_y;
	int n = coeffs->width / 2;
	unsigned char **in_pixels = (unsigned char **) in_image->pixels;
	signed short ** coeff_pixels = (signed short **) coeffs->pixels;
	signed short ** out_pixels = (signed short **) out_image->pixels;
	for (x = n; x < in_image->height - n; ++x) {
		for (y = n; y < in_image->width - n; ++y) {
			int out_pixel = 0;
			for (coeff_x = 0; coeff_x < coeffs->height; ++coeff_x) {
				for (coeff_y = 0; coeff_y < coeffs->width; ++coeff_y) {
					out_pixel += (int)in_pixels[x + coeff_x - n][y
							+ coeff_y - n]
							* (int)coeff_pixels[coeff_x][coeff_y];
				}
			}

			out_pixel = out_pixel >> 6;
			if (out_pixel >= 32768)
				out_pixel = 32768 - 1;
			else if (out_pixel < (-32768))
				out_pixel = -32768;
			out_pixels[x - n][y - n] = (signed short) out_pixel;
		}
	}
}
