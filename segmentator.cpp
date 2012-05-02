#include "segmentator.h"

Segmentator::Segmentator(int **image, int height, int width) {
    myImage = image;
    myWidth = width;
    myHeight = height;
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

void Segmentator::algorithm() {

    // Получаем проекцию гистограммы на ось Y (вертикальная ось)
    int *histogramY = getHistogramProjectionY();

    // Разрезаем текст на строки
    std::vector<int*> *stringsText = cutToLine(histogramY, myHeight);

    // Получение проекций гистограмм на ось X (горизонтальная ось)
    int countString = stringsText->size();
    int **histogramX = new int * [countString];
    for (int i = 0; i < countString; ++i) {
        int startLine = stringsText->at(i)[0];
        int stopLine = stringsText->at(i)[1];
        histogramX[i] = getHistogramProjectionX(stopLine, myWidth, startLine, 0);
    }

    // Debug
//    for (int i = 0; i < countString; ++i) {
//        for (int j = 0; j < myWidth; ++j) {
//            std::cout << histogramX[i][j] << " ";
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;

    // Разрезаем получившиеся строки на слова
    std::vector<int*> **symbolsText = new std::vector<int*> * [countString];
    for (int i = 0; i < countString; ++i) {
        symbolsText[i] = cutToLine(histogramX[i], myWidth);
    }

    // Debug
//    for (int i = 0; i < myHeight; ++i) {
//        std::cout << histogramY[i] << " ";
//    }
//    std::cout << std::endl;

    // Delete
    delete [] histogramY;
    int length = stringsText->size();
    for (int i = 0; i < length; ++i) {
        delete [] stringsText->at(i);
        delete [] histogramX[i];
        int strLength = symbolsText[i]->size();
        for (int j = 0; j < strLength; ++j) {
            delete [] symbolsText[i]->at(j);
        }
        delete symbolsText[i];
    }
    delete stringsText;
    delete [] histogramX;
    delete [] symbolsText;
}
