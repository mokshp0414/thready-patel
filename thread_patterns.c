#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

typedef struct
{
    int current;
    int total;
} thread_args;

void *thread_work(void *arg);
void start_pattern1(int n);
void start_pattern2(int n);
void start_pattern3(thread_args *args);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <number_of_things> <pattern_number>\n", argv[0]);
        return 1;
    }
    // Convert input strings to integers
    int numThings = atoi(argv[1]);
    int patternNum = atoi(argv[2]);

    if (numThings < 1 || numThings > 256 || (patternNum < 1 || patternNum > 3))
    {
        fprintf(stderr, "Error: Number of things must be between 1 and 256, and pattern number must be 1, 2, or 3.\n");
        return 1;
    }
    // rnadom number generator
    srand(time(NULL));

    // Choose the pattern based on the input
    switch (patternNum)
    {
    case 1:
        start_pattern1(numThings);
        break;
    case 2:
        start_pattern2(numThings);
        break;
    case 3:
        thread_args args = {1, numThings};
        start_pattern3(&args);
        break;
    default:
        fprintf(stderr, "Invalid pattern number.\n");
        return 1;
    }

    return 0;
}

void *thread_work(void *arg)
{
    thread_args *args = (thread_args *)arg;
    int sleep_time = rand() % 8 + 1;
    printf("Thread %d (TID: %ld) beginning\n", args->current, pthread_self());
    sleep(sleep_time);

    if (args->current == 1 && args->total > 1)
    {
        for (int i = 2; i <= args->total; i++)
        {
            thread_args *new_args = malloc(sizeof(thread_args));
            new_args->current = i;
            new_args->total = args->total;
            pthread_t new_thread;
            pthread_create(&new_thread, NULL, thread_work, new_args);
            pthread_join(new_thread, NULL);
        }
    }

    printf("Thread %d (TID: %ld) exiting after %d seconds\n", args->current, pthread_self(), sleep_time);
    free(arg);
    return NULL;
}

// Starts multiple threads at the same time
void start_pattern1(int n)
{
    pthread_t threads[n];
    printf("Starting pattern 1 with %d threads...\n", n);
    for (int i = 0; i < n; i++)
    {
        thread_args *args = malloc(sizeof(thread_args));
        args->current = i + 1;
        args->total = n;
        pthread_create(&threads[i], NULL, thread_work, args);
        printf("Parent: created thread %d (TID: %ld)\n", i + 1, threads[i]);
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(threads[i], NULL);
    }
    printf("All threads have completed.\n");
}

// Starts threads one ata time
void start_pattern2(int n)
{
    pthread_t thread;
    printf("Starting pattern 2 with %d threads...\n", n);
    for (int i = 0; i < n; i++)
    {
        thread_args *args = malloc(sizeof(thread_args));
        args->current = i + 1;
        args->total = n;
        pthread_create(&thread, NULL, thread_work, args);
        pthread_join(thread, NULL);
        printf("Parent: created thread %d (TID: %ld) and waited for it to exit\n", i + 1, thread);
    }
    printf("All threads have exited.\n");
}

void start_pattern3(thread_args *args)
{
    pthread_t thread;
    pthread_create(&thread, NULL, thread_work, args);
    pthread_join(thread, NULL);
}
