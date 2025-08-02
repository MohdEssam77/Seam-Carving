#include "energy.h"

#include <stdlib.h>
#include <string.h>

#include "indexing.h"
#include "util.h"

uint32_t max(uint32_t a, uint32_t b) { return a > b ? a : b; }

uint32_t min(uint32_t a, uint32_t b) { return a < b ? a : b; }

/**
 * Calculate the difference of two color values @p a and @p b.
 * The result is the sum of the squares of the differences of the three (red,
 * green and blue) color components.
 * */
inline uint32_t diff_color(struct pixel const a, struct pixel const b) {
    // TODO implement (assignment 3.2)
    uint32_t redsqr = (a.r - b.r) * (a.r - b.r);
    uint32_t greensqr = (a.g - b.g) * (a.g - b.g);
    uint32_t bluesqr = (a.b - b.b) * (a.b - b.b);
    uint32_t final = redsqr + greensqr + bluesqr;
    return final;
}

/**
 * Calculate the total energy at every pixel of the image @p `img`,
 * but only considering columns with index less than @p `w`.
 * To this end, first calculate the local energy and use it to calculate the
 * total energy.
 * @p `energy` is expected to have allocated enough space
 * to represent the energy for every pixel of the whole image @p `img.
 * @p `w` is the width up to (excluding) which column in the image the energy
 * should be calculated. The energy is expected to be stored exactly analogous
 * to the image, i.e. you should be able to access the energy of a pixel with
 * the same array index.
 */
void calculate_energy(uint32_t* const energy, struct image* const img,
                      int const w) {
    for (int y = 0; y < img->h; y++) {
        for (int x = 0; x < w; x++) {
            uint32_t local_energy = 0;
            struct pixel this_pixel = img->pixels[yx_index(y, x, img->w)];
            if (x > 0) {
                struct pixel left = img->pixels[yx_index(y, x - 1, img->w)];
                local_energy = local_energy + diff_color(this_pixel, left);
            }
            if (y > 0) {
                struct pixel above = img->pixels[yx_index(y - 1, x, img->w)];
                local_energy = local_energy + diff_color(this_pixel, above);
            }
            uint32_t total;
            int index = yx_index(y, x, img->w);
            if (y == 0) {
                total = local_energy;
            } else {
                uint32_t best = UINT32_MAX;
                if (x > 0) {
                    best = min(best, energy[yx_index(y - 1, x - 1, img->w)]);
                }
                best = min(best, energy[yx_index(y - 1, x, img->w)]);
                if (x + 1 < w) {
                    best = min(best, energy[yx_index(y - 1, x + 1, img->w)]);
                }
                total = local_energy + best;
            }
            energy[index] = total;
        }
    }
}

/**
 * Calculate the index of the column with the least energy in bottom row.
 * Expects that @p `energy` holds the energy of every pixel of @p `img` up to
 * column (excluding) @p `w`. Columns with index `>= w` are not considered as
 * part of the image.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
int calculate_min_energy_column(uint32_t const* const energy, int const w0,
                                int const w, int const h) {
    // TODO implement (assignment 3.2)
    int current = 0;
    uint32_t minimum = energy[yx_index(h - 1, current, w0)];
    for (int x = 0; x < w; x++) {
        uint32_t new = energy[yx_index(h - 1, x, w0)];
        if (minimum > new) {
            current = x;
            minimum = new;
        }
    }
    return current;
}

/**
 * Calculate the optimal path (i.e. least energy), according to the energy
 * entries in @p `energy` up to (excluding) column @p `w`. The path is stored in
 * @p `seam`. Columns with index `>= w` are not considered as part of the image.
 * @p `x` is the index in the bottom row where the seam starts.
 * @p `w0` states the width of the energy matrix @p `energy`
 * @p `h` states the height of the energy matrix @p `energy`
 */
void calculate_optimal_path(uint32_t const* const energy, int const w0,
                            int const w, int const h, int x,
                            uint32_t* const seam) {
    // TODO implement (assignment 3.2)
    seam[h - 1] = x;
    for (int y = h - 2; y > -1; y--) {
        uint32_t mini = x;
        uint32_t current_mini = energy[yx_index(y, x, w0)];
        if (x > 0 && energy[yx_index(y, x - 1, w0)] < current_mini) {
            mini = x - 1;
            current_mini = energy[yx_index(y, x - 1, w0)];
        }
        if (x + 1 < w && energy[yx_index(y, x + 1, w0)] < current_mini) {
            mini = x + 1;
            current_mini = energy[yx_index(y, x + 1, w0)];
        }
        seam[y] = mini;
        x = mini;
    }
}
