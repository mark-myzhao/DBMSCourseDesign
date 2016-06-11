#ifndef __MEDRANK_H
#define __MEDRANK_H

class medrank {
public:
    medrank();
    ~medrank();
private:
    BTree** btrees_;

    char indexPath_[200];  //  PATH

    //  store file name in <fileName>
    void generateFileName(int id, char* fileName);
};

#endif