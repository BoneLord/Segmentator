#ifndef SEGMENTATOR_H
#define SEGMENTATOR_H

#include <iostream>
#include <vector>
#include "rectangle.h"

class Segmentator {
private:
    int** myImage;
    int myWidth;
    int myHeight;
    int myCountSegments;
    Segmentator(const Segmentator &segmentator);
    Segmentator& operator =(const Segmentator &segmentator);
    std::vector<int*>* cutToLine(int *horizontalHistogram, int length) const;
    int* getHistogramProjectionY() const;
    int* getHistogramProjectionX(int height, int width, int row, int column) const;
    Rectangle** createRect(std::vector<int*> *stringsText, std::vector<int*> **symbolsText);
    void computeCountSegments(std::vector<int*> *stringsText, std::vector<int*> **symbolsText);
    std::vector<int*>* getBorderRow() const;
    int** getHistogramsProjectionX(std::vector<int*> *stringsText) const;
    std::vector<int*>** getBorderColumn(std::vector<int*> *stringsText) const;
    int ** binarizationImage(int **image) const;
    int otsuThreshold(int **image) const;
    int* findMaxMin(int **image) const;
    int* initializeHistogram(int histSize) const;
    int* computeHistogram(int **image, int max, int min) const;
    int computeThreshold(int histSize, int *hist) const;
public:
    Segmentator(int **image, int height, int width);
    ~Segmentator();
    Rectangle** algorithm();
    int getCountSegments() const;
};

#endif // SEGMENTATOR_H
