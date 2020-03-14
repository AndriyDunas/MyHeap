#include "my_heap.h"

void* MyMalloc(size_t msize)
{
	LEL *pnew;		//новий елемент служб частини
	int freeMemopy;		//для підрахунку пам'яті, що залишиться після виділення
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
	if(list_end == NULL)	//визначаємо адресу для нового елементу служб частини
	{
		pnew = (LEL*)pfinish - 1;
	}
	else
	{
		pnew = list_end - 1;
	}

	pnew->_size = msize;						//записуємо в елем списку розмір блоку даних корисувача
	pnew->_pdata = (char*)pbegin + GetAlign() + badMemory;		//записуємо цей блок в адресу, наступну за останнім записаним корист. блоком
	AddListElem(pnew);							//додаємо елемент списку в службову частину
	MyMemset(pnew->_pdata, '_', msize);

	return pnew->_pdata;		//повертаємо вказівник на виділений блок
}

void *MyRealloc(void *ptr, size_t newsize)
{
	LEL* pel = IsInHeap(ptr);
	if(pel == NULL || (int)newsize <= 0)					//якщо ні, то функція не виконуєтся
	{
		return NULL;
	}
	if(newsize == pel->_size)	//якщо новий розмір той самий
	{
		return ptr;
	}

	if(newsize < pel->_size)	//якщо новий розмір менший, просто зменшимо розмір ділянки, а вільні байти замаркуємо @
	{
		MyMemset((char*)ptr + newsize, '@', pel->_size - newsize);
		pel->_size = newsize;
		return ptr;
	}
	if(pel->_next == NULL)		//якщо цей користувацький блок останній з записаних
	{
		MyMemset((char*)ptr + pel->_size, '_', newsize - pel->_size);
		pel->_size = newsize;
	}
	else				//новий розмір більший, і користувацький юлок не перший і не єдиний
	{	
		void* oldPtr = ptr;					//інакше збережемо вказівник на стару ділянку корист. даних
		size_t oldSize = pel->_size;	//і розмір старої ділянки

		ptr = MyMalloc(newsize);			
		MyMemmove(oldPtr, ptr, oldSize);	//перемістимо байти з старої ділянки на нову
		MyFree(oldPtr);						
	}
	return ptr;
}

size_t MyFree(void *ptr)	
{
	LEL* pel; 
	int count;			//кількість байтів, звільнених після виконання

	pel = IsInHeap(ptr);		//чи ptr дійсно у нашій купі
	if(pel == NULL)					//якщо ні, то функція не виконуєтся
	{
		return 0;
	}
	count = pel->_size;			//витягуємо кількість байтів, що будуть звільнені
	badMemory += count;			//збільшуємо лічильник звільнених байтів на цю кількість
	DeleteListElem(pel);		//звільн елемент списку службової частини
	MyMemset(ptr, '@', count);	//маркуємо пам'ять як вільну

	return count;
}

int MyDefrag(size_t nparam, size_t nbytes, void **ptr, ...)
{
	LEL* pel = list_beg;
	size_t align;		//величина зміщення при дефрагментації
	void* pbegin = (char*)myHeap;		
	char *phelp, *phelp2;
	void ***indexOfPtr;		//вказівник на деякий параметр ( блок користувача )

	for( indexOfPtr = &ptr; nparam > 0; --nparam, indexOfPtr++) 
	// у циклі проходимо по списку неоголошених параметрів і дефрагентуємо кожен з них на nbytes
	{
		align = 0;
		while (pel != NULL)	
		{
			if(pel->_pdata == **indexOfPtr)		//якщо адреса дійсно знаходиться у моїй купі
			{
				break;
			}
			align += pel->_size;		//рахуємо зміщення
			pel = pel->_next;
		}
		if(pel == NULL || (int)nbytes <= 0)					//якщо ні або кривий розмір, то функція не виконуєтся 
		{
			continue;
		}

		if(pel->_pdata == (char*)pbegin + align + badMemory)		//якщо адреса цього блоку невирівняна
		{
			size_t i;
			phelp = (char*)pel->_pdata;		//зберігаємо адресу початку блоку даних користувача
			pel->_pdata = (char*)pbegin + align;		//вирівнюємо її
			phelp2 = (char*)pel->_pdata;				//і записуємо в допоміжну змінну

			for (i = 0; i < pel->_size; ++i)
			{
				phelp2[i] = phelp[i];		//записуємо байти один за одним з старої адреси в вирівняну
				phelp[i] = '@';				//байти за старою адресою маркуємо як вільні
			}
		}
		else			//інакше нічого вирівнювати не треба, виходимо з циклу 
		{
			break;
		}
		*ptr = ((char*)*ptr - nbytes);			//зміщуємо вказівник на користувацькі дані назад на величину num

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
	for(p = (char*)myHeap; p != (char*)myHeap+HEAP_SIZE; ++p)	//виводимо кожен байт купи
		printf(" %c", *p);	
	printf("\n");
}

void* MyMemset(void *ptr, char value, size_t num)
{
	size_t i;
	char *ptemp = (char*)ptr;
	for(i = 0; i < num; ++i)	//записуємо в кожен байт купи value
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
	//якщо адреса призначення більша за адресу джерела і при цьому адреса джерела, збільшена на вказану кількість байтів num,
	//буде перетинати адресу призначення
		for (i = num - 1; i >= 0; --i)		//будемо копіювати, починаючи з останнього байту блоку-джерела
		{
			dhelp[i] = shelp[i];
		}
	else
		for (i = 0; i < num; ++i)			//інакше будемо копіювати, починаючи з першого байту блоку-джерела
		{
			dhelp[i] = shelp[i];
		}
	return dest;	//повертаємо адресу призначення
}

void* MyMemcpy(void *src, void *dest, size_t num)
{
	size_t i;
	char *shelp = (char*)src;
	char* dhelp = (char*)dest; 
	for (i = 0; i < num; ++i)		//копіюємо, починаючи з першого байту блоку-джерела
	{
		dhelp[i] = shelp[i];
	}
	return dest;
}


void AddListElem(LEL* pnew)
{
	if(list_beg == NULL)		//якщо список був пустий
	{
		pnew->_next = pnew->_prev = NULL;	//ставимо елемент першим
		list_beg = list_end = pnew;	
		return;
	}
	list_end->_next = pnew;		//інакше елемент стає останнім у списку
	pnew->_prev = list_end;
	pnew->_next = NULL;
	list_end = pnew;
}

void DeleteListElem(LEL *pdel)
{
	if(pdel == list_beg)		//якщо видаляємо елемент списку з початку службової частини
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
	if(pdel == list_end)		//якщо видаляємо елемент списку з кінця службової частини
	{
		list_end = list_end->_prev;
		list_end->_next = NULL;
	} 
	else             //з середини
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
	while (pel != NULL)			//проходимо по елементах списку
	{
		align += pel->_size;	//сумуємо розміри користувацьких блоків
		pel = pel->_next;	
	}
	return align;				//повертаємо суму розмірів
}

LEL* IsInHeap(void *ptr)
{
	LEL* pel = list_beg;
	while (pel != NULL)
	{
		if(pel->_pdata == ptr)		//якщо адреса дійсно знаходиться у моїй купі
		{
			return pel;				
		}
		pel = pel->_next;
	}
	return NULL;
}