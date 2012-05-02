#ifndef SEGMENTATOR_H
#define SEGMENTATOR_H

#include <iostream>
#include <vector>

class Segmentator {
public:
    Segmentator(int **image, int height, int width);
    void algorithm();
private:
    int** myImage;
    int myWidth;
    int myHeight;
    std::vector<int*>* cutToLine(int *horizontalHistogram, int length) const;
    int* getHistogramProjectionY() const;
    int* getHistogramProjectionX(int height, int width, int row, int column) const;
};

#endif // SEGMENTATOR_H
