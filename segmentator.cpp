#include "segmentator.h"

Segmentator::Segmentator(int **image, int height, int width) {
    myWidth = width;
    myHeight = height;
    myImage = binarizationImage(image);
    myCountSegments = 0;
}

Segmentator::~Segmentator() {
    for (int i = 0; i < myHeight; ++i) {
        delete [] myImage [i];
    }
    delete [] myImage;
}

int* Segmentator::getHistogramProjectionY() const {
    /**
      Изображение должно быть бинаризовано. Иначе данный подход не работоспособен.
      */
    int *histogram = new int [myHeight];
    for (int i = 0; i < myHeight; ++i) {
        histogram[i] = 0;
        for (int j = 0; j < myWidth; ++j) {
            if (myImage[i][j] == 0) {
                ++histogram[i];
            }
        }
    }
    return histogram;
}

int* Segmentator::getHistogramProjectionX(int height, int width, int row, int column) const {
    /**
      Изображение должно быть бинаризовано. Иначе данный подход не работоспособен.
      */
    if (height == 0 || width == 0 || row >= height || column >= width) {
        return NULL;
    }
    int *histogram = new int [width];
    for (int j = column; j < width; ++j) {
        histogram[j] = 0;
        for (int i = row; i < height; ++i) {
            if (myImage[i][j] == 0) {
                ++histogram[j];
            }
        }
    }
    return histogram;
}

std::vector<int*>* Segmentator::cutToLine(int *histogram, int length) const {
    /**
      Метод нахождения разрыва между строк не будут работать с зашумлённым изображением.
      Вместо нуля надо выбрать какой-нибудь адаптивный коэффициент.
      */
    std::vector<int*> *stringText = new std::vector<int*>;
    int startLine = 0;
    while (startLine != length) {
        int stopLine = startLine;
        if (histogram[startLine] != 0) {
            ++stopLine;
            while (histogram[stopLine] != 0) {
                ++stopLine;
            }
            int *widthLine = new int [2];
            widthLine[0] = startLine;
            widthLine[1] = stopLine;
            stringText->push_back(widthLine);
            startLine = stopLine;
        }
        ++startLine;
    }
    return stringText;
}

Rectangle** Segmentator::createRect(std::vector<int*> *stringsText, std::vector<int*> **symbolsText) {
    computeCountSegments(stringsText, symbolsText);
    Rectangle **rectangles = new Rectangle * [myCountSegments];

    int countString = stringsText->size();
    int i = 0;
    for (int j = 0; j < countString; ++j) {
        int y1 = stringsText->at(j)[0];
        int y2 = stringsText->at(j)[1];
        int lengthString = symbolsText[j]->size();
        for (int k = 0; k < lengthString; ++k) {
            int x1 = symbolsText[j]->at(k)[0];
            int x2 = symbolsText[j]->at(k)[1];
            rectangles[i] = new Rectangle(x1, y1, x2, y2);
            ++i;
        }
    }
    return rectangles;
}

std::vector<int*>* Segmentator::getBorderRow() const {
    int *histogramY = getHistogramProjectionY();
    std::vector<int*> *stringsText = cutToLine(histogramY, myHeight);

    delete [] histogramY;

    return stringsText;
}

int** Segmentator::getHistogramsProjectionX(std::vector<int*> *stringsText) const {
    int countString = stringsText->size();
    int **histogramsX = new int * [countString];
    for (int i = 0; i < countString; ++i) {
        int startLine = stringsText->at(i)[0];
        int stopLine = stringsText->at(i)[1];
        histogramsX[i] = getHistogramProjectionX(stopLine, myWidth, startLine, 0);
    }
    return histogramsX;
}

std::vector<int*>** Segmentator::getBorderColumn(std::vector<int*> *stringsText) const {
    int **histogramsX = getHistogramsProjectionX(stringsText);
    int countString = stringsText->size();
    std::vector<int*> **symbolsText = new std::vector<int*> * [countString];
    for (int i = 0; i < countString; ++i) {
        symbolsText[i] = cutToLine(histogramsX[i], myWidth);
    }

    for (int i = 0; i < countString; ++i) {
        delete [] histogramsX[i];
    }
    delete [] histogramsX;

    return symbolsText;
}

Rectangle** Segmentator::algorithm() {
    std::vector<int*> *stringsText = getBorderRow();
    std::vector<int*> **symbolsText = getBorderColumn(stringsText);
    Rectangle **rectangles = createRect(stringsText, symbolsText);

    int countString = stringsText->size();
    for (int i = 0; i < countString; ++i) {
        delete [] stringsText->at(i);

        int strLength = symbolsText[i]->size();

        for (int j = 0; j < strLength; ++j) {
            delete [] symbolsText[i]->at(j);
        }
        delete symbolsText[i];
    }
    delete stringsText;

    delete [] symbolsText;

    return rectangles;
}

int Segmentator::getCountSegments() const {
    return myCountSegments;
}

void Segmentator::computeCountSegments(std::vector<int*> *stringsText, std::vector<int*> **symbolsText) {
    int size = 0;
    int countString = stringsText->size();
    for (int i = 0; i < countString; ++i) {
        size += symbolsText[i]->size();
    }
    myCountSegments = size;
}

int ** Segmentator::binarizationImage(int **image) const {
    int t = otsuThreshold(image);

    int width = myWidth;
    int height = myHeight;
    int **binaryImage = new int * [height];
    for (int i = 0; i < height; ++i) {
        binaryImage[i] = new int [width];
        int *row = image[i];
        int *rowBin = binaryImage[i];
        for (int j = 0; j < width; ++j) {
            if (row[j] > t) {
                rowBin[j] = 255;
            }
            else {
                rowBin[j] = 0;
            }
        }
    }
    return binaryImage;
}

int* Segmentator::findMaxMin(int **image) const {
    int min = image[0][0];
    int max = image[0][0];

    for (int i = 0; i < myHeight; ++i) {
        int *row = image[i];
        for (int j = 0; j < myWidth; ++j) {
            int value = row[j];
            if (value < min) {
                min = value;
            }
            if (value > max) {
                max = value;
            }
        }
    }
    int *maxAndMin = new int [2];
    maxAndMin[0] = max;
    maxAndMin[1] = min;
    return maxAndMin;
}

int* Segmentator::initializeHistogram(int histSize) const {
    int *hist = new int [histSize];

    for (int i = 0; i < histSize; ++i) {
        hist[i] = 0;
    }
    return hist;
}

int* Segmentator::computeHistogram(int **image, int max, int min) const {
    int histSize = max - min + 1;
    int *hist = initializeHistogram(histSize);

    for (int i = 0; i < myHeight; ++i) {
        int *row = image[i];
        for (int j = 0; j < myWidth; ++j) {
            int brightness = row[j] - min;
            ++hist[brightness];
        }
    }
    return hist;
}

int Segmentator::computeThreshold(int histSize, int *hist) const {
    int m = 0;
    int n = 0;
    for (int i = 0; i < histSize; ++i) {
        m += i * hist[i];
        n += hist[i];
    }

    float maxSigma = -1;
    int threshold = 0;

    int alpha1 = 0;
    int beta1 = 0;

    int length = histSize - 1;
    for (int t = 0; t < length; ++t) {
        alpha1 += t * hist[t];
        beta1 += hist[t];

        float w1 = (float)beta1 / n;

        float a = (float)alpha1 / beta1 - (float)(m - alpha1) / (n - beta1);

        float sigma = w1 * (1 - w1) * a * a;

        if (sigma > maxSigma) {
            maxSigma = sigma;
            threshold = t;
        }
    }
    return threshold;
}

int Segmentator::otsuThreshold(int **image) const {
    int *maxAndMin = findMaxMin(image);
    int max = maxAndMin[0];
    int min = maxAndMin[1];

    delete [] maxAndMin;

    int *hist = computeHistogram(image, max, min);

    int histSize = max - min + 1;
    int threshold = computeThreshold(histSize, hist);

    threshold += min;

    delete [] hist;

    return threshold;
}
