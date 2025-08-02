#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "argparser.h"
#include "energy.h"
#include "image.h"
#include "util.h"

/**
 * Compute the brightness and print the statistics of @p `img`,
 * i.e. width, height & brightness.
 */
// adding this comment just to test the push/ now testing again after
// dockerization
void statistics(struct image* img) {
    // TODO implement (assignment 3.1)
    uint8_t avg_brightness = image_brightness(img);
    printf("width: %u\n", img->w);
    printf("height: %u\n", img->h);
    printf("brightness: %u\n", avg_brightness);
}
/**
 * Find & print the minimal path of @p `img`.
 */
void find_print_min_path(struct image* img) {
    // TODO implement (assignment 3.2)
    int size = img->h * img->w;
    uint32_t* copy = malloc(size * sizeof(uint32_t));
    calculate_energy(copy, img, img->w);
    int start = calculate_min_energy_column(copy, img->w, img->w, img->h);
    uint32_t* seam = malloc(img->h * sizeof(uint32_t));
    calculate_optimal_path(copy, img->w, img->w, img->h, start, seam);
    for (int y = 0; y < img->h; y++) {
        printf("%u\n", seam[y]);
    }
    free(copy);
    free(seam);
}

/**
 * Find & carve out @p `n` minimal paths in @p `img`.
 * The image size stays the same, instead for every carved out path there is a
 * column of black pixels appended to the right.
 */
void find_and_carve_path(struct image* const img, int n) {
    int actual_w = img->w;
    int curr_w = actual_w;
    uint32_t* seam = malloc(img->h * sizeof *seam);
    uint32_t* energy = malloc(actual_w * img->h * sizeof(uint32_t));
    for (int i = 0; i < n; i++) {
        calculate_energy(energy, img, curr_w);
        int start =
            calculate_min_energy_column(energy, actual_w, curr_w, img->h);
        calculate_optimal_path(energy, actual_w, curr_w, img->h, start, seam);
        carve_path(img, curr_w, seam);
        curr_w--;
    }
    free(seam);
    free(energy);
    image_write_to_file(img, "out.ppm");
}

/**
 * Parse the arguments and call the appropriate functions as specified by the
 * arguments.
 */
int main(int const argc, char** const argv) {
    // DO NOT EDIT
    bool show_min_path = false;
    bool show_statistics = false;
    int n_steps = -1;

    char const* const filename =
        parse_arguments(argc, argv, &show_min_path, &show_statistics, &n_steps);
    if (!filename) return EXIT_FAILURE;

    struct image* img = image_read_from_file(filename);

    if (show_statistics) {
        statistics(img);
        image_destroy(img);
        return EXIT_SUCCESS;
    }

    if (show_min_path) {
        find_print_min_path(img);
    } else {
        if (n_steps < 0 || n_steps > img->w) n_steps = img->w;

        find_and_carve_path(img, n_steps);
    }

    image_destroy(img);
    return EXIT_SUCCESS;
}
