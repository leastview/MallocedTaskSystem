#include "MallocedTaskSystem.hpp"

#include <cassert>

namespace igl
{
	MallocedTask::MallocedTask(MallocedTaskList* list)
	{
		Prev = list->ActiveTask->Prev;
		Next = list->ActiveTask;
		Prev->Next = Next->Prev = this;
	}

	MallocedTaskList::MallocedTaskList(size_t size, size_t count) :
		TaskSize(size),
		TaskCount(count)
	{
		Buffer = new char [TaskSize * (TaskCount+2)];
		ActiveTask = GetTask(0);
		ActiveTask->Prev = ActiveTask->Next = ActiveTask;

		FreeTask = GetTask(1);
		for(size_t i = 1; i < TaskCount+2; i++)
		{
			GetTask(i)->Prev = NULL;
			GetTask(i)->Next = GetTask(i+1);
		}
		GetTask(TaskCount+1)->Next = FreeTask;
	}

	void* MallocedTaskList::New(size_t size)
	{
		assert(size <= TaskSize);
		if(FreeTask->Next == FreeTask)
		{
			return NULL;
		}

		MallocedTask* task = FreeTask->Next;
		FreeTask->Next = task->Next;
		return task;
	}

	void MallocedTaskList::erase(void* p)
	{
		MallocedTask* task = reinterpret_cast<MallocedTask*>(p);
		assert(task->Prev != NULL);

		delete task;

		task->Prev = NULL;
		task->Next = FreeTask->Next;
		FreeTask->Next = task;
	}

} // namespace igl
