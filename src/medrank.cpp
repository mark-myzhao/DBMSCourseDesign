#include "header.h"

medrank::medrank() : indexPath_("./Btree") {
    btrees_ = NULL;
}

medrank::~medrank() {
    for (int i = 0; i < BTree; ++i) {
        if (btrees_ != NULL) {
            delete btrees_[i];
            btrees_[i] = NULL;
        }
    }
    delete [] btrees_;
    btrees_ = NULL;
}

void medrank::init() {
  //  todo
}

void medrank::generateFileName(int id, char* fileName) {
    strcpy(fname, indexPath_);
    sprintf(c, "%d", id);
    strcat(fname, c);
    strcat(fname, ".medrank");
}