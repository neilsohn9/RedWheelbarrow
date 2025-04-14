#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUSES 10
#define MAX_TICKETS 100
#define MAX_NAME_LENGTH 50
#define MAX_ID_LENGTH 10

typedef struct {
    int routeNumber;
    char departureTime[10];
    int totalSeats;
    int availableSeats;
    double fare;
} Bus;

typedef struct {
    char passengerName[MAX_NAME_LENGTH];
    int routeNumber;
    int numSeats;
    char bookingID[MAX_ID_LENGTH];
    double totalFare;
} Ticket;

typedef struct {
    Bus buses[MAX_BUSES];
    Ticket tickets[MAX_TICKETS];
    int busCount;
    int ticketCount;
    int nextBookingID;
} BookingSystem;

void initBus(Bus* bus, int route, const char* time, int seats, double price) {
    bus->routeNumber = route;
    strcpy(bus->departureTime, time);
    bus->totalSeats = seats;
    bus->availableSeats = seats;
    bus->fare = price;
}

int bookSeats(Bus* bus, int numSeats) {
    if (bus->availableSeats >= numSeats) {
        bus->availableSeats -= numSeats;
        return 1;
    }
    return 0;
}

void cancelSeats(Bus* bus, int numSeats) {
    bus->availableSeats = (bus->availableSeats + numSeats > bus->totalSeats)
        ? bus->totalSeats : bus->availableSeats + numSeats;
}

void initTicket(Ticket* ticket, const char* name, int route, int seats, const char* id, double fare) {
    strcpy(ticket->passengerName, name);
    ticket->routeNumber = route;
    ticket->numSeats = seats;
    strcpy(ticket->bookingID, id);
    ticket->totalFare = fare * seats;
}

void displayTicket(const Ticket* ticket) {
    printf("\n--- Ticket Details ---\n");
    printf("Booking ID: %s\n", ticket->bookingID);
    printf("Passenger: %s\n", ticket->passengerName);
    printf("Route: %d\n", ticket->routeNumber);
    printf("Seats: %d\n", ticket->numSeats);
    printf("Total Fare: Ksh%.2f\n", ticket->totalFare);
}

void initializeSampleBuses(BookingSystem* system) {
    system->busCount = 3;
    system->ticketCount = 0;
    system->nextBookingID = 1000;

    initBus(&system->buses[0], 101, "08:00 AM", 50, 500);
    initBus(&system->buses[1], 102, "09:30 AM", 40, 600);
    initBus(&system->buses[2], 103, "11:15 AM", 35, 700);
}

void displayAvailableBuses(const BookingSystem* system) {
    printf("\nAvailable Buses:\n");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < system->busCount; i++) {
        printf("Route: %d\tDeparture: %s\tSeats: %d\tFare: Ksh%.2f\n",
               system->buses[i].routeNumber,
               system->buses[i].departureTime,
               system->buses[i].availableSeats,
               system->buses[i].fare);
    }
}

Bus* findBus(BookingSystem* system, int routeNumber) {
    for (int i = 0; i < system->busCount; i++) {
        if (system->buses[i].routeNumber == routeNumber) {
            return &system->buses[i];
        }
    }
    return NULL;
}

void generateBookingID(BookingSystem* system, char* id) {
    sprintf(id, "BID%d", system->nextBookingID++);
}

void bookTicket(BookingSystem* system) {
    char name[MAX_NAME_LENGTH];
    int route, seats;

    printf("\nEnter passenger name: ");
    getchar(); // Clear buffer
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0; // Remove newline

    printf("Enter route number: ");
    scanf("%d", &route);

    Bus* selectedBus = findBus(system, route);
    if (!selectedBus) {
        printf("Invalid route number!\n");
        return;
    }

    printf("Enter number of seats: ");
    scanf("%d", &seats);

    if (seats <= 0) {
        printf("Invalid number of seats!\n");
        return;
    }

    if (bookSeats(selectedBus, seats)) {
        char bookingID[MAX_ID_LENGTH];
        generateBookingID(system, bookingID);
        initTicket(&system->tickets[system->ticketCount], name, route, seats, bookingID, selectedBus->fare);
        printf("Booking successful!\n");
        displayTicket(&system->tickets[system->ticketCount]);
        system->ticketCount++;
    } else {
        printf("Failed to book. Not enough seats available!\n");
    }
}

void cancelTicket(BookingSystem* system) {
    char bid[MAX_ID_LENGTH];
    printf("Enter booking ID to cancel: ");
    scanf("%s", bid);

    for (int i = 0; i < system->ticketCount; i++) {
        if (strcmp(system->tickets[i].bookingID, bid) == 0) {
            Bus* bus = findBus(system, system->tickets[i].routeNumber);
            if (bus) {
                cancelSeats(bus, system->tickets[i].numSeats);
                // Shift remaining tickets
                for (int j = i; j < system->ticketCount - 1; j++) {
                    system->tickets[j] = system->tickets[j + 1];
                }
                system->ticketCount--;
                printf("Cancellation successful!\n");
                return;
            }
        }
    }
    printf("Invalid booking ID!\n");
}

int main() {
    BookingSystem system;
    initializeSampleBuses(&system);

    int choice;
    do {
        printf("\nPublic Transport Booking System\n");
        printf("1. View Available Buses\n");
        printf("2. Book Tickets\n");
        printf("3. Cancel Booking\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayAvailableBuses(&system);
                break;
            case 2:
                bookTicket(&system);
                break;
            case 3:
                cancelTicket(&system);
                break;
            case 4:
                printf("Exiting system. Thank you!\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (choice != 4);

    return 0;
}
