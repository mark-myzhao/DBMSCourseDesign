#include "header.h"

medrank::medrank() : indexPath_("./Btree") {
    btrees_ = NULL;
}

medrank::~medrank() {
    for (int i = 0; i < TREENUM; ++i) {
        if (btrees_ != NULL) {
            delete btrees_[i];
            btrees_[i] = NULL;
        }
    }
    delete [] btrees_;
    btrees_ = NULL;
}

void medrank::init(char* LItemfilePath) {
    //  TODO
    //  read from file, build B+ trees
    // char* fileName;
    // strcpy(fileName, LItemfilePath);
    // for (int i = 0; i < TREENUM; ++i) {
    //     sprintf(c, "%d", id);
    //     strcat(fileName, c);
    //     strcat(fileName, ".LItem");
    // }
}

void medrank::generateFileName(int id, char* fileName) {
    char c[20];
    strcpy(fname, indexPath_);
    sprintf(c, "%d", id);
    strcat(fname, c);
    strcat(fname, ".medrank");
}

void medrank::runAlgorithm(int q[50], int result) {
    //  TODO
}