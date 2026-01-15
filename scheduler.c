#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void swap_tasks(task_node** a, task_node** b);
static void heapify_down(task_scheduler* sch, int index);
static void heapify_up(task_scheduler* sch, int index);
static int find_task_index(const task_scheduler* sch, const char* name);
static void resize_scheduler(task_scheduler* sch);

static void swap_tasks(task_node** a, task_node** b) {
    task_node* temp = *a;
    *a = *b;
    *b = temp;
}

static void heapify_down(task_scheduler* sch, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < sch->size && sch->heap[left]->priority < sch->heap[smallest]->priority) {
        smallest = left;
    }
    if (right < sch->size && sch->heap[right]->priority < sch->heap[smallest]->priority) {
        smallest = right;
    }

    if (smallest != index) {
        swap_tasks(&sch->heap[index], &sch->heap[smallest]);
        heapify_down(sch, smallest);
    }
}

static void heapify_up(task_scheduler* sch, int index) {
    while (index > 0 && sch->heap[index]->priority < sch->heap[(index - 1) / 2]->priority) {
        int parent = (index - 1) / 2;
        swap_tasks(&sch->heap[index], &sch->heap[parent]);
        index = parent;
    }
}

static int find_task_index(const task_scheduler* sch, const char* name) {
    for (int i = 0; i < sch->size; i++) {
        if (strcmp(sch->heap[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

static void resize_scheduler(task_scheduler* sch) {
    int new_capacity = sch->capacity * 2;
    if (new_capacity == 0)
        new_capacity = 1;

    task_node** new_heap = realloc(sch->heap, new_capacity * sizeof(task_node*));

    if (new_heap == NULL) {
        perror("Error reallocating memory for heap");
        exit(EXIT_FAILURE);
    }
    sch->heap = new_heap;
    sch->capacity = new_capacity;
}   

task_scheduler* scheduler_init(int initial_capacity) {
    task_scheduler* sch = (task_scheduler*)malloc(sizeof(task_scheduler));
    if (sch == NULL) return NULL;

    sch->capacity = initial_capacity > 0 ? initial_capacity : 10;
    sch->size = 0;
    sch->heap = malloc(sch->capacity * sizeof(task_node*));
    if (sch->heap == NULL) {
        free(sch);
        return NULL;
    }
    return sch;
}

void scheduler_destroy(task_scheduler* sch) {
    if (sch == NULL) return;

    for (int i = 0; i < sch->size; i++) {
        free(sch->heap[i]);
    }
    free(sch->heap);
    free(sch);
}

bool contains(const task_scheduler* sch, const char* name) {
    return find_task_index(sch, name) != -1;
}

void add(task_scheduler* sch, const char* name, double priority) {
    if (contains(sch, name)) {
        printf("Error: Task '%s' already exists. Use changePriority to update.\n", name);
        return;
    }

    if (sch->size == sch->capacity) {
        resize_scheduler(sch);
    }

    task_node* new_task = (task_node*)malloc(sizeof(task_node));
    if (new_task == NULL) return;

    strncpy(new_task->name, name, 49);
    new_task->name[49] = '\0';
    new_task->priority = priority;

    sch->heap[sch->size] = new_task;
    sch->size++;

    heapify_up(sch, sch->size - 1);
    printf("Task '%s' added with priority %.2f.\n", name, priority);
}

const char* getSmallest(const task_scheduler* sch) {
    if (sch->size == 0) {
        return NULL;
    }
    return sch->heap[0]->name;
}

char* removeSmallest(task_scheduler* sch) {
    if (sch->size == 0) {
        return NULL;
    }

    task_node* root_task = sch->heap[0];
    
    char* task_name = (char*)malloc(strlen(root_task->name) + 1);
    if (task_name == NULL) return NULL;
    strcpy(task_name, root_task->name);

    sch->size--;
    if (sch->size > 0) {
        sch->heap[0] = sch->heap[sch->size];
        heapify_down(sch, 0);
    }

    free(root_task);

    return task_name;
}

int size(const task_scheduler* sch) {
    return sch->size;
}

void changePriority(task_scheduler* sch, const char* name, double new_priority) {
    int index = find_task_index(sch, name);

    if (index == -1) {
        printf("Error: Task '%s' not found. Cannot change priority.\n", name);
        return;
    }

    double old_priority = sch->heap[index]->priority;
    sch->heap[index]->priority = new_priority;

    if (new_priority < old_priority) {
        heapify_up(sch, index);
    } else if (new_priority > old_priority) {
        heapify_down(sch, index);
    }
    printf("Priority of task '%s' updated from %.2f to %.2f.\n", name, old_priority, new_priority);
}

void save_to_csv(const task_scheduler* sch, const char* filename) {
    if (sch == NULL || filename == NULL) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for saving");
        return;
    }

    fprintf(file, "Task,Priority\n");
    for (int i = 0; i < sch->size; i++) {
        fprintf(file, "%s,%.2f\n", sch->heap[i]->name, sch->heap[i]->priority);
    }
    fclose(file);
    printf("Tasks successfully saved to '%s'.\n", filename);
}

void load_from_csv(task_scheduler* sch, const char* filename) {
    if (sch == NULL || filename == NULL) return;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("⚠  No previous task file found. Starting fresh.\n");
        return;
    }

    char line[100];
    char task_name[50];
    double priority;

    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "%49[^,],%lf", task_name, &priority) == 2) {
            add(sch, task_name, priority);
        }
    }

    fclose(file);
    printf("Tasks successfully loaded from '%s'.\n", filename);
}