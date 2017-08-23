/*
 ============================================================================
 Name        : main.c
 Author      : Milica Stojiljkovic and Marko Kostic
 Version     : 1.0
 Description :
 Date        : 21.1.2016.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "altera_avalon_performance_counter.h"
#include "alt_types.h"
#include "system.h"
#include "sys/alt_cache.h"
#include "image.h"

int main() {
	while (1) {

		image_t in_image;
		image_t exp_image;
		image_t coeff;
		image_t out_image;

		char coef_file_name[50];
		char coef_file_name_full[] = "/mnt/host/";
		printf("Give the name of the bin file with coeficients:\n");
		fflush( stdout);
		scanf("%s", coef_file_name);
		strcat(coef_file_name_full, coef_file_name);
		coeffs_init_from_file(&coeff, coef_file_name_full);

		char image_file_name[50];
		char image_file_name_full[] = "/mnt/host/";
		printf("Give the name of the bin file with image pixels:\n");
		fflush( stdout);
		scanf("%s", image_file_name);
		strcat(image_file_name_full, image_file_name);
		image_init_from_file(&in_image, image_file_name_full, 1);

		image_init(&out_image, in_image.width, in_image.height, 2);
		image_init(&exp_image, in_image.width + FILTER_ORDER - 1,
				in_image.height + FILTER_ORDER - 1, 1);

		printf("Start image processing...\n");
		PERF_RESET(PERFORMANCE_COUNTER_BASE);
		PERF_START_MEASURING(PERFORMANCE_COUNTER_BASE);

		PERF_BEGIN(PERFORMANCE_COUNTER_BASE, 1);
		image_add_block(&exp_image, &in_image, FILTER_ORDER / 2,
		FILTER_ORDER / 2);
		image_edge_repeat(&exp_image, FILTER_ORDER);
		PERF_END(PERFORMANCE_COUNTER_BASE, 1);

		//image_write_to_file(&exp_image, "exp_image.bin");

		PERF_BEGIN(PERFORMANCE_COUNTER_BASE, 2);
		fir_filter(&exp_image, &coeff, &out_image);
		PERF_END(PERFORMANCE_COUNTER_BASE, 2);

		image_write_to_file(&out_image, "/mnt/host/output.bin");
		printf("Finishing image processing...\n");
		printf("Cleaning heap...\n\n");
		image_deinit(&in_image);
		image_deinit(&exp_image);
		image_deinit(&coeff);
		image_deinit(&out_image);

		perf_print_formatted_report((void *) PERFORMANCE_COUNTER_BASE,
				alt_get_cpu_freq(), 2, "Exp image", "fir");

	}
}
