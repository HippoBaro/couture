//
// Created by Hippolyte Barraud on 14/01/2018.
//

#ifndef DIRECTORY_TEST_SEAM_CARVER_HPP
#define DIRECTORY_TEST_SEAM_CARVER_HPP

#include <vector>
#include <boost/simd.hpp>
#include <boost/simd/algorithm.hpp>

template <typename U, typename Func>
void remove_seam_simded(matrix<U> & energy, matrix<U> & workset, Func functor) {
    namespace bs = boost::simd;
    //Filing the very first row directly from the energy map values
    bs::copy_n(energy.row_at(0), energy.width, workset.row_at(0));
    for (int r = 1; r < energy.height; ++r) {

        //The first and last values of every row remains special one, but with this approach there is no branching
        workset.at(r, 0) = energy.at(r, 0) + std::min(workset.at(r - 1, 1), workset.at(r - 1, 0));
        auto current_row = workset.row_at(r), previous_row = workset.row_at(r-1);

        //Filling the array with max value
        bs::fill(current_row + 1, current_row + workset.width, std::numeric_limits<U>::max());
        //Compare each cell with top left
        bs::transform(previous_row, previous_row + workset.width - 1,current_row + 1, current_row + 1, bs::min);
        //Compare each cell with top
        bs::transform(previous_row + 1, previous_row + workset.width,current_row + 1, current_row + 1, bs::min);
        //Compare each cell with top right
        bs::transform(previous_row + 2, previous_row + workset.width + 1, current_row + 1, current_row + 1, bs::min);
        //Add to each cell the energy value
        bs::transform(energy.row_at(r) + 1, energy.row_at(r) + energy.width - 1, current_row + 1, current_row + 1, bs::plus);

        workset.at(r, energy.width - 1) = energy.at(r, energy.width - 1) + std::min(workset.at(r - 1, energy.width - 2), workset.at(r - 1, energy.width - 1));
    }

    //Find the starting point of seam for removal
    auto energy_last_row = workset.row_at(energy.height - 1);
    auto min_index = static_cast<int>(bs::min_element(energy_last_row, energy_last_row + energy.width - 1) - energy_last_row);
    energy.erase(energy.height - 1, min_index);

    // Unfortunately this part remains messy, I was unable to get it to work with SIMD instruction-set.
    // This is the main bottleneck of the implementation.
    for (int j = energy.height - 2; j >= 0; --j) {
        auto row_dp = workset.row_at(j);
        if (min_index != 0 && min_index != energy.width - 1) {
            min_index = static_cast<int>(std::distance(row_dp, std::min_element(row_dp + min_index - 1, row_dp + min_index + 2)));
        }
        else if (min_index == 0) {
            min_index = static_cast<int>(std::distance(row_dp, std::min_element(row_dp + min_index, row_dp + min_index + 2)));
        }
        else {
            min_index = static_cast<int>(std::distance(row_dp, std::min_element(row_dp + min_index - 1, row_dp + min_index + 1)));
        }
        functor(j, min_index);
        energy.erase(j, min_index);
    }
}

template <typename U>
void remove_seam_simded(matrix<U> & energy, matrix<U> & workset) {
    remove_seam_simded(energy, workset, [] (auto const&, auto const&) {});
}


#endif //DIRECTORY_TEST_SEAM_CARVER_HPP
