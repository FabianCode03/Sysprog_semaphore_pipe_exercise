#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

static void *outputThread(void *arg)
{
	const int readFd = *((const int *)arg);

	for (;;)
	{
		int number;

		// Aus Pipe lesen. ACHTUNG: Fehler prüfen!!!
		if ((errno = read(readFd, number, sizeof(int))) == -1)
		{
			perror("failed to read from pipe");
			exit(EXIT_FAILURE);
		}

		printf("%i\n", number);
	}

	return NULL;
}

static void *randomThread(void *arg)
{
	const int writeFd = *((const int *)arg);

	for (;;)
	{
		const int number = rand();

		// Zufallszahl in Pipe schreiben. ACHTUNG: Fehler abfragen!!!
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
			}
			else
			{
				running = true;
			}
		}
	}
}

int main(void)
{
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
