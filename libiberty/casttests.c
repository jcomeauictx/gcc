#include <stdio.h>
int main() {
	char test[64] = "just testing";
	double dtest = (double)(long)test;
	long ltest = (long)dtest;
	void *vtest = (void *)ltest;
	printf("test: %p %f %ld \"%s\"\n", test, dtest, ltest, (char *)vtest);
	return 0;
}
