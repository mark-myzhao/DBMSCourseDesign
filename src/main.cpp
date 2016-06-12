#include "b_tree.h"
#include "medrank.h"
#include "def.h"
#include "LItem.h"
float result[100];

class Litem;

int a[60000][784];
LItem Litems[50][60000];
float q[100][50];
int queries[100][784];
void generate();

int main() {
	BTree trees[50];
	medrank solution;
	generate();
	printf("generated!");
	char fileName[50];
	int ans[100];
	for (int i = 0; i < 50; ++i)
	{
		solution.generateFileName(i, fileName);
		trees[i].init(fileName, BLOCKLENGTH);
		trees[i].bulkload(Litems[i], 60000);
		printf("bulkload finish:%d\n", i);
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

