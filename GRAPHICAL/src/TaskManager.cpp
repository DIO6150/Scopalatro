#include <TaskManager.hpp>

#include <Core/Logger.hpp>

using namespace TLOT;

TaskID TaskManager::RegisterTask(AsyncTask && callback)
{
	TaskID taskID = GenerateID();

	TaskWrapper wrapper;
	wrapper.callback = std::move(callback);
	m_tasks.emplace(taskID, std::move(wrapper));

	return taskID;
}

void TaskManager::StartTask(TaskID taskID, double ms)
{
	TaskExecutorNode node;
	node.currentTask = taskID;
	node.countdown   = ms;

	m_taskToIndex[taskID] = m_executionList.size();
	m_executionList.push_back(node);
}

void TaskManager::StartTaskAfter(TaskID taskID, TaskID parentID, double ms)
{
	if(m_taskToIndex.count(parentID) == 0) return;
	else if(m_tasks.count(taskID) == 0) return;

	TaskWrapper & parentWrapper = m_tasks[parentID];
	parentWrapper.after.push_back(taskID);

	TaskWrapper & currentWrapper = m_tasks[taskID];
	currentWrapper.countdown = ms;
}

void TaskManager::StartTaskOnDeath(TaskID taskID, TaskID parentID, double ms)
{
	if(m_taskToIndex.count(parentID) == 0) return;
	else if(m_tasks.count(taskID) == 0) return;

	TaskWrapper & parentWrapper = m_tasks[parentID];
	parentWrapper.death.push_back(taskID);

	TaskWrapper & currentWrapper = m_tasks[taskID];
	currentWrapper.countdown = ms;
}

void TaskManager::StopTask(TaskID taskID)
{
	if(m_taskToIndex.count(taskID) < 1) return;

	size_t index = m_taskToIndex[taskID];
	m_executionList[index].dead = true;
}

void TaskManager::PauseTask(TaskID taskID)
{
	if(m_taskToIndex.count(taskID) < 1) return;

	size_t index = m_taskToIndex[taskID];
	m_executionList[index].paused = true;
}

void TaskManager::ResumeTask(TaskID taskID)
{
	if(m_taskToIndex.count(taskID) < 1) return;

	size_t index = m_taskToIndex[taskID];
	m_executionList[index].paused = false;
}

bool TaskManager::IsTaskAlive(TaskID taskID)
{
	if(m_taskToIndex.count(taskID) < 1) return false;
	
	size_t index = m_taskToIndex[taskID];
	return !m_executionList[index].dead;
}

bool TaskManager::IsTaskInfant(TaskID taskID)
{
	if (m_tasks.contains(taskID) && !m_taskToIndex.contains(taskID)) return true;
	return false;
}

void TaskManager::Update(double deltaTime)
{
	std::vector<size_t> markedForDeletion;

	size_t deadTasks = 0;

	for(size_t index = 0; index < m_executionList.size(); ++index)
	{
		TaskExecutorNode const node = m_executionList[index];

		if(node.paused || node.dead)
		{
			if(node.dead)
			{
				markedForDeletion.push_back(index);
				deadTasks++;
			}

			continue;
		}

		m_executionList[index].countdown -= deltaTime;

		if(node.countdown > 0.0) continue;

		TaskWrapper & currentWrapper = m_tasks[node.currentTask];
		auto result = currentWrapper.callback(node.currentTask, node.progress, deltaTime);

		m_executionList[index].progress += deltaTime;

		if(result == TaskResult::Error)
		{
			Logger::log(LogLevel::Error, "Task(ID={}) failed.", node.currentTask);
			m_executionList[index].dead = true;
			markedForDeletion.push_back(index);
			deadTasks++;
		}
		else if(result == TaskResult::Return)
		{
			for(auto & childTaskID : currentWrapper.after)
			{
				TaskWrapper & childWrapper = m_tasks[childTaskID];
				StartTask(childTaskID, childWrapper.countdown);
			}

			m_executionList[index].dead = true;
			markedForDeletion.push_back(index);
			deadTasks++;
		}
	}

	// should not retrigger the indexing every frames but only when there is too much dead tasks
	if(deadTasks > m_executionList.size() / 2.0 && m_executionList.size() > 10)
	{
		// markedForDeletion should be ordered
		for(size_t index = markedForDeletion.size() - 1; index > 0; --index)
		{
			size_t indexToDelete = markedForDeletion[index];
			auto node = m_executionList[indexToDelete];
			m_executionList.erase(m_executionList.begin() + indexToDelete);

			m_tasks.erase(node.currentTask);

		}

		// update indexes
		m_taskToIndex.clear();
		size_t index = 0;
		for(auto & taskNode : m_executionList)
		{
			m_taskToIndex[taskNode.currentTask] = index++;
		}
	}
}

TaskID TaskManager::GenerateID()
{
	return m_nextID++;
}