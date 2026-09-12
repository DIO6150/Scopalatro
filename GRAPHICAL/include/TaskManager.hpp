#pragma once

// TODO: add this to engine

#include <stdint.h>

#include <functional>
#include <map>
#include <set>
#include <vector>
#include <deque>

using TaskID = uint64_t;
inline static TaskID const SentinelTask = UINT64_MAX;

enum class TaskResult
{
	Yield,
	Error,
	Return
};

using AsyncTask = std::function<TaskResult(TaskID ID, double progress, double deltaTime)>;

class TaskManager
{
public:
	TaskID RegisterTask(AsyncTask && callback);

	void StartTask(TaskID taskID, double ms=0.0);
	void StartTaskAfter(TaskID taskID, TaskID parentID, double ms=0.0);
	void StartTaskOnDeath(TaskID taskID, TaskID parentID, double ms=0.0);

	void StopTask(TaskID taskID);
	void PauseTask(TaskID taskID);
	void ResumeTask(TaskID taskID);

	bool IsTaskAlive(TaskID taskID);
	bool IsTaskInfant(TaskID taskID);

	void Update(double deltaTime);

private:
	struct TaskWrapper
	{
		AsyncTask callback;
		double countdown = -1.0;

		std::vector<TaskID> after;
		std::vector<TaskID> death;
	};

	struct TaskExecutorNode
	{
		TaskID currentTask = SentinelTask;

		double progress = 0.0;
		double countdown = 0.0;
		bool paused = false;
		bool dead = false;
	};

	std::map<TaskID, TaskWrapper> m_tasks;
	std::multimap<TaskID, TaskID> m_taskToParent;
	TaskResult Evaluate(TaskID taskID, TaskWrapper & taskWrapper, double deltaTime);

	std::vector<TaskExecutorNode> m_executionList;
	std::map<TaskID, size_t> m_taskToIndex;

	TaskID GenerateID();
	TaskID m_nextID = 0;
};
