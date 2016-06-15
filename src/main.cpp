#include "b_tree.h"
#include "b_node.h"
#include "medrank.h"
#include "def.h"
#include "LItem.h"

#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>

float result[100];

class Litem;

int a[60000][784];
LItem Litems[50][60000];
float q[100][50];
int queries[100][784];
void generate();

// void parseArgs(int argc, char** argv);

int main(int argc, char** argv) {
    //  Test Parse
    // printf("Test ParseArgs\n");
    // parseArgs(argc, argv);
    //  int dataSize = -1;

    BTree* tree;
    medrank solution;
    generate();
    printf("generated!");
    char fileName[50];
    int ans[100];
    for (int i = 0; i < 50; ++i) {
        solution.generateFileName(i, fileName);
	    tree = new BTree;
        tree->init(fileName, BLOCKLENGTH);
        tree->bulkload(Litems[i], 60000);
        printf("bulkload finish:%d\n", i);
	    delete tree;

        // printf("----------\nprint tree\n");
        // tree = new BTree;
        // tree->init_restore(fileName);
        // BLeafNode* look = new BLeafNode;
        // look->init_restore(tree, 1);
        // // while (look->get_right_sibling() != nullptr) {
        //     for (int k = 0; k < 50; ++k) {
        //         printf("(%d, %.4f) ", look->get_entry_id(k), look->get_key(k));
        //     }
        //     //look = look->get_right_sibling();
        // // }
        // printf("print tree end\n----------\n");
        // getchar();
    }

    FILE* absFileRead = fopen("./data/nearest.txt", "r");
    float resultRatio = 0;
    for (int i = 0; i < 100; ++i) {
		printf("run start:%d\n", i);
        ans[i] = solution.runAlgorithm(q[i]) - 1;
        // ans[i] = rand() % 60000;
        for (int j = 0; j < 784; ++j) {
            result[i] += pow((queries[i][j] - a[ans[i]][j]),2);
        }

        result[i] = sqrt(result[i]);
        float tmp = 0;
        fscanf(absFileRead, "%f", &tmp);
        printf("Medrank Result: %f, real: %f\n", result[i], tmp);
        resultRatio += (result[i] / tmp); 
    }

    resultRatio /= 100;
    printf("------Aver Ratio: %f\n------\n", resultRatio);

    return 0;
}


// void parseArgs(int argc, char** argv) {
//    int opt;  
//    int digit_optind = 0;  
//    int option_index = 0;  
//    char *optstring = "n:d:";  
//    static struct option long_options[] = {
//        {"qn", required_argument, NULL, '1'},  
//        {"ds", required_argument, NULL, '2'},  
//        {"qs", required_argument, NULL, '3'},  
//        {0, 0, 0, 0}  
//    };  
 
//    while ( (opt = getopt_long_only(argc, argv, optstring, long_options, &option_index)) != -1) {  
//        printf("optarg = %s\n", (char*) optarg);  
//    }  
// }