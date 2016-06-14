#include "b_tree.h"
#include "medrank.h"
#include "def.h"
#include "LItem.h"

#include <unistd.h>
#include <getopt.h>

float result[100];

class Litem;

//  global size args
int n_size = 0;
int d_size = 0;
int qn_size = 0;
char* db_path = NULL;
char* query_path = NULL;

int a[60000][784];
LItem Litems[50][60000];
float q[100][50];
int queries[100][784];
void generate(char* DB_PATH, char* QUERY_PATH);   //  defined in count.cpp

void parseArgs(int argc, char** argv);   //  defined in main.cpp
int stringToInt(char* strNum);           //  defined in main.cpp

int main(int argc, char** argv) {
    //  Parse Args
    parseArgs(argc, argv);

    int dataSize = -1;

    BTree trees[50];
    medrank solution;
    generate(db_path, query_path);
    printf("generated!\n");
    char fileName[50];
    int ans[100];
    for (int i = 0; i < 50; ++i) {
        solution.generateFileName(i, fileName);
        trees[i].init(fileName, BLOCKLENGTH);
        printf("Bulkload LTable %d...\n", i);
        trees[i].bulkload(Litems[i], 60000);
        printf("Bulkload finish:%d\n", i);
    }
    for (int i = 0; i < 100; ++i)
    {
        ans[i] = solution.runAlgorithm(q[i]);
        for (int j = 0; j < 784; ++j)
        {
            result[i] += queries[i][j] * a[ans[i]][j];
        }
        printf("%lf\n", result[i]);
    }
    return 0;
}

void parseArgs(int argc, char** argv) {
    printf("Parse Args...\n");
    int opt;  
    int digit_optind = 0;  
    int option_index = 0;  
    const char *optstring = "n:d:";  
    static struct option long_options[] = {
        {"qn", required_argument, NULL, '1'},  
        {"ds", required_argument, NULL, '2'},  
        {"qs", required_argument, NULL, '3'},  
        {0, 0, 0, 0}  
    };  
  
    while ( (opt = getopt_long_only(argc, argv, optstring, long_options, &option_index)) != -1)  
    {  
        //  printf("opt = %c\n", opt);
        //  printf("optarg = %s\n", (char*) optarg);
        if (opt == 'n') {
            n_size = stringToInt((char*) optarg);
        } else if (opt == 'd') {
            d_size = stringToInt((char*) optarg);
        } else if (opt == '1') {
            qn_size = stringToInt((char*) optarg);
        } else if (opt == '2') {
            db_path = (char*) optarg;
        } else if (opt == '3') {
            query_path = (char*) optarg;
        } else {
            //  do nothing
        }
    }  
    printf("Parse Args Finish\n");
    printf("Parse Result:\n n: %d, d: %d, qn: %d\ndb_path: %s\nquery_path: %s\n", n_size, d_size, qn_size, db_path, query_path);
}

int stringToInt(char* strNum) {
    int result = 0;
    for (unsigned i = 0; i < strlen(strNum); ++i) {
        result *= 10;
        result += (strNum[i] - '0');
    }
    return result;
}