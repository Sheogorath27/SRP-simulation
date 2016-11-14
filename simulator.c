#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
	int compTimeS;
	int compTimeE;
}resRequired;

typedef struct job1
{
	int id;
	int arrivalTime;
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

int taskCount, clock = 0, idcount[10], resourceCount, runtime, systemCeil = 0;

task *running = NULL, *taskQueue = NULL, *readyQueue = NULL, *finishedQueue = NULL;

job jobs[10];

resource resTable[10];

FILE *fRead, *fWrite;

void updateReadyQueue();
void createTaskQueue();
void addTask(task*, task**);
task* removeTask(task*, task**);
void preempt(task* );
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
	fRead = fopen("/home/avataran/Desktop/RTS/testcase2.txt", "r");
	fWrite = fopen("/home/avataran/Desktop/RTS/output","w+");

	printf("flag 1\n");

	fscanf(fRead, "%d %d %d", &runtime, &taskCount, &resourceCount);
	printf("%d %d %d\n", runtime, taskCount, resourceCount);

	for ( i = 0; i < resourceCount; ++i)
		resTable[i].id = i+1;

	for ( i = 0; i < taskCount; ++i)
	{
		fscanf(fRead, "%d %d %d %d %d", &(jobs[i].id), &(jobs[i].arrivalTime), &(jobs[i].computationTime), &(jobs[i].period), &(jobs[i].resCount) );
		printf( "%d %d %d %d %d\n", (jobs[i].id), (jobs[i].arrivalTime), (jobs[i].computationTime), (jobs[i].period), (jobs[i].resCount) );
		for ( j = 0; j < jobs[i].resCount; ++j)
		{
			fscanf(fRead, "%d %d %d", &(jobs[i].resources[j].id), &(jobs[i].resources[j].compTimeS), &(jobs[i].resources[j].compTimeE) );
			printf("%d %d %d\n", (jobs[i].resources[j].id), (jobs[i].resources[j].compTimeS), (jobs[i].resources[j].compTimeE) );
		}
	}

	printf("flag 2\n");

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
	fclose(fRead);
	fclose(fWrite);
	return 0;
}

void createTaskQueue()
{
	task *temp;
	int i,j;
	printf("flag 3\n");
	for (i = 0; i < taskCount; ++i)
	{
		j = 0;
		while( (jobs[i].arrivalTime + j*jobs[i].period) < runtime)
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
			temp->arrival = jobs[i].arrivalTime + j*jobs[i].period;
			temp->deadline = temp->arrival + jobs[i].period;
			temp->compRemaining = jobs[i].computationTime;
			j++;
		}
	}
	temp->next = NULL;
	printf("flag 4\n");
}


// EDF priority assignment

void assignPriority()
{
	//sort taskqueue based on deadline
	task *temp, *temp1, *temp2, *tempL;
	int flag,i;

	printf("flag 5\n");
	
	while(1)
	{
		//i = 0;
		flag = 1;
		for ( temp = taskQueue; temp->next != NULL; temp = temp->next)
		{
			if(temp->deadline > temp->next->deadline)
			{
				flag = 0;
				if(temp == taskQueue)
				{
					temp1 = temp;
					temp2 = temp->next;
					temp1->next = temp2->next;
					temp2->next = temp1;
					taskQueue = temp2;
				}
				else
				{
					temp1 = temp;
					temp2 = temp->next;
					temp1->next = temp2->next;
					temp2->next = temp1;
					tempL->next = temp2;
				}
				temp = temp2;
			}
			tempL = temp;
			//printf("flag 5%d val %d\n", i, flag);
			//i++;
		}
		if(flag)
			break;
	}

	printf("flag 6\n");

	for ( temp = taskQueue, i = 0; temp != NULL; temp = temp->next, i++)
		temp->priority = i;

	printf("flag 7\n");
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
		fprintf(fWrite, "-------\nArrived Id: %d Time: %d\n--------\n", insert->pjob->id, clock);
		fprintf(stdout, "-------\nArrived Id: %d Time: %d\n--------\n", insert->pjob->id, clock);
		assignPreempLevel();
	}
}

void assignPreempLevel()
{
	task *temp = readyQueue, *temp2, *temp3;

	printf("flag 8\n");
	if(temp->next == NULL)// && running == NULL)
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
	printf("flag 9\n");
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

	printf("flag 10\n");

	if (running == NULL)
	{

		printf("flag 10.1\n");
		
		if(readyQueue == NULL)
			return;

		for (temp = readyQueue; temp != NULL; temp = temp->next)
		{
			//printf("flag 10.12 %d %d\n", min, temp->pjob->id);
			if (temp->priority < min)
			{
				selected = temp;
				min = temp->priority;
			}
		}

		printf("flag 10.2\n");
		running = removeTask(selected, &readyQueue);
		//adding new task
		fprintf(fWrite, "-------\nRunning new task Id: %d Time: %d\n--------\n", running->pjob->id, clock);
		fprintf(stdout, "-------\nRunning new task Id: %d Time: %d\n--------\n", running->pjob->id, clock);
	}
	else
	{
		printf("flag 10.3\n");
		selected = NULL;
		for (temp = readyQueue; temp != NULL; temp = temp->next)
		{
			if (temp->arrival == clock)
			{
				selected = temp;
				min = temp->priority;
			}
		}
		printf("flag 10.4\n");
		if ( selected != NULL && selected->priority < running->priority && selected->preempLevel > systemCeil)
		{
			//removing task
			fprintf(fWrite, "-------\nRemoving task Id: %d Preemption Level: %d Time: %d\n--------\n", running->pjob->id, running->preempLevel, clock);
			fprintf(stdout, "-------\nRemoving task Id: %d Preemption Level: %d Time: %d\n--------\n", running->pjob->id, running->preempLevel, clock);

			preempt(selected);
			running = removeTask(selected, &readyQueue);
			
			//adding new task
			fprintf(fWrite, "-------\nRunning new task Id: %d Preemption Level: %d Time: %d\n--------\n", running->pjob->id, running->preempLevel, clock);
			fprintf(stdout, "-------\nRunning new task Id: %d Preemption Level: %d Time: %d\n--------\n", running->pjob->id, running->preempLevel, clock);
		}
	}
	printf("flag 11\n");
}

void preempt(task *selected)
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

	if(running == NULL)
	{
		sleep(1);

		fprintf(stdout, "--------\nWaiting for new task Time: %d\n--------\n", clock);
		fprintf(fWrite, "--------\nWaiting for new task Time: %d\n--------\n", clock);
		
		clock++;
		return;
	}

	//running task
	fprintf(fWrite, "-------\nExecuting task Id: %d Time: %d\n--------\n", running->pjob->id, clock);
	fprintf(stdout, "-------\nExecuting task Id: %d Time: %d\n--------\n", running->pjob->id, clock);

	while(running->pjob->resCount > i)
	{	
		if( (running->pjob->resources[i].compTimeS + running->arrival) == clock)
		{
			resTable[running->pjob->resources[i].id - 1].hold = running;
			assignResCeil(running->pjob->resources[i].id, 1);
			assignSystemCeil();

			//taking resource
			fprintf(fWrite, "-------\nHolding resource Id: %d Time: %d Resource Id: %d\n--------\n", running->pjob->id, clock, running->pjob->resources[i].id);
			fprintf(stdout, "-------\nHolding resource Id: %d Time: %d Resource Id: %d\n--------\n", running->pjob->id, clock, running->pjob->resources[i].id);
			
			break;
		}
		i++;
	}

	sleep(1);
	--(running->compRemaining);
	clock++;

	i = 0;
	while(running->pjob->resCount > i)
	{	
		if( (running->pjob->resources[i].compTimeE + running->arrival) == clock)
		{
			resTable[running->pjob->resources[i].id].hold = NULL;
			assignResCeil(running->pjob->resources[i].id, 0);
			assignSystemCeil();
		
			//releasing resource
			fprintf(fWrite, "-------\nReleasing resource Id: %d Time: %d Resource Id: %d\n--------\n", running->pjob->id, clock, running->pjob->resources[i].id);
			fprintf(stdout, "-------\nReleasing resource Id: %d Time: %d Resource Id: %d\n--------\n", running->pjob->id, clock, running->pjob->resources[i].id);

			break;
		
		}
		i++;
	}

	if(running->compRemaining == 0)
	{
		//task completed
		fprintf(fWrite, "-------\nCompleted task Id: %d Time: %d\n--------\n", running->pjob->id, clock);
		fprintf(stdout, "-------\nCompleted task Id: %d Time: %d\n--------\n", running->pjob->id, clock);
		
		running->completion = clock;
		addTask(running, &finishedQueue);
		running = NULL;
	}	
}

void assignResCeil(int id, int bool)
{
	task *temp = readyQueue;
	int maxLevel = running->preempLevel, i;
	
	if (bool)
	{
		for ( ; temp != NULL; temp = temp->next)
		{
			i = 0;
			while(temp->pjob->resCount > i)
			{
				if(temp->pjob->resources[i].id == id && maxLevel < temp->preempLevel)
				{	maxLevel = temp->preempLevel;
					break;
				}
				i++;
			}
		}
		resTable[id].ceiling = maxLevel;
	}
	else
	{
		resTable[id].ceiling = 0;
	}

	//resourcce ceil
	fprintf(fWrite, "-------\nResource Ceil for Resource Id: %d Ceil: %d Time: %d\n--------\n", id, resTable[id].ceiling, clock);
	fprintf(stdout, "-------\nResource Ceil for Resource Id: %d Ceil: %d Time: %d\n--------\n", id, resTable[id].ceiling, clock);
}

void assignSystemCeil()
{
	int i;
	for (i = 0; i < resourceCount; ++i)
	{
		if(resTable[i].ceiling > systemCeil)
			systemCeil = resTable[i].ceiling;
	}

	//system ceil11
	fprintf(fWrite, "-------\nSystem Ceil: %d Time: %d\n--------\n", systemCeil, clock);
	fprintf(stdout, "-------\nSystem Ceil: %d Time: %d\n--------\n", systemCeil, clock);
}