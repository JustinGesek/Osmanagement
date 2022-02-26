#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>

//macros
#define EXIT  0
#define INVALID  -1
#define MEMORY  1
#define FILES  2
#define THREAD  3
#define FIRST  0
#define BEST  1
#define WORST  2
#define NEXT  3
void nextFit(int* blockSize, int blocks, int* processSize, int processes);
void firstFit(int* blockSize, int blocks, int* processSize, int processes);
void bestFit(int* blockSize, int blocks, int* processSize, int processes);
void worstFit(int* blockSize, int blocks, int* processSize, int processes);
void* threadFunction(void* vargp);


time_t startedAt;

void printAttributes(char const *name, struct stat const *statBuff);
void fileAttributes();

void multiThreads()
{
	 int const SIZE = 7;
	 int i;
	 int error;
	 pthread_t tid[SIZE];
	 for (i = 0; i < SIZE; i++)
	 {
		 error = pthread_create(&tid[i], NULL, threadFunction, (void*)&tid[i]);
		 if (error != 0)
		 {
			 printf("\nThread can't be created : [%s]. Press `Enter' to continue . ..", strerror(error));
			 getchar();
		 }
	 }
	 for (i = 0; i < SIZE; i++)
	 {
		 pthread_join(tid[i], NULL);
	 }
}

void *threadFunction(void *vargp)
{
	int const LOOP = 10;
	int i;
	int* myid = (int*)vargp;
	//Thread ID 1130170112 started
	//----------------------------
	printf("-----------------------------\nThread ID %d started\n-----------------------------\n\n\n", *myid);
	
	for (i = 0; i < LOOP; i++)
	{
		printf("Thread ID %d printing\n", *myid);
	}
	printf("----------------------------\nThread ID %d ended\n----------------------------\n\n\n", *myid);
	return NULL;
}

void fileAttributes()
{
	struct stat statBuff;
	int err;
	struct dirent *de;
	DIR* dr = opendir(".");

	if (dr == NULL)
	{
		fprintf(stderr, "Could not open current directory!\n");
		exit(1);
	}
	while ((de = readdir(dr)) != NULL)
	{
		err = stat(de->d_name, &statBuff);
		if (err == -1)
		{
			fprintf(stderr, "Error in stat;   Press 'Enter' to continue...");
			getchar();
		}
		else
		{
			printAttributes(de->d_name, &statBuff);
		}
	}
}


void printAttributes(char const* name, struct stat const* statBuff)
{
	static char const* modes[8] = {
		"none", "execute", "write", "write execute", "read", "read execute", "read write", "read write execute"
	};
	time_t t;
	char timeStr[100];
	fprintf(stdout, "\n-------------------- File = %s --------------------\n\n", name);
	fprintf(stdout, "Device id = %lu\n", statBuff->st_dev);
	fprintf(stdout, "File serial number = %lu\n", statBuff->st_ino);
	fprintf(stdout, "File user id = %u\n", statBuff->st_uid);
	fprintf(stdout, "File group id = %u\n", statBuff->st_gid);
	fprintf(stdout, "File mode = %u\n", statBuff->st_mode);
	fprintf(stdout, "Owner premissions: %s\n", modes[(statBuff->st_mode >> 6) & 7]);
	fprintf(stdout, "Group permissions: %s\n", modes[(statBuff->st_mode >> 3) & 7]);
	fprintf(stdout, "Others permissions: %s\n", modes[(statBuff->st_mode >> 0) & 7]);
	strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime(&statBuff->st_ctime));
	fprintf(stdout, "Date created = %s\n", timeStr);
	strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime(&statBuff->st_mtime));
	fprintf(stdout, "Date modified = %s\n", timeStr);
	strftime(timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime(&statBuff->st_atime));
	fprintf(stdout, "Time file was last accessed = %s\n", timeStr);
	fprintf(stdout, "File size = %ld\n", statBuff->st_size);
}



void ourExit(int returnCode)
{
	time_t endedAt = time(NULL);
	int duration = endedAt - startedAt;
	printf("startedAt = %ld; endedAt = %ld\n", startedAt, endedAt);
	printf("Procces returned %d (0x%x)\texecution time : %.3lf s\n", returnCode, returnCode, ((double)duration) / /*CLOCKS_PER_SEC*/ 1);
	printf("Press any key to continue.\n");
	char c;
	scanf("%c", &c);
	exit(returnCode);
}

int displayMenu()
{
	int choice = -1; // stores the users menu selection
	while (choice == -1)
	{
		printf("1: memoryManagement\n");
		printf("2: fileManagement\n");
		printf("3: multiThreads\n");
		printf("0: exit\n");
		if (scanf(" %d", &choice) == 1 && 0 <= choice && choice < 4)
		{
			break;
		}
		printf("Please enter a value between 0 and 3\n");
	}
	return choice;
}

void clearScreen()
{
	char c;
	printf("Please hit the enter key to continue.");
	scanf("%c", &c);
	printf("\n");
	system("clear");
}

void memoryManagement()
{
	clearScreen();
	printf("       ********** Memory Management **********\n");
	for (int algorithm = 0; algorithm < 4; algorithm++)
	{
		int blockSize[] = { 15, 10, 20, 35, 80 };
		int processSize[] = { 10, 20, 5, 30, 65 };
		int blocks = sizeof(blockSize) / sizeof(blockSize[0]);
		int processes = sizeof(processSize) / sizeof(processSize[0]);

		switch (algorithm)
		{
		case FIRST:
			firstFit(blockSize, blocks, processSize, processes);
			break;
		case BEST:
			bestFit(blockSize, blocks, processSize, processes);
			break;
		case WORST:
			worstFit(blockSize, blocks, processSize, processes);
			break;
		case NEXT:
			nextFit(blockSize, blocks, processSize, processes);
			break;
		default:
			fprintf(stderr, "memoryManagement(): unrecognized algorithm = %d\n", algorithm);
			ourExit(1);
		}
	}
}

void displayProcess(int* allocation, int processes, int* processSize)
{
	printf("Process No.     Process Size    Block No.\n");
	for (int p = 0; p < processes; p++)
	{
		if (allocation[p] == -1)
		{
			printf("%2d               %2d             Not Allocated\n", p + 1, processSize[p]);

		}
		else
		{
			printf("%2d               %2d             %d\n", p + 1, processSize[p], allocation[p] + 1);
		}
	}
	printf("\n");
}

void nextFit(int *blockSize, int blocks, int *processSize, int processes)
{
	printf("       ********** Next Fit **********\n");

	int* allocation = (int*)malloc(sizeof(int) * processes);
	int id = 0;
	memset(allocation, INVALID, sizeof(int) * processes);
	for (int p = 0; p < processes; ++p)
	{
		while (id < blocks)
		{
			if (blockSize[id] >= processSize[p])
			{
				allocation[p] = id;
				blockSize[id] -= processSize[p];
				break;
			}
			id = (id + 1) % blocks;
		}
	}
	displayProcess(allocation, processes, processSize);
}

void firstFit(int* blockSize, int blocks, int* processSize, int processes)
{
	printf("       ********** First Fit **********\n");

	int* allocation = (int*)malloc(sizeof(int) * processes);
	int id = 0;
	memset(allocation, INVALID, sizeof(int) * processes);
	for (int p = 0; p < processes; ++p)
	{
		for(id = 0; id < blocks; id++)
		{
			if (blockSize[id] >= processSize[p])
			{
				allocation[p] = id;
				blockSize[id] -= processSize[p];
				break;
			}
		}
	}
	displayProcess(allocation, processes, processSize);
}

void bestFit(int* blockSize, int blocks, int* processSize, int processes)
{
	printf("       ********** Best Fit **********\n");

	int* allocation = (int*)malloc(sizeof(int) * processes);
	int id = 0;
	memset(allocation, INVALID, sizeof(int) * processes);
	for (int p = 0; p < processes; ++p)
	{
		int bestIdx = INVALID;
		for (id = 0; id < blocks; id++)
		{
			if (blockSize[id] >= processSize[p])
			{
				if (bestIdx == INVALID || blockSize[id] < blockSize[bestIdx])
				{
					bestIdx = id;
				}
			}
		}
		if (bestIdx != INVALID)
		{
			allocation[p] = bestIdx;
			blockSize[bestIdx] -= processSize[p];
		}
		else
		{
			allocation[p] = INVALID;
		}
	}
	displayProcess(allocation, processes, processSize);
}

void worstFit(int* blockSize, int blocks, int* processSize, int processes)
{
	printf("       ********** Worst Fit **********\n");

	int* allocation = (int*)malloc(sizeof(int) * processes);
	int id = 0;
	memset(allocation, INVALID, sizeof(int) * processes);
	for (int p = 0; p < processes; ++p)
	{
		int worstIdx = INVALID;
		for (id = 0; id < blocks; id++)
		{
			if (blockSize[id] >= processSize[p])
			{
				if (worstIdx == INVALID || blockSize[id] > blockSize[worstIdx])
				{
					worstIdx = id;
				}
			}
		}
		if (worstIdx != INVALID)
		{
			allocation[p] = worstIdx;
			blockSize[worstIdx] -= processSize[p];
		}
		else
		{
			allocation[p] = INVALID;
		}
	}
	displayProcess(allocation, processes, processSize);
}


// main calls displaymenu
int main()
{
	startedAt = time(NULL);
	printf("Select the OS program to run; enter the number of your selection\n");
	int choice = -1; // stores the users menu selection
	while (choice != 0)
	{
		choice = displayMenu();
		switch (choice)
		{

		case 1:
			memoryManagement();
			break;
		case 2:
			fileAttributes();
			break;
		case 3:
			multiThreads();
			break;
		case 0:
			ourExit(0);
		}
	}
	ourExit(0);
}