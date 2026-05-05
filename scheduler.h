// scheduler.h
// public interface for the scheduler module

#ifndef SCHEDULER_H
#define SCHEDULER_H

// init the queue lock, semaphore and preempt pipe
void scheduler_init(void);

// spawn the scheduler thread
void scheduler_start(void);

// add a new command to the queue, returns 0 on success or -1 on malloc fail
int scheduler_enqueue(int client_id, int client_socket, const char *command);

// drop all tasks belonging to this client, called on disconnect
void scheduler_remove_client(int client_socket);

#endif
