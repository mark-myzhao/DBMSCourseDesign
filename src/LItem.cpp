#include "headers.h"

void LItem::set(int index, float value) {
    index_ = index;
    value_ = value;
};

int LItem::getIndex() const {
    return index_;
};

float LItem::getValue() const {
    return this->value_;
};
