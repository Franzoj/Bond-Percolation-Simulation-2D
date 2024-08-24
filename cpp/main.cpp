#include <opencv2/opencv.hpp>
#include <random>
#include <iostream>
#include <set>

void printProgressBar(int percentage) {
    int barWidth = 50; // Width of the progress bar

    std::cout << "[";
    int pos = barWidth * percentage / 100;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << percentage << " %\r";
    std::cout.flush();
}

// Function to generate a random matrix with 0s and 1s based on probability p
cv::Mat generateMatrix(int width, int height, double p) {
    // Create a matrix of size height x width filled with zeros
    cv::Mat matrix = cv::Mat::zeros(height, width, CV_8UC1);

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    // Fill the matrix with 0s and 1s
    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            // Set the matrix element to 1 with probability p
            if(dis(gen) < p)
                matrix.at<uchar>(i, j) = 255; // Set to 255 for white pixels
            else
                matrix.at<uchar>(i, j) = 150; // Set to 0 for black pixels
        }
    }

    return matrix;
}

std::set<std::pair<int, int>> getCluster(cv::Mat matrix, int i, int j) {
    std::set<std::pair<int, int>> cluster;
    std::set<std::pair<int, int>> visited;

    // bfs to find the cluster until there are no more white neighbors
    std::queue<std::pair<int, int>> q;
    q.push({i, j});
    while(!q.empty()) {
        auto [i, j] = q.front();
        q.pop();
        if(matrix.at<uchar>(i, j) == 255 && visited.find({i, j}) == visited.end()) {
            cluster.insert({i, j});

            if(i > 0) q.push({i - 1, j}); // Up
            if(i < matrix.rows - 1) q.push({i + 1, j}); // Down
            if(j > 0) q.push({i, j - 1}); // Left
            if(j < matrix.cols - 1) q.push({i, j + 1}); // Right
            
            visited.insert({i, j});
        }
    }

    return cluster;
}

std::set<std::pair<int, int>> getBiggestCluster(cv::Mat matrix) {
    std::set<std::pair<int, int>> biggestCluster;
    std::set<std::pair<int, int>> visited;

    // Iterate over the matrix
    for(int i = 0; i < matrix.rows; ++i) {
        for(int j = 0; j < matrix.cols; ++j) {
            if(matrix.at<uchar>(i, j) == 255 && visited.find({i, j}) == visited.end()) {
                std::set<std::pair<int, int>> cluster = getCluster(matrix, i, j);
                visited.insert(cluster.begin(), cluster.end());
                if(cluster.size() > biggestCluster.size()) {
                    biggestCluster = cluster;
                }
            }
        }
    }

    return biggestCluster;
}

cv::Mat drawCluster(cv::Mat matrix, std::set<std::pair<int, int>> cluster) {
    cv::Mat result = matrix.clone();
    for(auto [i, j] : cluster) {
        result.at<uchar>(i, j) = 0; // Set to 100 for gray pixels
    }
    return result;
}



int main() {
    int width = 1000;   // Width of the matrix
    int height = 1000;  // Height of the matrix
    double p = 0.01;    // Probability of a cell being 1
    double step = 0.01; // Step size for the probability

    // Being in a 2D lattice configuration the critical parameter for bond percolation is pc = 0.5927
    double pc = 0.5927;


    // Generate the binary matrix
    while (p <= 1.0) {
        cv::Mat matrix = generateMatrix(width, height, p);
        std::set<std::pair<int, int>> biggestCluster = getBiggestCluster(matrix);
        cv::Mat result = drawCluster(matrix, biggestCluster);

        // Save the matrix as an image
        std::string filename = "imgs10k/binary_matrix_" + std::to_string(p) + ".png";
        if (cv::imwrite(filename, result)) {
            std::cout << "Image saved successfully: " << filename << std::endl;
        } else {
            std::cout << "Error: Could not save the image." << std::endl;
        }

        p += step;
        printProgressBar((p - 0.01) * 100);
    }

    return 0;
}