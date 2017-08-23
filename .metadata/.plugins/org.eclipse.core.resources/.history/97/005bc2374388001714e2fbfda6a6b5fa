/*
 ============================================================================
 Name        : image.h
 Author      : Milica Stojiljkovic and Marko Kostic
 Version     : 1.0
 Description :
 Date        : 21.1.2016.
 ============================================================================
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdlib.h>

#define DEBUG

#ifndef DEBUG
#define assert(n)
#else
#define assert(n) \
	if (!(n)) { \
		printf("%s - Failed. ", #n); \
		printf("On %s. ", __DATE__); \
		printf("At %s. ", __TIME__); \
		printf("In file %s. ", __FILE__); \
		printf("At line %d.\n", __LINE__); \
		exit(1); \
	}
#endif

#define FILTER_ORDER    (7)

typedef struct {
	unsigned long width;
	unsigned long height;
	int type;
	void **pixels;
} image_t;

void image_init_from_file(image_t *im, const char *filename, int type);
void image_init(image_t *im, unsigned long width, unsigned long height,
		int type);
void coeffs_init_from_file(image_t *im, const char *filename);
void image_deinit(image_t *im);
void image_write_to_file(image_t *im, const char* filename);
void image_add_block(image_t* dest, image_t* src, unsigned long start_x,
		unsigned long start_y);
void image_edge_repeat(image_t *im, int filter_order);
void fir_filter(image_t *in_image, image_t *coeffs, image_t *out_image);

#endif /* IMAGE_H_ */
