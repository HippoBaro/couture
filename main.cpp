//
// Created by Hippolyte Barraud on 06/01/2018.
//

#include <string>
#include <iostream>
#include <vector>
#include <boost/simd.hpp>
#include <boost/simd/algorithm.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include "io.hpp"

template <typename U>
void find_seam_simded(matrix<U> & energy, matrix<U> & workset) {
    boost::simd::copy_n(energy.row_at(0).data(), energy.width, workset.row_at(0).data());
    for (int r = 1; r < energy.height; ++r) {
        workset.at(r, 0) = energy.at(r, 0) + std::min(workset.at(r - 1, 1), workset.at(r - 1, 0));

        //Filling the array with max value
        boost::simd::fill(workset.row_at(r).data() + 1, workset.row_at(r).data() + workset.width, std::numeric_limits<U>::max());
        //Compare each cell with top left
        boost::simd::transform(workset.row_at(r-1).data(), workset.row_at(r-1).data() + workset.width - 1, workset.row_at(r).data() + 1, workset.row_at(r).data() + 1, boost::simd::min);
        //Compare each cell with top
        boost::simd::transform(workset.row_at(r-1).data() + 1, workset.row_at(r-1).data() + workset.width, workset.row_at(r).data() + 1, workset.row_at(r).data() + 1, boost::simd::min);
        //Compare each cell with top right
        boost::simd::transform(workset.row_at(r-1).data() + 2, workset.row_at(r-1).data() + workset.width + 1, workset.row_at(r).data() + 1, workset.row_at(r).data() + 1, boost::simd::min);
        //Add to each cell the energy value
        boost::simd::transform(energy.row_at(r).data() + 1, energy.row_at(r).data() + energy.width - 1, workset.row_at(r).data() + 1, workset.row_at(r).data() + 1, boost::simd::plus);

        workset.at(r, energy.width - 1) = energy.at(r, energy.width - 1) + std::min(workset.at(r - 1, energy.width - 2), workset.at(r - 1, energy.width - 1));
    }

    //Find the starting point of seam removal
    auto min_index = static_cast<int>(boost::simd::min_element(workset.row_at(energy.height - 1).data(), workset.row_at(energy.height - 1).data() + energy.width - 1) - workset.row_at(energy.height - 1).data());
    energy.row_at(energy.height - 1).erase(energy.row_at(energy.height - 1).cbegin() + min_index);

    for (int j = energy.height - 2; j >= 0; --j) {
        auto row_dp = workset.row_at(j);
        auto row_img = energy.row_at(j);
        if (min_index != 0 && min_index != energy.width - 1) {
            min_index = static_cast<int>(std::distance(row_dp.begin(), std::min_element(row_dp.begin() + min_index - 1, row_dp.begin() + min_index + 2)));
        }
        else if (min_index == 0) {
            min_index = static_cast<int>(std::distance(row_dp.begin(), std::min_element(row_dp.begin() + min_index, row_dp.begin() + min_index + 2)));
        }
        else {
            min_index = static_cast<int>(std::distance(row_dp.begin(), std::min_element(row_dp.begin() + min_index - 1, row_dp.begin() + min_index + 1)));
        }
        energy.row_at(j).erase(energy.row_at(j).cbegin() + min_index);
    }
}

int main(int ac, char **av) {
    std::ios_base::sync_with_stdio (false);
    std::cin.tie(nullptr);

    if (ac < 2) {
        std::cout << "Usage:" << av[0] << " input_file_path > output_file" << std::endl;
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
        find_seam_simded(energy, workset);
        energy.width = workset.width;
    }

    print_matrix(energy);
    return EXIT_SUCCESS;
}