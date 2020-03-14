#include "my_heap.h"

int main(void)
{
	myHeap = malloc(HEAP_SIZE);
	MyMemset(myHeap, '@', HEAP_SIZE);

	system("pause");
	return 0;
}