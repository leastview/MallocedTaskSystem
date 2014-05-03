#include "MallocedTaskSystem.hpp"

class MyClass : public igl::MallocedTask
{
public:
	int value;

	MyClass(igl::MallocedTaskList* list) : MallocedTask(list) {}
	MyClass(igl::MallocedTaskList* list, int n) :
		MallocedTask(list),
		value(n)
	{}

	virtual ~MyClass()
	{
		printf("Release: (%d):%p\n", value, this);
	}

	MyClass& operator=(int i)
	{
		value = i;
		return *this;
	}

	operator int()
	{
		return value;
	}
};

class MySubClass : public MyClass
{
public:
	int value[100];

	MySubClass(igl::MallocedTaskList* list) : MyClass(list) {}
	MySubClass(igl::MallocedTaskList* list, int n) :
		MyClass(list, n)
	{}

	~MySubClass()
	{
		printf("(Sub)Release\n");
	}

};

int main()
{
	igl::MallocedTaskList list(sizeof(MySubClass), 10);

	MyClass* p;
	for(int i = 1; i <= 20; i++)
	{
		p = new(&list) MySubClass(&list, i);
		printf("%d: %p\n", i, p);
	}

	igl::MallocedTaskList::iterator iter;

	puts("-----");

	for(iter = list.begin(); iter != list.end(); iter++)
	{
		p = reinterpret_cast<MyClass*>(*iter);
		printf("List[%d]: %d\n", iter.index(), p->value);
	}

	puts("-----");

	for(iter = list.begin(); iter != list.end(); iter++)
	{
		p = reinterpret_cast<MyClass*>(*iter);
		if((p->value / 2) % 2 == 0)
		{
			list.erase(iter--);
		}
	}

	for(iter = list.begin(); iter != list.end(); iter++)
	{
		p = reinterpret_cast<MyClass*>(*iter);
		printf("List[%d]: %d\n", iter.index(), p->value);
	}

	puts("-----");

	for(int i = 1; i <= 10; i++)
	{
		p = new(&list) MySubClass(&list, i);
		printf("%d: %p\n", i, p);
	}

	for(iter = list.begin(); iter != list.end(); iter++)
	{
		p = reinterpret_cast<MyClass*>(*iter);
		printf("List[%d]: %d\n", iter.index(), p->value);
	}

	return 0;
}
