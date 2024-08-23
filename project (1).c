/******************************************************************************

                            Online C Compiler.
                Code, Compile, Run and Debug C program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

typedef struct {
    char username[MAX_LEN];
    char password[MAX_LEN];
} User;

typedef struct {
    char bus_number[MAX_LEN];
    char origin[MAX_LEN];
    char destination[MAX_LEN];
    int total_seats;
    int available_seats;
    int fare;
} Bus;

typedef struct {
    char username[MAX_LEN];
    char bus_number[MAX_LEN];
    char name[MAX_LEN];
    int age;
} Booking;


void register_user() {
    // Open the file in append mode
    FILE *file = fopen("users.txt", "a");
    if (!file) {
        perror("Failed to open users file");
        return;
    }
    
    // Create a User instance
    User user;
    
    // Prompt for and read the username
    printf("Enter username: ");
    scanf("%49s", user.username);  // Limiting input to prevent buffer overflow
    
    // Prompt for and read the password
    printf("Enter password: ");
    scanf("%49s", user.password);  // Limiting input to prevent buffer overflow
    
    // Write the username and password to the file
    if (fprintf(file, "%s %s\n", user.username, user.password) < 0) {
        perror("Failed to write to users file");
    }
    
    // Flush the buffer to ensure data is written to the file
    fflush(file);
    
    // Close the file
    fclose(file);
    
    // Confirm successful registration
    printf("Registration successful.\n");
}

int login_user(char *logged_in_username) {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        perror("Failed to open users file");
        return 0;
    }
    
    User user;
    char username[MAX_LEN], password[MAX_LEN];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    
    while (fscanf(file, "%s %s", user.username, user.password) != EOF) {
        if (strcmp(user.username, username) == 0 && strcmp(user.password, password) == 0) {
            fclose(file);
            strcpy(logged_in_username, username);
            printf("Login successful.\n");
            return 1;
        }
    }
    
    fclose(file);
    printf("Invalid username or password.\n");
    return 0;
}

void load_buses(Bus buses[], int *bus_count) {
    FILE *file = fopen("buses.txt", "r");
    if (!file) {
        perror("Failed to open buses file");
        return;
    }
    
    *bus_count = 0;
    while (fscanf(file, "%s %s %s %d %d %d", buses[*bus_count].bus_number, buses[*bus_count].origin,
                  buses[*bus_count].destination, &buses[*bus_count].total_seats, 
                  &buses[*bus_count].available_seats, &buses[*bus_count].fare) != EOF) {
        (*bus_count)++;
    }
    
    fclose(file);
}

void save_buses(Bus buses[], int bus_count) {
    FILE *file = fopen("buses.txt", "w");
    if (!file) {
        perror("Failed to open buses file");
        return;
    }
    
    for (int i = 0; i < bus_count; i++) {
        fprintf(file, "%s %s %s %d %d %d\n", buses[i].bus_number, buses[i].origin, buses[i].destination,
                buses[i].total_seats, buses[i].available_seats, buses[i].fare);
    }
    
    fclose(file);
}

void book_ticket(char *username) {
    Bus buses[MAX_LEN];
    int bus_count;
    load_buses(buses, &bus_count);
    
    char bus_number[MAX_LEN];
    printf("Enter bus number: ");
    scanf("%s", bus_number);
    
    int i;
    for (i = 0; i < bus_count; i++) {
        if (strcmp(buses[i].bus_number, bus_number) == 0) {
            break;
        }
    }
    
    if (i == bus_count) {
        printf("Invalid bus number.\n");
        return;
    }
    
    if (buses[i].available_seats == 0) {
        printf("No available seats.\n");
        return;
    }
    
    Booking booking;
    strcpy(booking.username, username);
    strcpy(booking.bus_number, bus_number);
    printf("Enter your name: ");
    scanf("%s", booking.name);
    printf("Enter your age: ");
    scanf("%d", &booking.age);
    
    FILE *file = fopen("bookings.txt", "a");
    if (!file) {
        perror("Failed to open bookings file");
        return;
    }
    
    fprintf(file, "%s %s %s %d\n", booking.username, booking.bus_number, booking.name, booking.age);
    fclose(file);
    
    buses[i].available_seats--;
    save_buses(buses, bus_count);
    
    printf("Ticket booked successfully.\n");
}

void cancel_ticket(char *username) {
    Bus buses[MAX_LEN];
    int bus_count;
    load_buses(buses, &bus_count);
    
    char bus_number[MAX_LEN], name[MAX_LEN];
    printf("Enter bus number: ");
    scanf("%s", bus_number);
    printf("Enter your name: ");
    scanf("%s", name);
    
    FILE *file = fopen("bookings.txt", "r");
    if (!file) {
        perror("Failed to open bookings file");
        return;
    }
    
    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Failed to open temp file");
        fclose(file);
        return;
    }
    
    Booking booking;
    int found = 0;
    while (fscanf(file, "%s %s %s %d", booking.username, booking.bus_number, booking.name, &booking.age) != EOF) {
        if (strcmp(booking.username, username) == 0 && strcmp(booking.bus_number, bus_number) == 0 &&
            strcmp(booking.name, name) == 0) {
            found = 1;
        } else {
            fprintf(temp_file, "%s %s %s %d\n", booking.username, booking.bus_number, booking.name, booking.age);
        }
    }
    
    fclose(file);
    fclose(temp_file);
    
    if (found) {
        remove("bookings.txt");
        rename("temp.txt", "bookings.txt");
        
        for (int i = 0; i < bus_count; i++) {
            if (strcmp(buses[i].bus_number, bus_number) == 0) {
                buses[i].available_seats++;
                break;
            }
        }
        
        save_buses(buses, bus_count);
        printf("Ticket cancelled successfully.\n");
    } else {
        remove("temp.txt");
        printf("No booking found with the provided details.\n");
    }
}

void check_bus_status() {
    Bus buses[MAX_LEN];
    int bus_count;
    load_buses(buses, &bus_count);
    
    char bus_number[MAX_LEN];
    printf("Enter bus number: ");
    scanf("%s", bus_number);
    
    for (int i = 0; i < bus_count; i++) {
        if (strcmp(buses[i].bus_number, bus_number) == 0) {
            printf("Bus Number: %s\n", buses[i].bus_number);
            printf("Origin: %s\n", buses[i].origin);
            printf("Destination: %s\n", buses[i].destination);
            printf("Total Seats: %d\n", buses[i].total_seats);
            printf("Available Seats: %d\n", buses[i].available_seats);
            printf("Fare: %d\n", buses[i].fare);
            return;
        }
    }
    
    printf("Invalid bus number.\n");
}

int main() {
    char logged_in_username[MAX_LEN] = "";
    int logged_in = 0;
    
    while (1) {
        printf("\n1. Register\n");
        printf("2. Login\n");
        printf("3. Book Ticket\n");
        printf("4. Cancel Ticket\n");
        printf("5. Check Bus Status\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        
        int choice;
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                register_user();
                break;
            case 2:
                logged_in = login_user(logged_in_username);
                break;
            case 3:
                if (!logged_in) {
                    printf("Please login first.\n");
                } else {
                    book_ticket(logged_in_username);
                }
                break;
            case 4:
                if (!logged_in) {
                    printf("Please login first.\n");
                } else {
                    cancel_ticket(logged_in_username);
                }
                break;
            case 5:
                check_bus_status();
                break;
            case 6:
                exit(0);
            default:
                printf("Invalid choice.\n");
        }
    }
    
    return 0;
}
