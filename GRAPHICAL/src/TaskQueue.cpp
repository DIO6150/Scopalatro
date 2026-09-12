#include <TaskQueue.hpp>

#include <core/Logger.hpp>

using namespace TLOT;

TaskQueue::TaskQueue (TaskManager & taskManager):
	m_taskManager {taskManager}
{

}

void TaskQueue::Push (uint64_t ID, TaskID task, double ms)
{
	if (m_tasks.count (ID) > 0 && m_taskManager.IsTaskAlive (m_tasks[ID]))
	{
		m_taskManager.StartTaskAfter (task, m_tasks[ID], ms);
	}
	else
	{
		m_taskManager.StartTask (task, ms);
	}

	m_tasks[ID] = task;
}

void TaskQueue::PushCancel (uint64_t ID, TaskID task, double ms)
{
	if (m_tasks.count (ID) > 0)
	{
		m_taskManager.StopTask (m_tasks[ID]);
	}
	m_taskManager.StartTask (task, ms);
	
	m_tasks[ID] = task;
}

void TaskQueue::Cancel (uint64_t ID)
{
	if (m_tasks.count (ID) > 0)
	{
		m_taskManager.StopTask (m_tasks[ID]);
		m_tasks.erase(ID);
	}

}
