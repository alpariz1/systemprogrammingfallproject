#include <stdio.h>

#include <stdlib.h>

#include <string.h>



#define MAX_INPUT_FILES 32

#define MAX_PATH_LENGTH 512

#define MAX_TOTAL_SIZE (200 * 1024 * 1024) // 200 MBytes



// Structure to represent a Plate in the organization section

struct  Plate{

    char name_Of_File[MAX_PATH_LENGTH];

    char permissions[10];

    size_t size;

    struct Plate* next;

    struct Plate* prev;

};

// Global variable for the linked list

struct Plate* organizationList = NULL;



// Function to add a Plate to the linked list

void addPlate(struct Plate** head, const char* name_Of_File, const char* permissions, size_t size) {

    struct Plate* newPlate = (struct Plate*)malloc(sizeof(struct Plate));

    if (newPlate == NULL) {

        perror("Error allocating memory for Plate");

        exit(EXIT_FAILURE);

    }



    strncpy(newPlate->name_Of_File, name_Of_File, sizeof(newPlate->name_Of_File));

    strncpy(newPlate->permissions, permissions, sizeof(newPlate->permissions));

    newPlate->size = size;



    newPlate->next = NULL;



    if (*head == NULL) {

        newPlate->prev = NULL;

        *head = newPlate;

    } else {

        struct Plate* current = *head;

        while (current->next != NULL) {

            current = current->next;

        }



        newPlate->prev = current;

        current->next = newPlate;

    }

}



// Function to free the memory used by the linked list

void freePlates(struct Plate** head) {

    struct Plate* current = *head;

    while (current != NULL) {

        struct Plate* next = current->next;

        free(current);

        current = next;

    }

    // Set the head pointer to NULL to avoid double freeing

    *head = NULL;

}



// Function to write Plates from the linked list to the output file

void writePlatesToFile(FILE* outputFile, struct Plate* head) {

    struct Plate* current = head;

    while (current != NULL) {

        if (current->size > 0) {

            // Write the Plate to the output file

            fprintf(outputFile, "|%s,%s,%lu|", current->name_Of_File, current->permissions, current->size);

        }

        current = current->next;

    }

}

// Function to read Plates from the input file and populate the linked list

void fileSplitFromList(FILE* inputFile, struct Plate** head) {

    size_t organizationSize;

    // Read the organization size

    fscanf(inputFile, "%010lu", &organizationSize);



    char buffer[MAX_TOTAL_SIZE]; // Assuming MAX_TOTAL_SIZE is your maximum file size



    // Read and discard the organization section

    if (fgets(buffer, sizeof(buffer), inputFile) == NULL) {

        perror("Error reading organization section");

        exit(EXIT_FAILURE);

    }



    char* token = strtok(buffer, "|");



    while (token != NULL) {

        char name_Of_File[MAX_PATH_LENGTH];

        char permissions[10];

        // Use size as a placeholder, as it's not stored in the structure

        size_t size = 0;



        sscanf(token, "%[^,],%[^,],%lu", name_Of_File, permissions, &size);



        addPlate(head, name_Of_File, permissions, size);



        token = strtok(NULL, "|");

    }

}



void fileSplit(int argc, char *argv[]) {

    if (argc != 4 || strcmp(argv[1], "-b") != 0) {

        printf("Usage: %s -b input.sau output_directory\n", argv[0]);

        exit(EXIT_FAILURE);

    }



    FILE *inputFile = fopen(argv[2], "r");

    if (inputFile == NULL) {

        perror("Error opening input file");

        exit(EXIT_FAILURE);

    }



    // Create a linked list for the organization section

     struct Plate *organizationList = NULL;



    // Populate the linked list with Plates

      fileSplitFromList(inputFile, &organizationList);



    // Write archived files to the output directory

    char outputDirectory[MAX_PATH_LENGTH + 20];

    snprintf(outputDirectory, sizeof(outputDirectory), "%s", argv[3]);



    struct Plate *current = organizationList;

    int fileCount = 1;



    while (current != NULL) {

        if (current->size > 0) {

            char filePath[MAX_PATH_LENGTH + 30];

            snprintf(filePath, sizeof(filePath), "%s/file%d.txt", outputDirectory, fileCount);



            printf("DEBUG: Opening file %s\n", filePath);



            FILE *outputFile = fopen(filePath, "w");

            if (outputFile == NULL) {

                perror("Error opening output file");

                freePlates(organizationList);

                fclose(inputFile);

                exit(EXIT_FAILURE);

            }



            char buffer[1024];

            size_t bytesRead = 0;



            while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {

                // Check for the end of the organization section

                if (strncmp(buffer, "|", 1) == 0) {

                    break;

                }



                printf("DEBUG: Writing to file %s\n", filePath);  // Debug statement

                fputs(buffer, outputFile);

                bytesRead += strlen(buffer);

            }



            fclose(outputFile);

            fileCount++;

        }



        // Update the current pointer to the next node in the linked list

        current = current->next;

    }



    // Free memory used by the linked list

    freePlates(&organizationList);



    fclose(inputFile);

}





// Function to read Plates from the input files and populate the global linked list

void file_Merger(int argc, char *argv[]) {

    if (argc < 5 || argc > MAX_INPUT_FILES + 4) {

        printf("Usage: %s -a file1.txt file2.txt -o output.sau\n", argv[0]);

        exit(EXIT_FAILURE);

    }



    FILE *outputFile = fopen(argv[argc - 1], "w");

    if (outputFile == NULL) {

        perror("Error opening output file");

        exit(EXIT_FAILURE);

    }



    for (int i = 2; i < argc - 2; i++) {

        FILE *inputFile = fopen(argv[i], "r");

        if (inputFile == NULL) {

            perror("Error opening input file");

            fclose(outputFile);

            exit(EXIT_FAILURE);

        }



        char buffer[1024];

        while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {

            fprintf(outputFile, "%s", buffer);

        }



        fclose(inputFile);

    }



    fclose(outputFile);

}



int main(int argc, char *argv[]) {

    if (argc < 4) {

        printf("Usage: \n %s -a file1.txt file2.txt -o a.sau \n %s -b input.sau selectdirectory\n", argv[0], argv[0]);

        exit(EXIT_FAILURE);

    }



   if (strcmp(argv[1], "-a") == 0) {

     file_Merger(argc, argv);  // Uncomment this line if you want to implement file_Merger

    } else if (strcmp(argv[1], "-b") == 0) {

        fileSplit(argc, argv);

    } else {

        printf("Invalid command\n");

        exit(EXIT_FAILURE);

    }

    return 0;

}