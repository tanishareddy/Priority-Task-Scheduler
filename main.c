#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "scheduler.h" 

void flush_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    task_scheduler* scheduler = scheduler_init(10); 
    if (scheduler == NULL) {
        fprintf(stderr, "Failed to initialize scheduler.\n");
        return 1;
    }

    load_from_csv(scheduler, "tasks.csv");

    int choice;
    char task_name[50];
    double priority;
    char input_buffer[100];

    printf("--- Interactive Priority-Based Task Scheduler ---\n");

    while (true) {
        printf("\nScheduler Menu (Tasks: %d):\n", size(scheduler));
        printf("1. Add Task\n");
        printf("2. Remove Most Urgent Task\n");
        printf("3. Get Most Urgent Task (Peek)\n");
        printf("4. Change Task Priority\n");
        printf("5. Check if Task Exists\n");
        printf("6. Get Total Task Count\n");
        printf("7. Exit (Save & Quit)\n");
        printf("Enter your choice: ");

        if (fgets(input_buffer, sizeof(input_buffer), stdin) == NULL) {
            choice = 7;
        } else {
            if (sscanf(input_buffer, "%d", &choice) != 1) {
                choice = 0;
            }
        }

        switch (choice) {
            case 1:
                printf("Enter task name (max 49 chars): ");
                if (fgets(task_name, sizeof(task_name), stdin) != NULL) {
                    task_name[strcspn(task_name, "\n")] = 0;
                    printf("Enter priority (smaller is more urgent, e.g., 1.0): ");
                    if (scanf("%lf", &priority) == 1) {
                        add(scheduler, task_name, priority);
                    } else {
                        printf("Invalid priority input.\n");
                    }
                    flush_input();
                }
                break;

            case 2: {
                char* removed_task = removeSmallest(scheduler);
                if (removed_task) {
                    printf("SUCCESS: Removed most urgent task: '%s'\n", removed_task);
                    free(removed_task);
                } else {
                    printf("The scheduler is currently empty.\n");
                }
                break;
            }

            case 3: {
                const char* smallest_task = getSmallest(scheduler);
                if (smallest_task) {
                    printf("The most urgent task is: '%s'\n", smallest_task);
                } else {
                    printf("The scheduler is currently empty.\n");
                }
                break;
            }

            case 4:
                printf("Enter task name to update: ");
                if (fgets(task_name, sizeof(task_name), stdin) != NULL) {
                    task_name[strcspn(task_name, "\n")] = 0;
                    printf("Enter NEW priority (smaller is more urgent): ");
                    if (scanf("%lf", &priority) == 1) {
                        changePriority(scheduler, task_name, priority);
                    } else {
                        printf("Invalid priority input.\n");
                    }
                    flush_input();
                }
                break;

            case 5:
                printf("Enter task name to check: ");
                if (fgets(task_name, sizeof(task_name), stdin) != NULL) {
                    task_name[strcspn(task_name, "\n")] = 0;
                    if (contains(scheduler, task_name)) {
                        printf("Task '%s' EXISTS in the scheduler.\n", task_name);
                    } else {
                        printf("Task '%s' DOES NOT exist in the scheduler.\n", task_name);
                    }
                }
                break;

            case 6:
                printf("Total number of pending tasks: %d\n", size(scheduler));
                break;

            case 7:
                printf("\nSaving tasks before exit...\n");
                save_to_csv(scheduler, "tasks.csv");
                printf("Exiting scheduler. Goodbye!\n");
                scheduler_destroy(scheduler);
                return 0;

            default:
                printf("Invalid choice. Please enter a number between 1 and 7.\n");
                break;
        }
    }
}