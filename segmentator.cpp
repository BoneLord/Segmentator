#include "segmentator.h"

Segmentator::Segmentator(int **image, int height, int width) {
    myImage = image;
    myWidth = width;
    myHeight = height;
    myCountSegments = 0;
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
