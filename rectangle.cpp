#include "rectangle.h"

Rectangle::Rectangle(int x1, int y1, int x2, int y2) {
    if (x1 < x2 && y1 < y2) {
        myX1 = x1;
        myX2 = x2;
        myY1 = y1;
        myY2 = y2;
    }
    else {
        myX1 = 0;
        myX2 = 0;
        myY1 = 0;
        myY2 = 0;
    }
}

int Rectangle::width() const {
    return myX2 - myX1;
}

int Rectangle::height() const {
    return myY2 - myY1;
}

int Rectangle::x() const {
    return myX1;
}

int Rectangle::y() const {
    return myY1;
}
