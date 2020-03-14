#include "my_heap.h"

void* MyMalloc(size_t msize)
{
	LEL *pnew;		//����� ������� ����� �������
	int freeMemopy;		//��� ��������� ���'��, �� ���������� ���� ��������
	void* pbegin;
	void* pfinish;

	if( (int)msize <= 0 || msize > HEAP_SIZE )
	{
		return NULL;
	}
	freeMemopy = GetFreeMemory() - sizeof(LEL) - msize;
	if(freeMemopy < 0)
	{
		return NULL;
	}

	pbegin = (char*)myHeap;				
	pfinish = (char*)myHeap + HEAP_SIZE;
	if(list_end == NULL)	//��������� ������ ��� ������ �������� ����� �������
	{
		pnew = (LEL*)pfinish - 1;
	}
	else
	{
		pnew = list_end - 1;
	}

	pnew->_size = msize;						//�������� � ���� ������ ����� ����� ����� ����������
	pnew->_pdata = (char*)pbegin + GetAlign() + badMemory;		//�������� ��� ���� � ������, �������� �� ������� ��������� ������. ������
	AddListElem(pnew);							//������ ������� ������ � �������� �������
	MyMemset(pnew->_pdata, '_', msize);

	return pnew->_pdata;		//��������� �������� �� �������� ����
}

void *MyRealloc(void *ptr, size_t newsize)
{
	LEL* pel = IsInHeap(ptr);
	if(pel == NULL || (int)newsize <= 0)					//���� �, �� ������� �� ���������
	{
		return NULL;
	}
	if(newsize == pel->_size)	//���� ����� ����� ��� �����
	{
		return ptr;
	}

	if(newsize < pel->_size)	//���� ����� ����� ������, ������ �������� ����� ������, � ���� ����� ��������� @
	{
		MyMemset((char*)ptr + newsize, '@', pel->_size - newsize);
		pel->_size = newsize;
		return ptr;
	}
	if(pel->_next == NULL)		//���� ��� �������������� ���� ������� � ���������
	{
		MyMemset((char*)ptr + pel->_size, '_', newsize - pel->_size);
		pel->_size = newsize;
	}
	else				//����� ����� ������, � �������������� ���� �� ������ � �� ������
	{	
		void* oldPtr = ptr;					//������ ��������� �������� �� ����� ������ ������. �����
		size_t oldSize = pel->_size;	//� ����� ����� ������

		ptr = MyMalloc(newsize);			
		MyMemmove(oldPtr, ptr, oldSize);	//���������� ����� � ����� ������ �� ����
		MyFree(oldPtr);						
	}
	return ptr;
}

size_t MyFree(void *ptr)	
{
	LEL* pel; 
	int count;			//������� �����, ��������� ���� ���������

	pel = IsInHeap(ptr);		//�� ptr ����� � ����� ���
	if(pel == NULL)					//���� �, �� ������� �� ���������
	{
		return 0;
	}
	count = pel->_size;			//�������� ������� �����, �� ������ �������
	badMemory += count;			//�������� �������� ��������� ����� �� �� �������
	DeleteListElem(pel);		//����� ������� ������ �������� �������
	MyMemset(ptr, '@', count);	//������� ���'��� �� �����

	return count;
}

int MyDefrag(size_t nparam, size_t nbytes, void **ptr, ...)
{
	LEL* pel = list_beg;
	size_t align;		//�������� ������� ��� ��������������
	void* pbegin = (char*)myHeap;		
	char *phelp, *phelp2;
	void ***indexOfPtr;		//�������� �� ������ �������� ( ���� ����������� )

	for( indexOfPtr = &ptr; nparam > 0; --nparam, indexOfPtr++) 
	// � ���� ��������� �� ������ ������������ ��������� � ������������ ����� � ��� �� nbytes
	{
		align = 0;
		while (pel != NULL)	
		{
			if(pel->_pdata == **indexOfPtr)		//���� ������ ����� ����������� � ��� ���
			{
				break;
			}
			align += pel->_size;		//������ �������
			pel = pel->_next;
		}
		if(pel == NULL || (int)nbytes <= 0)					//���� � ��� ������ �����, �� ������� �� ��������� 
		{
			continue;
		}

		if(pel->_pdata == (char*)pbegin + align + badMemory)		//���� ������ ����� ����� ����������
		{
			size_t i;
			phelp = (char*)pel->_pdata;		//�������� ������ ������� ����� ����� �����������
			pel->_pdata = (char*)pbegin + align;		//��������� ��
			phelp2 = (char*)pel->_pdata;				//� �������� � �������� �����

			for (i = 0; i < pel->_size; ++i)
			{
				phelp2[i] = phelp[i];		//�������� ����� ���� �� ����� � ����� ������ � ��������
				phelp[i] = '@';				//����� �� ������ ������� ������� �� ����
			}
		}
		else			//������ ����� ���������� �� �����, �������� � ����� 
		{
			break;
		}
		*ptr = ((char*)*ptr - nbytes);			//������ �������� �� ������������ ��� ����� �� �������� num

		/*MyMemmap();
		PrintList();
		getchar();*/
	}
	badMemory -= nbytes;

	return 1;
}


void MyMemmap(void)
{
	char *p;
	printf("\n MyMemmap():: Map of myHeap:\n");
	for(p = (char*)myHeap; p != (char*)myHeap+HEAP_SIZE; ++p)	//�������� ����� ���� ����
		printf(" %c", *p);	
	printf("\n");
}

void* MyMemset(void *ptr, char value, size_t num)
{
	size_t i;
	char *ptemp = (char*)ptr;
	for(i = 0; i < num; ++i)	//�������� � ����� ���� ���� value
	{
		*(ptemp+i) = value;
	}
	return ptr;
}

void* MyMemmove(void *src, void *dest, size_t num)
{
	size_t i;
	char *shelp = (char*)src;
	char* dhelp = (char*)dest; 

	if( (dhelp > shelp) && (shelp + (num-1) >= dhelp) )
	//���� ������ ����������� ����� �� ������ ������� � ��� ����� ������ �������, �������� �� ������� ������� ����� num,
	//���� ���������� ������ �����������
		for (i = num - 1; i >= 0; --i)		//������ ��������, ��������� � ���������� ����� �����-�������
		{
			dhelp[i] = shelp[i];
		}
	else
		for (i = 0; i < num; ++i)			//������ ������ ��������, ��������� � ������� ����� �����-�������
		{
			dhelp[i] = shelp[i];
		}
	return dest;	//��������� ������ �����������
}

void* MyMemcpy(void *src, void *dest, size_t num)
{
	size_t i;
	char *shelp = (char*)src;
	char* dhelp = (char*)dest; 
	for (i = 0; i < num; ++i)		//�������, ��������� � ������� ����� �����-�������
	{
		dhelp[i] = shelp[i];
	}
	return dest;
}


void AddListElem(LEL* pnew)
{
	if(list_beg == NULL)		//���� ������ ��� ������
	{
		pnew->_next = pnew->_prev = NULL;	//������� ������� ������
		list_beg = list_end = pnew;	
		return;
	}
	list_end->_next = pnew;		//������ ������� ��� ������� � ������
	pnew->_prev = list_end;
	pnew->_next = NULL;
	list_end = pnew;
}

void DeleteListElem(LEL *pdel)
{
	if(pdel == list_beg)		//���� ��������� ������� ������ � ������� �������� �������
	{
		LEL*pnext = list_beg;
		if(list_beg == list_end)
		{
			MyMemset((char*)list_end, '@', sizeof(LEL));
			list_beg = list_end = NULL;
			return;
		}

		do
		{
			pnext->_pdata = pnext->_next->_pdata;
			pnext->_size = pnext->_next->_size;
			pnext = pnext->_next;
		} while (pnext != list_end);
		pnext = pnext->_prev;
		pnext->_next = NULL;
		list_end = pnext;

		MyMemset((char*)list_end-sizeof(LEL), '@', sizeof(LEL));
		return;
	}
	if(pdel == list_end)		//���� ��������� ������� ������ � ���� �������� �������
	{
		list_end = list_end->_prev;
		list_end->_next = NULL;
	} 
	else             //� ��������
	{
		LEL* pnext = pdel;
		do
		{
			pnext->_pdata = pnext->_next->_pdata;
			pnext->_size = pnext->_next->_size;
			pnext = pnext->_next;
		} while (pnext != list_end);
		pnext = pnext->_prev;
		pnext->_next = NULL;
		list_end = pnext;
	}
	MyMemset((char*)list_end-sizeof(LEL), '@', sizeof(LEL));
}

void PrintList(void)
{
	LEL* pel = list_beg;
	int i = 0;
	
	printf("\n PrintList():: Memory control list:\n");
	while (pel != NULL)
	{
		printf(" element %d:         %-10dadress: %d\n", i, pel, pel);
		printf(" element %d->_prev:  %-10dadress: %d\n", i, pel->_prev, &pel->_prev);
		printf(" element %d->_next:  %-10dadress: %d\n", i, pel->_next, &pel->_next);
		printf(" element %d->_pdata: %-10dadress: %d\n", i, pel->_pdata, &pel->_pdata);
		printf(" element %d->_size:  %-10dadress: %d\n\n", i, pel->_size, &pel->_size);
		pel = pel->_next;	
		i++;
	}
}


int GetFreeMemory(void)
{
	if( (list_end == list_beg) && (list_beg == NULL) )
	{
		return HEAP_SIZE;
	}
	else
	{	
		return ( (char*)list_end - (char*)list_end->_pdata - list_end->_size );
	}
}

size_t GetAlign(void)
{
	LEL* pel = list_beg;	
	size_t align = 0;
	while (pel != NULL)			//��������� �� ��������� ������
	{
		align += pel->_size;	//������ ������ �������������� �����
		pel = pel->_next;	
	}
	return align;				//��������� ���� ������
}

LEL* IsInHeap(void *ptr)
{
	LEL* pel = list_beg;
	while (pel != NULL)
	{
		if(pel->_pdata == ptr)		//���� ������ ����� ����������� � ��� ���
		{
			return pel;				
		}
		pel = pel->_next;
	}
	return NULL;
}