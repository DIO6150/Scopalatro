#pragma once

#include <TaskManager.hpp>

class TaskQueue
{
public:
	TaskQueue (TaskManager & taskManager);

	void Push (uint64_t ID, TaskID taskID, double ms=0.0);
	void PushCancel (uint64_t ID, TaskID taskID, double ms=0.0);

	void Cancel (uint64_t ID);

private:
	std::map<uint64_t, TaskID> m_tasks;

	TaskManager & m_taskManager;
};
