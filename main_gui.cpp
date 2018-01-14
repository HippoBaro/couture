//
// Created by Hippolyte Barraud on 14/01/2018.
//

#include <opencv2/opencv.hpp>
#include "matrix.hpp"
#include "seam_carver.hpp"

void rot90(cv::Mat &image, bool revert){
    cv::transpose(image, image);
    cv::flip(image, image, revert);
}

void resize(cv::Mat &image, bool rotate = false) {
    // If the user want to scale up rather left, we have to rotate the image memory representation
    if (rotate) {
        rot90(image, false);
    }

    if (image.cols - 1 == 0)
        return;

    matrix<cv::Vec3b> image_matrix(image.rows, image.cols, (cv::Vec3b*)image.data);

    // OpenCV semantics are bizare. Those are not necessarily stand-alone matrices
    // but rather references to subparts of others.
    cv::Mat gray, dx, dy, abs_dx, abs_dy, grad, output;

    // Create a greyscale version of the image
    cvtColor(image, gray, CV_BGR2GRAY);

    // Applying Sobel on X-axis
    cv::Sobel(image, dx, CV_64F, 1, 0);
    // Applying Sobel on Y-axis
    cv::Sobel(image, dy, CV_64F, 0, 1);

    // Creating a uniform energy-map
    cv::convertScaleAbs( dx, abs_dx );
    cv::convertScaleAbs( dy, abs_dy );
    cv::addWeighted( abs_dx, 0.5, abs_dy, 0.5, 0, output );

    // Finding the seam and registering the indexes to get rid off
    matrix<uint16_t > energy(output.rows, output.cols, (uint16_t*)output.data);
    matrix<uint_fast16_t> workset(energy.height, energy.width);
    std::vector<int> rem(energy.height);

    // remove_seam_simded provides a handy Functor to get called back whenever the algorithm remove something.
    remove_seam_simded(energy, workset, [&rem] (auto const& r, auto const& c) {
        rem[r] = c;
    });

    // Removing the seam
    for (int j = 0; j < image_matrix.height; ++j) {
        image_matrix.erase(j, rem[j]);
    }
    // Creating the final image
    cv::Mat out(image.rows, image.cols - 1, image.type());
    for (int k = 0; k < image_matrix.height; ++k) {
        for (int j = 0; j < image_matrix.width - 1; ++j) {
            out.at<cv::Vec3b>(k, j) = image_matrix.at(k, j);
        }
    }

    if (rotate)
        rot90(out, true);

    image = out;
};

void CallBackFunc(int event, int x, int y, int, void* data) {
    auto *image = static_cast<cv::Mat *>(data);
    if (event == cv::EVENT_LBUTTONDOWN) {
        while (image->cols > x) resize(*image);
        while (image->rows > y) resize(*image, true);
        cv::imshow("Couture", *image);
    }
}

int main(int ac, char** av ) {
    if (ac != 2) {
        std::cout << "usage: " << av[0] <<  "image_path" << std::endl;
        return -1;
    }

    cv::Mat image = cv::imread(av[1], 1);
    image.convertTo(image, CV_8UC3);
    cv::Mat image_backup = image;
    if (!image.data) {
        printf("No image data \n");
        return EXIT_FAILURE;
    }

    cv::namedWindow("Couture", cv::WINDOW_AUTOSIZE);
    cv::setWindowTitle("Couture", "Couture: use the UP/LEFT arrows or the mouse to resize. "
            "Press BACKSPACE to cancel and ESC to quit");
    cv::setMouseCallback("Couture", CallBackFunc, &image);

    while (true) {
        cv::imshow("Couture", image);
        auto key = cv::waitKey(0);
        if (key == 'q')
            break;
        else if (key == 0) //UP
            resize(image, true);
        else if (key == 2) // LEFT
            resize(image);
        else if (key == 127)
            image = image_backup;
        else if (key == 27)
            break;
    }
    return EXIT_SUCCESS;
}