#include "segmentator.h"
#include "rectangle.h"
#include <fstream>
#include <iostream>

int** readImageFromFile(char *fileName);
int getWidth(char *fileName);
int getHeight(char *fileName);

int main(void) {

    char fileName[] = "image.dat";
    int **image = readImageFromFile(fileName);
    int height = getHeight(fileName);
    int width = getWidth(fileName);
    if (height == 0 || width == 0) {
        return 1;
    }
    Segmentator segmentator(image, height, width);
    Rectangle **rectangles = segmentator.algorithm();

//    for (int i = 0; i < segmentator.getCountSegments(); ++i) {
//        std::cout << "height = " << rectangles[i]->height() << " ";
//        std::cout << "width = " << rectangles[i]->width() << std::endl;
//    }

    // Delete
    for (int i = 0; i < height; ++i) {
        delete [] image[i];
    }
    delete [] image;

    int size = segmentator.getCountSegments();
    for (int i = 0; i < size; ++i) {
        delete rectangles[i];
    }
    delete [] rectangles;

    return 0;
}

//============================================================================================================================

int** readImageFromFile(char *fileName) {
    std::ifstream inputFile(fileName, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        return NULL;
    }
    int height;
    inputFile.read((char *) &height, sizeof(int));
    int **image = new int * [height];
    for (int i = 0; i < height; ++i) {
        int width;
        inputFile.read((char *) &width, sizeof(int));
        image[i] = new int [width];
        inputFile.read((char *) image[i], width * sizeof(int));
    }
    inputFile.close();
    return image;
}

int getHeight(char *fileName) {
    std::ifstream inputFile(fileName, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        return 0;
    }
    int height;
    inputFile.read((char *) &height, sizeof(int));
    inputFile.close();
    return height;
}

int getWidth(char *fileName) {
    std::ifstream inputFile(fileName, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        return 0;
    }
    int width;
    inputFile.seekg(sizeof(int));
    inputFile.read((char *) &width, sizeof(int));
    inputFile.close();
    return width;
}
