//
// Created by Hippolyte Barraud on 06/01/2018.
//

#include <string>
#include <iostream>
#include <vector>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "io.hpp"
#include "seam_carver.hpp"

int main(int ac, char **av) {
    std::ios_base::sync_with_stdio (false);
    std::cin.tie(nullptr);

    if (ac < 2) {
        std::cout << "Usage: " << av[0] << " input_file_path > output_file" << std::endl;
        return EXIT_FAILURE;
    }

    boost::iostreams::mapped_file mmap_file(av[1], boost::iostreams::mapped_file::readonly);
    if (!mmap_file.is_open()) {
        std::cout << "Failed to open file" << std::endl;
        return EXIT_FAILURE;
    }
    auto file_buffer = mmap_file.const_data();
    int width, height, target;

    sscanf(file_buffer, "%d %d %d", &height, &width, &target);
    assert(width > 0 && height > 0 && target >= 0 && (width - target) > 0);

    matrix<uint_fast16_t> energy(height, width);
    read_input(energy, file_buffer, mmap_file.size());

    matrix<uint_fast16_t> workset(height, width);
    auto current = width;
    while (current > (width - target)) {
        workset.width = --current;
        remove_seam_simded(energy, workset);
        energy.width = workset.width;
    }

    print_matrix(energy);
    return EXIT_SUCCESS;
}