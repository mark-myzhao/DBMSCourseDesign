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
#include <string.h>

class Litem;

int **a;
// int a[60000][784];
LItem **Litems;
// LItem Litems[50][60000];
float **q;
// float q[100][50];
int **queries;
// int queries[100][784];

//  main Args
int nSize = 0;  // 60000
int dSize = 0;  // 784
int queryNum = 0;  // 100
int randomSize = 50;
char datasetPath[50];
char queryPath[50];

int stringToInt(char* str) {
    int result = 0;
    for (unsigned i = 0; i < strlen(str); ++i) {
        result *= 10;
        result += (str[i] - '0');
    }
    return result;
}

void generate();

void parseArgs(int argc, char** argv);

void createArray() {
    a = new int*[nSize];
    for (int i = 0; i < nSize; ++i)
        a[i] = new int[dSize];

    Litems = new LItem*[randomSize];
    for (int i = 0; i < randomSize; ++i)
        Litems[i] = new LItem[nSize];

    q = new float*[queryNum];
    for (int i = 0; i < queryNum; ++i)
        q[i] = new float[randomSize];

    queries = new int*[queryNum];
    for (int i = 0; i < queryNum; ++i)
        queries[i] = new int[dSize];
}

void deleteArray() {
    for (int i = 0; i < nSize; ++i)
        delete[] a[i];
    delete[] a;

    for (int i = 0; i < randomSize; ++i)
        delete[] Litems[i];
    delete[] Litems;

    for (int i = 0; i < queryNum; ++i)
        delete[] q[i];
    delete[] q;

    for (int i = 0; i < queryNum; ++i)
        delete[] queries[i];
    delete[] queries;
}

int main(int argc, char** argv) {
    time_t startTime, finishTime, totalTime = 0;
    parseArgs(argc, argv);
    createArray();
    BTree* tree;
    medrank solution;
    generate();
    printf("generated!\n");
    char fileName[50];
    int ans[queryNum];
    
    startTime = clock();
    for (int i = 0; i < randomSize; ++i) {
        solution.generateFileName(i, fileName);
	    tree = new BTree;
        tree->init(fileName, BLOCKLENGTH);
        tree->bulkload(Litems[i], nSize);       
	    delete tree;
    }
    finishTime = clock();
    printf("bulkload finished!\n");
    printf("Indexing Time: %.4fs\n", (float)(finishTime - startTime) / CLOCKS_PER_SEC);
    FILE* absFileRead = fopen("./data/nearest.txt", "r");
    float resultRatio = 0, result[queryNum];
    for (int i = 0; i < queryNum; ++i) {
		// printf("run start:%d\n", i);
        result[i] = 0;
        startTime = clock();
        ans[i] = solution.runAlgorithm(q[i]);
        finishTime = clock();
        totalTime += finishTime - startTime;
        for (int j = 0; j < dSize; ++j) {
            result[i] += pow((queries[i][j] - a[ans[i]][j]),2);
        }

        result[i] = sqrt(result[i]);
        float tmp = 0;
        fscanf(absFileRead, "%f", &tmp);
        // printf("Medrank Result: %f, cloest: %f\n", result[i], tmp);
        resultRatio += (result[i] / tmp); 
    }
    printf("Average Running Time: %.4fs\n", (float)totalTime / queryNum / CLOCKS_PER_SEC);
    resultRatio /= queryNum;
    printf("------Overall Ratio: %f------\n", resultRatio);
    deleteArray();
    return 0;
}


void parseArgs(int argc, char** argv) {
    int opt;  
    int digit_optind = 0;  
    int option_index = 0;  
    char *optstring = "n:d:";  
    static struct option long_options[] = {
        {"qn", required_argument, NULL, '1'},  
        {"ds", required_argument, NULL, '2'},  
        {"qs", required_argument, NULL, '3'},  
        {0, 0, 0, 0}  
    };  
 
   while ( (opt = getopt_long_only(argc, argv, optstring, long_options, &option_index)) != -1) {  
        if (opt == 'n') {
            nSize = stringToInt((char*) optarg);
        } else if (opt == 'd') {
            dSize = stringToInt((char*) optarg);
        } else if (opt == '1') {
            queryNum = stringToInt((char*) optarg);
        } else if (opt == '2') {
            strcpy(datasetPath, (char*) optarg);
        } else if (opt == '3') {
            strcpy(queryPath, (char*) optarg);
        }
    }  
}

