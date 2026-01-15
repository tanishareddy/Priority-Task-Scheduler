#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>

typedef struct {
    char name[50];
    double priority;
} task_node;

typedef struct {
    task_node** heap;
    int size;
    int capacity;
} task_scheduler;

task_scheduler* scheduler_init(int initial_capacity);
void scheduler_destroy(task_scheduler* sch);
void add(task_scheduler* sch, const char* name, double priority);
char* removeSmallest(task_scheduler* sch);
const char* getSmallest(const task_scheduler* sch);
void changePriority(task_scheduler* sch, const char* name, double new_priority);
bool contains(const task_scheduler* sch, const char* name);
int size(const task_scheduler* sch);

// New CSV-related functions
void save_to_csv(const task_scheduler* sch, const char* filename);
void load_from_csv(task_scheduler* sch, const char* filename);

#endif