#include <stdio.h>
int main() {
	char test[64] = "just testing";
	double dtest = (double)(long)test;
	long ltest = (long)dtest;
	void *vtest = (void *)ltest;
	printf("test: %p %f %ld \"%s\"\n", test, dtest, ltest, (char *)vtest);
	/* https://gcc.gnu.org/onlinedocs/gcc/Compound-Literals.html */
	struct foo {int a; char b[2];} structure;
	/* skip this one, it assumes predefined x and y */
	/*structure = ((struct foo) {x + y, 'a', 0});*/
	char **foo = (char *[]) { "x", "y", "z" };
	int i = ++(int) { 1 };
	static struct foo x = (struct foo) {1, 'a', 'b'};
	static int y[] = (int []) {1, 2, 3};
	static int z[] = (int [3]) {1};
	return 0;
}
