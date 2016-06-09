#ifndef _LItem_H_
#define _LItem_H_

class LItem {
  public:
    LItem(int index = -1, int value = -1) : index_(index), value_(value) {}
    int getIndex() { return index_; }
    int getValue() { return value_; }
  private:
    int index_;
    int value_;
};

#endif