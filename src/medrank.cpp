#include "medrank.h"
#include "b_node.h"
#include "b_tree.h"
#include "def.h"
#include <cstring>

extern int nSize;  // 60000
extern int dSize;  // 784
extern int queryNum;  // 100
extern char datasetPath[50];
extern char queryPath[50];

medrank::medrank() {

}
medrank::~medrank() {

}

//  projected query vector, length = 50
int medrank::runAlgorithm(float* q){  
    int *vote = new int[nSize + 1];
    for (int i = 0; i < nSize + 1; ++i) {
        vote[i] = 0;
    }
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
    BTree* tree[50];
    char BTreeFname[50];

    //  第一次查询
    for (int i = 0; i < 50; ++i) {
        generateFileName(i, BTreeFname);
	    tree[i] = new BTree;
		tree[i]->init_restore(BTreeFname);
		tree[i]->searchLowerAndHigher(q[i],
                                   lower[i], lowerIndex[i],
                                   higher[i], higherIndex[i]);

    }
    //  投票过程
    while (curLargestVoteNum <= 25) {
		// printf("current:%d\n", curLargestVoteNum);
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
				error("higher == NULL and lower  == NULL", true);
            } else {
                if (q[i] - lower[i]->get_key(lowerIndex[i]) < higher[i]->get_key(higherIndex[i]) - q[i]) {
                    //  lower[lowerIndex] is nearer
                    flag = false;
                } else {
                    //  higher[higherIndex] is nearer
                    flag = true;
                }
            }

            if (flag) {
                //  higher[higherIndex] is nearer
				int index = higher[i]->get_entry_id(higherIndex[i]) - 1;
                //printf("%d\n", index);
                vote[index]++;
                //  保存curLagerestNum为当前最大票数,更新结果
                if (vote[index] > curLargestVoteNum) {
                    curLargestVoteNum = vote[index];
                }
                if (curLargestVoteNum > 25) {
                    resultNode = higher[i];
                    resultIndex = higherIndex[i];
                    break;
                }
                //if (i == 0)
                //    printf("qi:%.4f, select:%.4f\n", q[i], higher[i]->get_key(higherIndex[i]));
                ++higherIndex[i];
                if (higherIndex[i] >= higher[i]->get_num_entries()) {
                    higher[i] = higher[i]->get_right_sibling();
                    if (higher[i] != nullptr)
                        higherIndex[i] = 0;
                }
            } else {
                //  lower[lowerIndex] is nearer
				int index = lower[i]->get_entry_id(lowerIndex[i]) - 1;
                //printf("%d\n", index);
                vote[index]++;
				//  保存curLagerestNum为当前最大票数,更新结果
				if (vote[index] > curLargestVoteNum) {
					curLargestVoteNum = vote[index];
                }
                if (curLargestVoteNum > 25) {
					resultNode = lower[i];
					resultIndex = lowerIndex[i];
                    break;
                }
                //if (i == 0)
                //    printf("qi:%.4f, select:%.4f\n", q[i], lower[i]->get_key(lowerIndex[i]));
                --lowerIndex[i];
                if (lowerIndex[i] < 0) {
                    lower[i] = lower[i]->get_left_sibling();
                    if (lower[i] != nullptr)
                        lowerIndex[i] = lower[i]->get_num_entries() - 1;
                }
            }
        }
    }
    int returnResult = resultNode->get_entry_id(resultIndex) - 1;

    if (vote != nullptr) {          //  释放内存空间
       delete [] vote;
       vote = nullptr;
    }

    for (int i = 0; i < 50; ++i) {
        delete tree[i];
    }

	return returnResult;
}
void medrank::generateFileName(int id, char* fileName) {
    sprintf(fileName, "./%d.medrank", id);
}

