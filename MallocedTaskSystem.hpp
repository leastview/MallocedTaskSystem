#pragma once

#include <iterator>

typedef unsigned int size_t;

namespace igl
{
	class MallocedTask;
	class MallocedTaskList;
	class MallocedTaskListIterator;

	class MallocedTask
	{
		friend class MallocedTaskList;
		friend class MallocedTaskListIterator;

	private:
		MallocedTask* Prev;
		MallocedTask* Next;

		void* operator new(size_t n) {}
		void operator delete(void* p) {}

	public:
		MallocedTask(MallocedTaskList* list);
		virtual ~MallocedTask();

		void* operator new(size_t n, MallocedTaskList* list);
		void operator delete(void* p, MallocedTaskList* list);
	};

	class MallocedTaskList
	{
		friend class MallocedTask;
		friend class MallocedTaskListIterator;

	private:
		char* Buffer;
		MallocedTask* ActiveTask;
		MallocedTask* FreeTask;
		const size_t TaskSize;
		const size_t TaskCount;

		MallocedTask* GetTask(size_t idx);

	public:
		MallocedTaskList(size_t size, size_t count);
		virtual ~MallocedTaskList();

		typedef MallocedTaskListIterator iterator;
		MallocedTaskList::iterator begin();
		MallocedTaskList::iterator end();
		void erase(void* p);
		void erase(MallocedTaskList::iterator iter);

		void* New(size_t size);
	};

	class MallocedTaskListIterator : public std::iterator<std::bidirectional_iterator_tag, MallocedTask>
	{
		friend class MallocedTaskList;

	private:
		size_t idx;
		MallocedTask* ptr;
		
		MallocedTaskListIterator(MallocedTask* task, size_t index);

	public:
		MallocedTaskListIterator();
		MallocedTaskListIterator(const MallocedTaskListIterator& iter);

		MallocedTaskListIterator& operator++();
		MallocedTaskListIterator operator++(int);
		MallocedTaskListIterator& operator--();
		MallocedTaskListIterator operator--(int);
 
		MallocedTask*& operator*();
 
		bool operator==(const MallocedTaskListIterator& iterator);
		bool operator!=(const MallocedTaskListIterator& iterator);

		size_t index();
	};

#pragma region inline functions

#pragma region MallocedTask

	inline MallocedTask::~MallocedTask()
	{
		Prev->Next = Next;
		Next->Prev = Prev;
	}

	inline void* MallocedTask::operator new(size_t n, MallocedTaskList* list)
	{
		return list->New(n);
	}

	inline void MallocedTask::operator delete(void* p, MallocedTaskList* list)
	{
		return list->erase(p);
	}

#pragma endregion
	
#pragma region MallocedTaskList

	inline MallocedTask* MallocedTaskList::GetTask(size_t idx)
	{
		return reinterpret_cast<MallocedTask*>(Buffer + TaskSize * idx);
	}

	inline MallocedTaskList::~MallocedTaskList()
	{
		delete [] Buffer;
	}

	inline MallocedTaskList::iterator MallocedTaskList::begin()
	{
		return MallocedTaskListIterator(ActiveTask->Next, 0);
	}

	inline MallocedTaskList::iterator MallocedTaskList::end()
	{
		return MallocedTaskListIterator(ActiveTask, SIZE_MAX);
	}

	inline void MallocedTaskList::erase(MallocedTaskList::iterator iter)
	{
		erase(iter.ptr);
	}

#pragma endregion
	
#pragma region MallocedTaskListIterator

	inline MallocedTaskListIterator::MallocedTaskListIterator() :
		idx(SIZE_MAX),
		ptr(NULL)
	{}

	inline MallocedTaskListIterator::MallocedTaskListIterator(MallocedTask* task, size_t index) :
		idx(index),
		ptr(task)
	{
		if(idx < 0)
		{
			idx = SIZE_MAX;
		}
	}

	inline MallocedTaskListIterator::MallocedTaskListIterator(const MallocedTaskListIterator& iter) :
		idx(iter.idx),
		ptr(iter.ptr)
	{}

	inline MallocedTaskListIterator& MallocedTaskListIterator::operator++()
	{
		idx++;
		ptr = ptr->Next;
		return *this;
	}

	inline MallocedTaskListIterator MallocedTaskListIterator::operator++(int)
	{
		MallocedTaskListIterator result = *this;
		idx++;
		ptr = ptr->Next;
		return result;
	}

	inline MallocedTaskListIterator& MallocedTaskListIterator::operator--()
	{
		idx--;
		ptr = ptr->Prev;
		return *this;
	}

	inline MallocedTaskListIterator MallocedTaskListIterator::operator--(int)
	{
		MallocedTaskListIterator result = *this;
		idx--;
		ptr = ptr->Prev;
		return result;
	}

	inline MallocedTask*& MallocedTaskListIterator::operator*()
	{
		static MallocedTask* dummy;
		return (idx == SIZE_MAX ? dummy : ptr);
	}

	inline bool MallocedTaskListIterator::operator!=(const MallocedTaskListIterator& iterator)
	{
		return this->ptr != iterator.ptr;
	}

	inline bool MallocedTaskListIterator::operator==(const MallocedTaskListIterator& iterator)
	{
		return !(*this != iterator);
	}

	inline size_t MallocedTaskListIterator::index()
	{
		return idx;
	}

#pragma endregion

#pragma endregion


} // namespace igl
