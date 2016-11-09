#include <stdio.h>
#include <stdlib.h>

#define maxTask 10
#define maxRes 10

typedef struct resource1
{
	int id;
	int ceiling;
	struct task1 *hold;
}resource;

typedef struct resRequired1
{
	int id;
	int compTime;
}resRequired;

typedef struct job1
{
	int id;
	int computationTime;
	int period;
	int resCount;
	struct resRequired1 resources[10];
}job;

typedef struct task1
{
	job *pjob;
	int arrival;
	int deadline;
	int compRemaining;
	int completion;
	int priority;
	int preempLevel;
	struct task1 *next;
}task;

int taskCount, clock = 0, idcount[10], resourceCount, runtime, systemCeil;

task *running = NULL, *taskQueue = NULL, *readyQueue = NULL, *finishedQueue = NULL;

job jobs[10];

resource resTable[10];

FILE *fRead, *fWrite;

void updateReadyQueue();
void createTaskQueue();
void addTask(task*, task**);
task* removeTask(task*, task**);
void preempt();
void runTask();
void assignPriority();
void assignPreempLevel();
void assignResCeil();
void assignSystemCeil();
void updateRunningTask();
task* returnEnd(task *);

int main(int argc, char const *argv[])
{
	int i,j;
	fRead = fopen("/home/avataran/Desktop/RTS/testcase1.txt", "r");
	fWrite = fopen("/home/avataran/Desktop/RTS/output.txt","w");

	fscanf(fRead, "%d %d %d", &runtime, &taskCount, &resourceCount);

	for ( i = 0; i < taskCount; ++i)
	{
		fscanf(fRead, "%d %d %d", &(jobs[i].id), &(jobs[i].computationTime), &(jobs[i].resCount) );
		for ( j = 0; j < resourceCount; ++j)
		{
			fscanf(fRead, "%d %d", &(jobs[i].resources[j].id), &(jobs[i].resources[j].compTime) );
		}
	}

	createTaskQueue();
	assignPriority();
	//assignPreempLevel();
	//assignResCeil();
	//assignSystemCeil();

	while(clock <= runtime)
	{
		updateReadyQueue();
		updateRunningTask();
		runTask();
	}
	return 0;
}

void createTaskQueue()
{
	task *temp;
	int i,j;
	for (i = 0; i < taskCount; ++i)
	{
		j = 0;
		while(j*jobs[i].period < runtime)
		{
			if ( taskQueue != NULL)
			{
				temp->next = (task *) malloc(sizeof(task));
				temp = temp->next;
			}
			else
			{	
				taskQueue = (task *) malloc(sizeof(task));
				temp = taskQueue;
			}
			temp->pjob = &(jobs[i]);
			temp->arrival = j*jobs[i].period;
			temp->deadline = temp->arrival + jobs[i].period;
			temp->compRemaining = jobs[i].computationTime;
			j++;
		}
	}
	temp->next = NULL;
}


// EDF priority assignment

void assignPriority()
{
	//sort taskqueue based on deadline
	task *temp,*temp1;
	int flag,i;
	while(1)
	{
		flag = 1;
		for ( temp = taskQueue; temp->next != NULL; temp = temp->next)
		{
			if(temp->deadline > temp->next->deadline)
			{
				flag = 0;
				temp1 = temp;
				temp = temp->next;
				temp->next = temp1;
			}
		}
		if(flag)
			break;
	}
	for ( temp = taskQueue, i = 0; temp != NULL; temp = temp->next, i++)
		temp->priority = i;
}


//priority of RM

// void assignPriority()
// {
// 	//sort taskqueue based on deadline
// 	task *temp,*temp1;
// 	int flag,i;
// 	while(1)
// 	{
// 		flag = 1;
// 		for ( temp = taskQueue; temp->next != NULL; temp = temp->next)
// 		{
// 			if(temp->deadline > temp->next->deadline)
// 			{
// 				flag = 0;
// 				temp2 = temp;
// 				temp = temp->next;
// 				temp->next = temp2;
// 			}
// 		}
// 		if(flag)
// 			break;
// 	}
// 	for ( temp = taskQueue, i = 0; temp != NULL; temp = temp->next, i++)
// 		temp->priority = temp->pjob->period;
// }

void runTask()
{

}

task* returnEnd(task* list)
{
	for( ; list->next != NULL; list = list->next);
	return list;
}

task* removeTask(task* location, task** queue)
{
	task* temp = *queue;
	if (location == *queue )
	{
				*queue = location->next;
				return location;
	}
	while( temp->next != location)
		temp = temp->next;
	temp->next = location->next;
	return location;
}

void addTask( task* insert, task** queue)
{
	task *temp = *queue;

	*queue = insert;
	insert->next = temp;

	if (*queue == readyQueue)
	{
		assignPreempLevel();
	}
}

void assignPreempLevel()
{
	task *temp = readyQueue, *temp2, *temp3;
	if(temp->next == NULL)
		temp->preempLevel = 100;
	else
	{	temp2 = temp->next;
		while(temp->deadline < temp2->deadline)
			{
				temp2->preempLevel = temp2->preempLevel - 1;
				if(temp2->next == NULL)
				{
					readyQueue = readyQueue->next;
					temp2->next == temp;
					temp->preempLevel = 100;
					return;
				}
				temp3 = temp2;
				temp2 = temp2->next;
			}

		readyQueue = readyQueue->next;
		temp3->next = temp;
		temp->next = temp2;
		temp->preempLevel = temp3->preempLevel + 1;
	}
}

void updateReadyQueue()
{
	task *temp, *insert, *temp2;
	//insert = returnEnd(readyQueue);
	for ( temp = taskQueue; temp != NULL; temp = temp->next)
	{
		if (temp->arrival == clock)
		{
			addTask(removeTask(temp, &taskQueue), &readyQueue);
			//insert = insert->next;
		}
	}
}

void updateRunningTask()
{
	task *temp, *selected;
	int min = 100;
	if (running == NULL)
	{
		for (temp = readyQueue; temp != NULL; temp->next)
		{
			if (temp->priority < min)
			{
				selected = temp;
				min = temp->priority;
			}
		}
		running = removeTask(selected, &readyQueue);
	}
	else
	{
		for (temp = readyQueue; temp != NULL; temp->next)
		{
			if (temp->arrival == clock)
			{
				selected = temp;
				min = temp->priority;
			}
		}
		if (selected->priority < running->priority && selected->preempLevel > systemCeil)
		{
			preempt();
			running = removeTask(selected, &readyQueue);
		}
	}
}

void preempt()
{
	task *temp = readyQueue, *temp2;
	while(running->deadline < temp->deadline)
	{
		if(temp->next == NULL)
		{
			temp2->next == temp;
			return;
		}
		temp2 = temp;
		temp = temp->next;
	}	
	if(temp == readyQueue)
	{
		running->next = readyQueue;
		readyQueue = running;
	}
	else
	{
		temp2->next = running;
		running->next = temp;
	}
}

void runTask()
{	
	int i = 0;
	while(running->pjob->resCount > i)
	{	
		if( (running->pjob->resources[i].compTime + running.arrival) == clock)
		{
			assignResCeil(running->pjob->resources[i].id, 1);
			assignSystemCeil();
			break;
		}
		i++;
	}

	i = 0;
	while(running->pjob->resCount > i)
	{	
		if( (running->pjob->resources[i].compTime + running.arrival) == clock + 1)
		{
			assignResCeil(running->pjob->resources[i].id, 0);
			assignSystemCeil();
			break;
		}
		i++;
	}
	--(running->compRemaining);

	if(running->compRemaining == 0)
	{
		running->completion = clock;
		addTask(running, &finishedQueue);
	}	
}