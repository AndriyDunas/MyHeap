#ifndef MY_HEAP_H
#define MY_HEAP_H

#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE 1000

typedef struct list_elem	//елемент списку - елемент службової частини
{
	struct list_elem *_prev, *_next;	//вказівники на попередній та наступний елементи списку
	void *_pdata;						//вказівник на блок даних, виділених користувачу
	size_t _size;					//розмір цього блоку
} LEL;

void* MyMalloc(size_t msize);
void* MyRealloc(void *ptr, size_t newsize);
size_t MyFree(void *ptr);
int MyDefrag(size_t nparam, size_t nbytes, void **ptr, ...);	//функція дефрагментує(зсуває) вказані блоки даних користувача на num байтів
void MyMemmap(void);				//виведення на екран стану пам'яті
void* MyMemset(void *ptr, char value, size_t num);		
void* MyMemmove(void *src, void *dest, size_t num);
void* MyMemcpy(void *src, void *dest, size_t num);

void AddListElem(LEL* pnew);		//додаємо новий елемент списку(елемент службової частини)
void DeleteListElem(LEL *pdel);		//видалити елемент списку
void PrintList(void);				//виведення даних у службовій частині

int GetFreeMemory(void);
size_t GetAlign(void);		//шукаємо позицію, куди буде записаний новий блок даних користувача
LEL* IsInHeap(void *ptr);

void *myHeap;				//вказівник на початок моєї ДП
LEL *list_beg, *list_end;	//вказівники на початок і кінець службової частини
size_t badMemory;

#endif // !MY_HEAP_H