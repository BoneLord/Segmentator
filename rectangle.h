#ifndef RECTANGLE_H
#define RECTANGLE_H

class Rectangle {
private:
    int myX1;
    int myX2;
    int myY1;
    int myY2;
    Rectangle(const Rectangle &rectangle);
    Rectangle& operator=(const Rectangle &rectangle);
public:
    Rectangle(int x1, int y1, int x2, int y2);
    int width() const;
    int height() const;
    int x() const;
    int y() const;
};

#endif // RECTANGLE_H
