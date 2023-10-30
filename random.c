#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

sem_t readWriteSem;
int semaphoreValue = 1;

static void *outputThread(void *arg)
{
	const int readFd = *((const int *)arg);

	for (;;)
	{
		int number;

		sem_wait(&readWriteSem);
		semaphoreValue--;

		// Aus Pipe lesen. ACHTUNG: Fehler prüfen!!!
		if ((errno = read(readFd, &number, sizeof(int))) == -1)
		{
			perror("failed to read from pipe");
			exit(EXIT_FAILURE);
		}

		printf("%i\n", number);

		semaphoreValue++;
		sem_post(&readWriteSem);
	}

	return NULL;
}

static void *randomThread(void *arg)
{
	const int writeFd = *((const int *)arg);

	for (;;)
	{
		sem_wait(&readWriteSem);
		semaphoreValue--;
		const int number = rand();

		// Zufallszahl in Pipe schreiben. ACHTUNG: Fehler abfragen!!!
		if ((errno = write(writeFd, &number, sizeof(int))) == -1)
		{
			perror("failed to write data to pipe");
			exit(EXIT_FAILURE);
		}
		semaphoreValue++;
		sem_post(&readWriteSem);
	}

	return NULL;
}

static void startStop(void)
{
	bool running = false;

	for (;;)
	{
		const int ch = getchar();
		if (ch == EOF)
		{
			if (ferror(stdin)) // error
			{
				perror("getchar");
				exit(EXIT_FAILURE);
			}
			else // end of file, Ctrl+D
				return;
		}
		else if (ch == '\n')
		{
			if (running)
			{
				running = false;
				sem_wait(&readWriteSem);
				semaphoreValue--;
			}
			else
			{
				running = true;
				semaphoreValue++;
				sem_post(&readWriteSem);
			}
		}
	}
}

int main(void)
{
	sem_init(&readWriteSem, 0, 1U);

	int pipeFds[2];
	// Pipe erzeugen
	if ((errno = pipe(pipeFds)) != 0)
	{
		perror("failed to create a pipe");
		exit(EXIT_FAILURE);
	}

	pthread_t outputThreadHandle;
	if ((errno = pthread_create(&outputThreadHandle, NULL, outputThread, &pipeFds[0])) != 0)
	{
		perror("pthread_create (outputThread)");
		return EXIT_FAILURE;
	}

	pthread_t randomThreadHandle;
	if ((errno = pthread_create(&randomThreadHandle, NULL, randomThread, &pipeFds[1])) != 0)
	{
		perror("pthread_create (randomThread)");
		return EXIT_FAILURE;
	}

	startStop();
	return EXIT_SUCCESS;
}
