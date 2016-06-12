#include "headers.h"

medrank::medrank() : indexPath_("./Btree") {
    btrees_ = nullptr;
}

medrank::~medrank() {
    for (int i = 0; i < TREENUM; ++i) {
        if (btrees_ != nullptr) {
            delete btrees_[i];
            btrees_[i] = nullptr;
        }
    }
    delete [] btrees_;
    btrees_ = nullptr;
}

//  projected query vector, length = 50
int medrank::runAlgorithm(LItem* q, char* BTreeFname) {  
    int *vote = new int[60000];
    memset(vote, 0, 60000);        //  初始化为0
    int curLargestVoteNum = 0;   //  记录当前最多票数
    BLeafNode* resultNode = nullptr;//  记录最终结果
    int resultIndex = -1;        //  记录最终结果
    int lowerIndex[50];          //  记录叶子节点中的位置
    int higherIndex[50];         //
    BLeafNode* lower[50];        //  记录叶子节点指针
    BLeafNode* higher[50];       //  
    for (int i = 0; i < 50; ++i) {
        lowerIndex[i] = higherIndex[i] = -1;
        lower[i] = higher[i] = nullptr;
    }

    //  初始化B+树
    BTree tree;
    tree.init_restore(BTreeFname);

    //  第一次查询
    for (int i = 0; i < 50; ++i) {
        tree.searchLowerAndHigher(q[i].getValue(),
                                   lower[i], lowerIndex[i],
                                   higher[i], higherIndex[i]);
    }

    //  投票过程
    while (curLargestVoteNum <= 25) {
        for (int i = 0; i < 50; ++i) {
            bool flag = false;  //  false->left; true->right
            if (higher[i] == nullptr && lower[i] != nullptr) {
                //  lower[lowerIndex] is nearer
                flag = false;
            } else if (higher[i] != nullptr && lower[i] == nullptr) {
                //  higher[higherIndex] is nearer
                flag = true;
            } else if (higher[i] == nullptr && lower[i] == nullptr) {
                //  error
                bool error = true;
                assert(error == false);
            } else {
                if (q[i].getValue() - lower[i]->get_key(lowerIndex[i]) < higher[i]->get_key(higherIndex[i])) {
                    //  lower[lowerIndex] is nearer
                    flag = false;
                } else {
                    //  higher[higherIndex] is nearer
                    flag = true;
                }
            }

            if (flag) {
                //  higher[higherIndex] is nearer
                vote[higher[i]->get_entry_id(higherIndex[i])]++;
                //  保存curLagerestNum为当前最大票数,更新结果
                if (vote[higher[i]->get_entry_id(higherIndex[i])] > curLargestVoteNum) {
                    curLargestVoteNum = vote[higher[i]->get_entry_id(higherIndex[i])];
                    resultNode = higher[i];
                    resultIndex = higherIndex[i];
                }

                ++higherIndex[i];
                if (higherIndex[i] >= higher[i]->get_num_keys()) {
                    higher[i] = higher[i]->get_right_sibling();
                    if (higher[i] != nullptr)
                        higherIndex[i] = 0;
                }
            } else {
                //  lower[lowerIndex] is nearer
                vote[lower[i]->get_entry_id(lowerIndex[i])]++;
                --lowerIndex[i];
                if (lowerIndex[i] < 0) {
                    lower[i] = lower[i]->get_left_sibling();
                    if (lower[i] != nullptr)
                        lowerIndex[i] = lower[i]->get_num_keys() - 1;
                }
            }
        }
    }

    if (vote != nullptr) {          //  释放内存空间
        delete [] vote;
        vote = nullptr;
    }
	return 0;
}

void medrank::generateFileName(int id, char* fileName) {
    char c[20];
    strcpy(fileName, indexPath_);
    sprintf(c, "%d", id);
    strcat(fileName, c);
    strcat(fileName, ".medrank");
}
