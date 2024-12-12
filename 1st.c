#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 100
#define MAX_SLOTS 10
#define HASH_SIZE 101

typedef struct TimeSlot {
    char time[6]; // Format HH:MM
    int isBooked;
    struct TimeSlot *left;
    struct TimeSlot *right;
} TimeSlot;

typedef struct Patient {
    char name[50];
    char appointmentTime[6];
    struct Patient *next;
} Patient;

typedef struct HashMap {
    Patient *table[HASH_SIZE];
} HashMap;

// Function prototypes
TimeSlot* createTimeSlot(char *time);
void insertTimeSlot(TimeSlot **root, char *time);
int searchTimeSlot(TimeSlot *root, char *time);
void bookTimeSlot(TimeSlot **root, char *time);
void cancelTimeSlot(TimeSlot **root, char *time);
unsigned int hash(char *name);
void addPatient(HashMap *map, char *name, char *time);
void removePatient(HashMap *map, char *name);
void printPatients(HashMap *map);
void freeTimeSlots(TimeSlot *root);
void freePatients(HashMap *map);

int main() {
    TimeSlot *timeSlots = NULL;
    HashMap patientMap = {0};

    // Initialize time slots
    char *availableTimes[MAX_SLOTS] = {"09:00", "09:30", "10:00", "10:30", "11:00", "11:30", "12:00", "13:00", "13:30", "14:00"};
    for (int i = 0; i < MAX_SLOTS; i++) {
        insertTimeSlot(&timeSlots, availableTimes[i]);
    }

    // Example operations
    bookTimeSlot(&timeSlots, "10:00");
    addPatient(&patientMap, "Aditya", "10:00");
    
    bookTimeSlot(&timeSlots, "10:00"); // Attempt to double-book
    addPatient(&patientMap, "Mukul", "10:00"); // Attempt to add another patient to the same slot

    printPatients(&patientMap);
    
    cancelTimeSlot(&timeSlots, "10:00");
    removePatient(&patientMap, "Aditya");

    printPatients(&patientMap);

    // Free allocated memory
    freeTimeSlots(timeSlots);
    freePatients(&patientMap);

    return 0;
}

// Function to create a new time slot
TimeSlot* createTimeSlot(char *time) {
    TimeSlot *newSlot = (TimeSlot *)malloc(sizeof(TimeSlot));
    strcpy(newSlot->time, time);
    newSlot->isBooked = 0;
    newSlot->left = newSlot->right = NULL;
    return newSlot;
}

// Function to insert a time slot into the BST
void insertTimeSlot(TimeSlot **root, char *time) {
    if (*root == NULL) {
        *root = createTimeSlot(time);
    } else if (strcmp(time, (*root)->time) < 0) {
        insertTimeSlot(&(*root)->left, time);
    } else {
        insertTimeSlot(&(*root)->right, time);
    }
}

// Function to search for a time slot in the BST
int searchTimeSlot(TimeSlot *root, char *time) {
    if (root == NULL) {
        return 0; // Not found
    }
    if (strcmp(root->time, time) == 0) {
        return 1; // Found
    } else if (strcmp(time, root->time) < 0) {
        return searchTimeSlot(root->left, time);
    } else {
        return searchTimeSlot(root->right, time);
    }
}

// Function to book a time slot
void bookTimeSlot(TimeSlot **root, char *time) {
    if (searchTimeSlot(*root, time)) {
        TimeSlot *slot = *root;
        while (slot != NULL) {
            if (strcmp(slot->time, time) == 0) {
                if (slot->isBooked) {
                    printf("Time slot %s is already booked.\n", time);
                    return;
                } else {
                    slot->isBooked = 1;
                    printf("Time slot %s booked successfully.\n", time);
                    return;
                }
            } else if (strcmp(time, slot->time) < 0) {
                slot = slot->left;
            } else {
                slot = slot->right;
            }
        }
    } else {
        printf("Time slot %s does not exist.\n", time);
    }
}

// Function to cancel a time slot
void cancelTimeSlot(TimeSlot **root, char *time) {
    if (searchTimeSlot(*root, time)) {
        TimeSlot *slot = *root;
        while (slot != NULL) {
            if (strcmp(slot->time, time) == 0) {
                if (!slot->isBooked) {
                    printf("Time slot %s is not booked.\n", time);
                    return;
                } else {
                    slot->isBooked = 0;
                    printf("Time slot %s canceled successfully.\n", time);
                    return;
                }
            } else if (strcmp(time, slot->time) < 0) {
                slot = slot->left;
            } else {
                slot = slot->right;
            }
        }
    } else {
        printf("Time slot %s does not exist.\n", time);
    }
}

// Hash function for patient names
unsigned int hash(char *name) {
    unsigned int hashValue = 0;
    while (*name) {
        hashValue = (hashValue << 5) + *name++;
    }
    return hashValue % HASH_SIZE;
}

// Function to add a patient to the hash map
void addPatient(HashMap *map, char *name, char *time) {
    unsigned int index = hash(name);
    Patient *newPatient = (Patient *)malloc(sizeof(Patient));
    strcpy(newPatient->name, name);
    strcpy(newPatient->appointmentTime, time);
    newPatient->next = map->table[index];
    map->table[index] = newPatient;
    printf("Patient %s added with appointment at %s.\n", name, time);
}

// Function to remove a patient from the hash map
void removePatient(HashMap *map, char *name) {
    unsigned int index = hash(name);
    Patient *current = map->table[index];
    Patient *previous = NULL;

    while (current != NULL && strcmp(current->name, name) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Patient %s not found.\n", name);
        return;
    }

    if (previous == NULL) {
        map->table[index] = current->next;
    } else {
        previous->next = current->next;
    }
    free(current);
    printf("Patient %s removed successfully.\n", name);
}

// Function to print all patients in the hash map
void printPatients(HashMap *map) {
    printf("Patient Records:\n");
    for (int i = 0; i < HASH_SIZE; i++) {
        Patient *current = map->table[i];
        while (current != NULL) {
            printf("Name: %s, Appointment Time: %s\n", current->name, current->appointmentTime);
            current = current->next;
        }
    }
}

// Function to free memory allocated for time slots
void freeTimeSlots(TimeSlot *root) {
    if (root != NULL) {
        freeTimeSlots(root->left);
        freeTimeSlots(root->right);
        free(root);
    }
}

// Function to free memory allocated for patients
void freePatients(HashMap *map) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Patient *current = map->table[i];
        while (current != NULL) {
            Patient *temp = current;
            current = current->next;
            free(temp);
        }
    }
}