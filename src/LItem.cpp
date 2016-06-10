#include "LItem.h"

void LItem::set(int index, double value) {
    index_ = index;
    value_ = value;
};

int LItem::getIndex() {
    return index_;
};

double LItem::getValue() {
    return this->value_;
};
