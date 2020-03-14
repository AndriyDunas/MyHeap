#ifndef MY_HEAP_H
#define MY_HEAP_H

#include <stdio.h>
#include <stdlib.h>

#define HEAP_SIZE 1000

typedef struct list_elem	//������� ������ - ������� �������� �������
{
	struct list_elem *_prev, *_next;	//��������� �� ��������� �� ��������� �������� ������
	void *_pdata;						//�������� �� ���� �����, �������� �����������
	size_t _size;					//����� ����� �����
} LEL;

void* MyMalloc(size_t msize);
void* MyRealloc(void *ptr, size_t newsize);
size_t MyFree(void *ptr);
int MyDefrag(size_t nparam, size_t nbytes, void **ptr, ...);	//������� �����������(�����) ������ ����� ����� ����������� �� num �����
void MyMemmap(void);				//��������� �� ����� ����� ���'��
void* MyMemset(void *ptr, char value, size_t num);		
void* MyMemmove(void *src, void *dest, size_t num);
void* MyMemcpy(void *src, void *dest, size_t num);

void AddListElem(LEL* pnew);		//������ ����� ������� ������(������� �������� �������)
void DeleteListElem(LEL *pdel);		//�������� ������� ������
void PrintList(void);				//��������� ����� � �������� ������

int GetFreeMemory(void);
size_t GetAlign(void);		//������ �������, ���� ���� ��������� ����� ���� ����� �����������
LEL* IsInHeap(void *ptr);

void *myHeap;				//�������� �� ������� � ��
LEL *list_beg, *list_end;	//��������� �� ������� � ����� �������� �������
size_t badMemory;

#endif // !MY_HEAP_H