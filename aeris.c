#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* =========================================================
   AERIS - AIRPORT OPERATING SYSTEM SIMULATOR
   ========================================================= */

/* -------------------- LIMITS -------------------- */

#define MAX_FLIGHTS 100
#define MAX_AIRCRAFT 100
#define MAX_GATES 20
#define MAX_RUNWAYS 5
#define MAX_PASSENGERS 300
#define MAX_BAGGAGE 400
#define MAX_ALERTS 100

/* -------------------- FILES -------------------- */

#define FLIGHT_FILE "flights.dat"
#define AIRCRAFT_FILE "aircraft.dat"
#define PASSENGER_FILE "passengers.dat"
#define BAGGAGE_FILE "baggage.dat"
#define GATE_FILE "gates.dat"
#define RUNWAY_FILE "runways.dat"
#define ALERT_FILE "alerts.dat"
#define LOG_FILE "aeris.log"

/*----------STRUCTURES---------------*/


typedef struct
{
    char flightNo[12];
    char airline[30];
    char origin[30];
    char destination[30];
    char time[8];
    char status[20];

    char aircraftId[12];
    char gateId[8];
    char runwayId[8];

} Flight;

typedef struct
{
    char id[12];
    char model[30];
    int capacity;
    char status[20];

} Aircraft;

typedef struct
{
    char id[8];
    char status[15];
    char flightNo[12];

} Gate;

typedef struct
{
    char id[8];
    char status[15];
    char flightNo[12];

} Runway;

typedef struct
{
    int id;
    char name[50];
    char flightNo[12];
    int checkedIn;

} Passenger;

typedef struct
{
    int id;
    int passengerId;
    char flightNo[12];
    char status[20];

} Baggage;

typedef struct
{
    int id;
    char type[20];
    char message[100];
    char status[15];

} Alert;

/*----------GLOBAL ARRAYS---------------*/

Flight flights[MAX_FLIGHTS];
Aircraft aircraft[MAX_AIRCRAFT];
Gate gates[MAX_GATES];
Runway runways[MAX_RUNWAYS];
Passenger passengers[MAX_PASSENGERS];
Baggage baggage[MAX_BAGGAGE];
Alert alerts[MAX_ALERTS];

/* Record counters */

int flightCount = 0;
int aircraftCount = 0;
int gateCount = 0;
int runwayCount = 0;
int passengerCount = 0;
int baggageCount = 0;
int alertCount = 0;

/*------------INPUT FUNCTIONS------------*/

void readLine(const char *prompt, char *buffer, size_t size)
{
    int ch;

    if (prompt != NULL)
        printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) != NULL)
    {
        size_t length = strcspn(buffer, "\n");

        if (buffer[length] == '\n')
        {
            buffer[length] = '\0';
        }
        else
        {
            while ((ch = getchar()) != '\n' && ch != EOF)
            {
                /* Clear extra input */
            }
        }
    }
    else
    {
        buffer[0] = '\0';
    }
}

int readInt(const char *prompt, int minimum, int maximum)
{
    char input[64];
    char extra;
    int number;

    while (1)
    {
        readLine(prompt, input, sizeof(input));

        if (sscanf(input, "%d %c", &number, &extra) == 1 &&
            number >= minimum &&
            number <= maximum)
        {
            return number;
        }

        printf("Invalid input. Enter a number from %d to %d.\n",
               minimum, maximum);
    }
}

void convertToUpper(char *text)
{
    while (*text)
    {
        *text = (char)toupper((unsigned char)*text);
        text++;
    }
}

/*--------------SYSTEM LOGGING------------*/

void logEvent(const char *message)
{
    FILE *file = fopen(LOG_FILE, "a");

    if (file == NULL)
        return;

    time_t currentTime = time(NULL);
    struct tm *timeInfo = localtime(&currentTime);

    if (timeInfo != NULL)
    {
        fprintf(file,
                "[%04d-%02d-%02d %02d:%02d:%02d] %s\n",

                timeInfo->tm_year + 1900,
                timeInfo->tm_mon + 1,
                timeInfo->tm_mday,
                timeInfo->tm_hour,
                timeInfo->tm_min,
                timeInfo->tm_sec,
                message);
    }

    fclose(file);
}

/*-----------GENERIC FILE HANDLING----------*/

void loadArray(
    const char *filename,
    void *array,
    size_t recordSize,
    int maximumRecords,
    int *count)
{
    FILE *file = fopen(filename, "rb");

    if (file == NULL)
    {
        *count = 0;
        return;
    }

    *count = (int)fread(
        array,
        recordSize,
        (size_t)maximumRecords,
        file);

    fclose(file);
}

void saveArray(
    const char *filename,
    const void *array,
    size_t recordSize,
    int count)
{
    FILE *file = fopen(filename, "wb");

    if (file == NULL)
    {
        printf("Warning: Could not save %s\n", filename);
        return;
    }

    fwrite(
        array,
        recordSize,
        (size_t)count,
        file);

    fclose(file);
}

/*------------INITIAL AIRPORT RESOURCES-------------*/

void initializeResources(void)
{
    int i;

    /*
       If no gate file exists,
       create 8 default airport gates.
    */

    if (gateCount == 0)
    {
        gateCount = 8;

        for (i = 0; i < gateCount; i++)
        {
            snprintf(
                gates[i].id,
                sizeof(gates[i].id),
                "G%02d",
                i + 1);

            strcpy(gates[i].status, "AVAILABLE");

            gates[i].flightNo[0] = '\0';
        }
    }

    /*
       If no runway file exists,
       create 3 default runways.
    */

    if (runwayCount == 0)
    {
        runwayCount = 3;

        for (i = 0; i < runwayCount; i++)
        {
            snprintf(
                runways[i].id,
                sizeof(runways[i].id),
                "R%02d",
                i + 1);

            strcpy(runways[i].status, "AVAILABLE");

            runways[i].flightNo[0] = '\0';
        }
    }
}

/*---------LOAD ALL DATA-----------*/

void loadData(void)
{
    loadArray(
        FLIGHT_FILE,
        flights,
        sizeof(Flight),
        MAX_FLIGHTS,
        &flightCount);

    loadArray(
        AIRCRAFT_FILE,
        aircraft,
        sizeof(Aircraft),
        MAX_AIRCRAFT,
        &aircraftCount);

    loadArray(
        PASSENGER_FILE,
        passengers,
        sizeof(Passenger),
        MAX_PASSENGERS,
        &passengerCount);

    loadArray(
        BAGGAGE_FILE,
        baggage,
        sizeof(Baggage),
        MAX_BAGGAGE,
        &baggageCount);

    loadArray(
        GATE_FILE,
        gates,
        sizeof(Gate),
        MAX_GATES,
        &gateCount);

    loadArray(
        RUNWAY_FILE,
        runways,
        sizeof(Runway),
        MAX_RUNWAYS,
        &runwayCount);

    loadArray(
        ALERT_FILE,
        alerts,
        sizeof(Alert),
        MAX_ALERTS,
        &alertCount);

    initializeResources();
}

/*-------------SAVE ALL DATA---------- */

void saveData(void)
{
    saveArray(
        FLIGHT_FILE,
        flights,
        sizeof(Flight),
        flightCount);

    saveArray(
        AIRCRAFT_FILE,
        aircraft,
        sizeof(Aircraft),
        aircraftCount);

    saveArray(
        PASSENGER_FILE,
        passengers,
        sizeof(Passenger),
        passengerCount);

    saveArray(
        BAGGAGE_FILE,
        baggage,
        sizeof(Baggage),
        baggageCount);

    saveArray(
        GATE_FILE,
        gates,
        sizeof(Gate),
        gateCount);

    saveArray(
        RUNWAY_FILE,
        runways,
        sizeof(Runway),
        runwayCount);

    saveArray(
        ALERT_FILE,
        alerts,
        sizeof(Alert),
        alertCount);
}

/*SEARCH FUNCTIONS*/

int findFlight(const char *flightNo)
{
    int i;

    for (i = 0; i < flightCount; i++)
    {
        if (strcmp(flights[i].flightNo, flightNo) == 0)
            return i;
    }

    return -1;
}

int findAircraft(const char *id)
{
    int i;

    for (i = 0; i < aircraftCount; i++)
    {
        if (strcmp(aircraft[i].id, id) == 0)
            return i;
    }

    return -1;
}

int findGate(const char *id)
{
    int i;

    for (i = 0; i < gateCount; i++)
    {
        if (strcmp(gates[i].id, id) == 0)
            return i;
    }

    return -1;
}

int findRunway(const char *id)
{
    int i;

    for (i = 0; i < runwayCount; i++)
    {
        if (strcmp(runways[i].id, id) == 0)
            return i;
    }

    return -1;
}

int findPassenger(int id)
{
    int i;

    for (i = 0; i < passengerCount; i++)
    {
        if (passengers[i].id == id)
            return i;
    }

    return -1;
}

int findBaggage(int id)
{
    int i;

    for (i = 0; i < baggageCount; i++)
    {
        if (baggage[i].id == id)
            return i;
    }

    return -1;
}

/*FLIGHT MANAGEMENT*/

void viewFlights(void)
{
    int i;

    if (flightCount == 0)
    {
        printf("No flight records available.\n");
        return;
    }

    printf("\n%-10s %-14s %-14s %-14s %-8s %-14s %-8s %-8s\n",
           "FLIGHT",
           "AIRLINE",
           "ORIGIN",
           "DESTINATION",
           "TIME",
           "STATUS",
           "GATE",
           "RUNWAY");

    for (i = 0; i < flightCount; i++)
    {
        printf("%-10s %-14s %-14s %-14s %-8s %-14s %-8s %-8s\n",
               flights[i].flightNo,
               flights[i].airline,
               flights[i].origin,
               flights[i].destination,
               flights[i].time,
               flights[i].status,

               flights[i].gateId[0]
                   ? flights[i].gateId
                   : "-",

               flights[i].runwayId[0]
                   ? flights[i].runwayId
                   : "-");
    }
}

void addFlight(void)
{
    Flight newFlight = {0};

    if (flightCount >= MAX_FLIGHTS)
    {
        printf("Flight storage is full.\n");
        return;
    }

    readLine(
        "Enter flight number: ",
        newFlight.flightNo,
        sizeof(newFlight.flightNo));

    convertToUpper(newFlight.flightNo);

    if (newFlight.flightNo[0] == '\0')
    {
        printf("Flight number cannot be empty.\n");
        return;
    }

    if (findFlight(newFlight.flightNo) >= 0)
    {
        printf("Flight already exists.\n");
        return;
    }

    readLine(
        "Enter airline: ",
        newFlight.airline,
        sizeof(newFlight.airline));

    readLine(
        "Enter origin: ",
        newFlight.origin,
        sizeof(newFlight.origin));

    readLine(
        "Enter destination: ",
        newFlight.destination,
        sizeof(newFlight.destination));

    readLine(
        "Enter scheduled time (HH:MM): ",
        newFlight.time,
        sizeof(newFlight.time));

    strcpy(newFlight.status, "ON TIME");

    flights[flightCount] = newFlight;

    flightCount++;

    saveData();

    logEvent("New flight added");

    printf("Flight added successfully.\n");
}

void updateFlight(void)
{
    char flightNo[12];

    readLine(
        "Enter flight number: ",
        flightNo,
        sizeof(flightNo));

    convertToUpper(flightNo);

    int index = findFlight(flightNo);

    if (index < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    printf("\n1. Update Status\n");
    printf("2. Update Scheduled Time\n");
    printf("3. Update Destination\n");

    int choice = readInt("Enter choice: ", 1, 3);

    if (choice == 1)
    {
        readLine(
            "Enter new status: ",
            flights[index].status,
            sizeof(flights[index].status));
    }
    else if (choice == 2)
    {
        readLine(
            "Enter new time: ",
            flights[index].time,
            sizeof(flights[index].time));
    }
    else
    {
        readLine(
            "Enter new destination: ",
            flights[index].destination,
            sizeof(flights[index].destination));
    }

    saveData();

    logEvent("Flight record updated");

    printf("Flight updated successfully.\n");
}

void releaseFlightResources(const char *flightNo)
{
    int i;

    for (i = 0; i < gateCount; i++)
    {
        if (strcmp(gates[i].flightNo, flightNo) == 0)
        {
            strcpy(gates[i].status, "AVAILABLE");

            gates[i].flightNo[0] = '\0';
        }
    }

    for (i = 0; i < runwayCount; i++)
    {
        if (strcmp(runways[i].flightNo, flightNo) == 0)
        {
            strcpy(runways[i].status, "AVAILABLE");

            runways[i].flightNo[0] = '\0';
        }
    }
}

void deleteFlight(void)
{
    char flightNo[12];

    int i;
    int j;

    readLine(
        "Enter flight number: ",
        flightNo,
        sizeof(flightNo));

    convertToUpper(flightNo);

    int index = findFlight(flightNo);

    if (index < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    /*
       Do not delete a flight if passengers
       are still connected to it.
    */

    for (i = 0; i < passengerCount; i++)
    {
        if (strcmp(passengers[i].flightNo, flightNo) == 0)
        {
            printf(
                "Cannot delete flight. "
                "Passengers are linked to it.\n");

            return;
        }
    }

    /*
       Do not delete a flight if baggage
       records are still connected.
    */

    for (i = 0; i < baggageCount; i++)
    {
        if (strcmp(baggage[i].flightNo, flightNo) == 0)
        {
            printf(
                "Cannot delete flight. "
                "Baggage is linked to it.\n");

            return;
        }
    }

    releaseFlightResources(flightNo);

    for (j = index; j < flightCount - 1; j++)
    {
        flights[j] = flights[j + 1];
    }

    flightCount--;

    saveData();

    logEvent("Flight deleted");

    printf("Flight deleted successfully.\n");
}

void searchFlight(void)
{
    char flightNo[12];

    readLine(
        "Enter flight number: ",
        flightNo,
        sizeof(flightNo));

    convertToUpper(flightNo);

    int index = findFlight(flightNo);

    if (index < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    printf("\nFlight Number : %s\n",
           flights[index].flightNo);

    printf("Airline       : %s\n",
           flights[index].airline);

    printf("Route         : %s -> %s\n",
           flights[index].origin,
           flights[index].destination);

    printf("Time          : %s\n",
           flights[index].time);

    printf("Status        : %s\n",
           flights[index].status);
}

void flightMenu(void)
{
    while (1)
    {
        printf("\n===== FLIGHT MANAGEMENT =====\n");

        printf("1. Add Flight\n");
        printf("2. View Flights\n");
        printf("3. Update Flight\n");
        printf("4. Delete Flight\n");
        printf("5. Search Flight\n");
        printf("0. Back\n");

        int choice = readInt(
            "Enter choice: ",
            0,
            5);

        switch (choice)
        {
        case 1:
            addFlight();
            break;

        case 2:
            viewFlights();
            break;

        case 3:
            updateFlight();
            break;

        case 4:
            deleteFlight();
            break;

        case 5:
            searchFlight();
            break;

        case 0:
            return;
        }
    }
}

/*AIRCRAFT MANAGEMENT*/

void viewAircraft(void)
{
    int i;

    if (aircraftCount == 0)
    {
        printf("No aircraft records available.\n");
        return;
    }

    printf("\n%-10s %-20s %-10s %-14s\n",
           "ID",
           "MODEL",
           "CAPACITY",
           "STATUS");

    for (i = 0; i < aircraftCount; i++)
    {
        printf("%-10s %-20s %-10d %-14s\n",
               aircraft[i].id,
               aircraft[i].model,
               aircraft[i].capacity,
               aircraft[i].status);
    }
}

void addAircraft(void)
{
    Aircraft newAircraft = {0};

    if (aircraftCount >= MAX_AIRCRAFT)
    {
        printf("Aircraft storage is full.\n");
        return;
    }

    readLine(
        "Enter aircraft ID: ",
        newAircraft.id,
        sizeof(newAircraft.id));

    convertToUpper(newAircraft.id);

    if (newAircraft.id[0] == '\0' ||
        findAircraft(newAircraft.id) >= 0)
    {
        printf("Invalid or duplicate aircraft ID.\n");
        return;
    }

    readLine(
        "Enter aircraft model: ",
        newAircraft.model,
        sizeof(newAircraft.model));

    newAircraft.capacity =
        readInt("Enter passenger capacity: ", 1, 1000);

    strcpy(newAircraft.status, "AVAILABLE");

    aircraft[aircraftCount] = newAircraft;

    aircraftCount++;

    saveData();

    logEvent("Aircraft added");

    printf("Aircraft added successfully.\n");
}

void updateAircraft(void)
{
    char id[12];

    readLine(
        "Enter aircraft ID: ",
        id,
        sizeof(id));

    convertToUpper(id);

    int index = findAircraft(id);

    if (index < 0)
    {
        printf("Aircraft not found.\n");
        return;
    }

    readLine(
        "Enter new aircraft status: ",
        aircraft[index].status,
        sizeof(aircraft[index].status));

    saveData();

    printf("Aircraft updated successfully.\n");
}

void deleteAircraft(void)
{
    char id[12];

    int i;

    readLine(
        "Enter aircraft ID: ",
        id,
        sizeof(id));

    convertToUpper(id);

    int index = findAircraft(id);

    if (index < 0)
    {
        printf("Aircraft not found.\n");
        return;
    }

    /*
       Aircraft cannot be deleted if
       currently connected to a flight.
    */

    for (i = 0; i < flightCount; i++)
    {
        if (strcmp(
                flights[i].aircraftId,
                id) == 0)
        {
            printf(
                "Cannot delete aircraft. "
                "It is assigned to a flight.\n");

            return;
        }
    }

    for (i = index; i < aircraftCount - 1; i++)
    {
        aircraft[i] = aircraft[i + 1];
    }

    aircraftCount--;

    saveData();

    printf("Aircraft deleted successfully.\n");
}

void assignAircraft(void)
{
    char flightNo[12];
    char aircraftId[12];

    int i;

    readLine(
        "Enter flight number: ",
        flightNo,
        sizeof(flightNo));

    convertToUpper(flightNo);

    int flightIndex = findFlight(flightNo);

    if (flightIndex < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    readLine(
        "Enter aircraft ID: ",
        aircraftId,
        sizeof(aircraftId));

    convertToUpper(aircraftId);

    int aircraftIndex = findAircraft(aircraftId);

    if (aircraftIndex < 0)
    {
        printf("Aircraft not found.\n");
        return;
    }

    /*
       Check whether the aircraft has already
       been assigned to another flight.
    */

    for (i = 0; i < flightCount; i++)
    {
        if (i != flightIndex &&
            strcmp(
                flights[i].aircraftId,
                aircraftId) == 0)
        {
            printf(
                "Aircraft is already assigned "
                "to another flight.\n");

            return;
        }
    }

    strcpy(
        flights[flightIndex].aircraftId,
        aircraftId);

    strcpy(
        aircraft[aircraftIndex].status,
        "ASSIGNED");

    saveData();

    printf("Aircraft assigned successfully.\n");
}

void aircraftMenu(void)
{
    while (1)
    {
        printf("\n===== AIRCRAFT MANAGEMENT =====\n");

        printf("1. Add Aircraft\n");
        printf("2. View Aircraft\n");
        printf("3. Update Aircraft Status\n");
        printf("4. Delete Aircraft\n");
        printf("5. Assign Aircraft to Flight\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 5);

        switch (choice)
        {
        case 1:
            addAircraft();
            break;

        case 2:
            viewAircraft();
            break;

        case 3:
            updateAircraft();
            break;

        case 4:
            deleteAircraft();
            break;

        case 5:
            assignAircraft();
            break;

        case 0:
            return;
        }
    }
}

/*GATE MANAGEMENT*/

void viewGates(void)
{
    int i;

    printf("\n%-8s %-12s %-12s\n",
           "GATE",
           "STATUS",
           "FLIGHT");

    for (i = 0; i < gateCount; i++)
    {
        printf("%-8s %-12s %-12s\n",
               gates[i].id,
               gates[i].status,

               gates[i].flightNo[0]
                   ? gates[i].flightNo
                   : "-");
    }
}

void assignGate(void)
{
    char gateId[8];
    char flightNo[12];

    readLine(
        "Enter gate ID: ",
        gateId,
        sizeof(gateId));

    convertToUpper(gateId);

    int gateIndex = findGate(gateId);

    if (gateIndex < 0)
    {
        printf("Gate not found.\n");
        return;
    }

    if (strcmp(
            gates[gateIndex].status,
            "AVAILABLE") != 0)
    {
        printf("Gate is not available.\n");
        return;
    }

    readLine(
        "Enter flight number: ",
        flightNo,
        sizeof(flightNo));

    convertToUpper(flightNo);

    int flightIndex = findFlight(flightNo);

    if (flightIndex < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    if (flights[flightIndex].gateId[0] != '\0')
    {
        printf(
            "This flight already has a gate.\n");

        return;
    }

    strcpy(
        gates[gateIndex].status,
        "OCCUPIED");

    strcpy(
        gates[gateIndex].flightNo,
        flightNo);

    strcpy(
        flights[flightIndex].gateId,
        gateId);

    saveData();

    printf("Gate assigned successfully.\n");
}

void releaseGate(void)
{
    char gateId[8];

    readLine(
        "Enter gate ID: ",
        gateId,
        sizeof(gateId));

    convertToUpper(gateId);

    int gateIndex = findGate(gateId);

    if (gateIndex < 0)
    {
        printf("Gate not found.\n");
        return;
    }

    if (gates[gateIndex].flightNo[0] == '\0')
    {
        printf("Gate is already available.\n");
        return;
    }

    int flightIndex =
        findFlight(gates[gateIndex].flightNo);

    if (flightIndex >= 0)
    {
        flights[flightIndex].gateId[0] = '\0';
    }

    gates[gateIndex].flightNo[0] = '\0';

    strcpy(
        gates[gateIndex].status,
        "AVAILABLE");

    saveData();

    printf("Gate released successfully.\n");
}

void gateMenu(void)
{
    while (1)
    {
        printf("\n===== GATE MANAGEMENT =====\n");

        printf("1. View Gates\n");
        printf("2. Assign Gate\n");
        printf("3. Release Gate\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 3);

        switch (choice)
        {
        case 1:
            viewGates();
            break;

        case 2:
            assignGate();
            break;

        case 3:
            releaseGate();
            break;

        case 0:
            return;
        }
    }
}

/*RUNWAY MANAGEMENT*/

void viewRunways(void)
{
    int i;

    printf("\n%-8s %-14s %-12s\n",
           "RUNWAY",
           "STATUS",
           "FLIGHT");

    for (i = 0; i < runwayCount; i++)
    {
        printf("%-8s %-14s %-12s\n",
               runways[i].id,
               runways[i].status,

               runways[i].flightNo[0]
                   ? runways[i].flightNo
                   : "-");
    }
}

void assignRunway(void)
{
    char runwayId[8];
    char flightNo[12];

    readLine(
        "Enter runway ID: ",
        runwayId,
        sizeof(runwayId));

    convertToUpper(runwayId);

    int runwayIndex = findRunway(runwayId);

    if (runwayIndex < 0)
    {
        printf("Runway not found.\n");
        return;
    }

    if (strcmp(
            runways[runwayIndex].status,
            "AVAILABLE") != 0)
    {
        printf("Runway is not available.\n");
        return;
    }

    readLine(
        "Enter flight number: ",
        flightNo,
        sizeof(flightNo));

    convertToUpper(flightNo);

    int flightIndex = findFlight(flightNo);

    if (flightIndex < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    if (flights[flightIndex].runwayId[0] != '\0')
    {
        printf(
            "This flight already has a runway.\n");

        return;
    }

    strcpy(
        runways[runwayIndex].status,
        "OCCUPIED");

    strcpy(
        runways[runwayIndex].flightNo,
        flightNo);

    strcpy(
        flights[flightIndex].runwayId,
        runwayId);

    saveData();

    printf("Runway assigned successfully.\n");
}

void releaseRunway(void)
{
    char runwayId[8];

    readLine(
        "Enter runway ID: ",
        runwayId,
        sizeof(runwayId));

    convertToUpper(runwayId);

    int runwayIndex = findRunway(runwayId);

    if (runwayIndex < 0)
    {
        printf("Runway not found.\n");
        return;
    }

    if (runways[runwayIndex].flightNo[0] == '\0')
    {
        printf("Runway is already available.\n");
        return;
    }

    int flightIndex =
        findFlight(runways[runwayIndex].flightNo);

    if (flightIndex >= 0)
    {
        flights[flightIndex].runwayId[0] = '\0';
    }

    runways[runwayIndex].flightNo[0] = '\0';

    strcpy(
        runways[runwayIndex].status,
        "AVAILABLE");

    saveData();

    printf("Runway released successfully.\n");
}

void toggleRunwayMaintenance(void)
{
    char runwayId[8];

    readLine(
        "Enter runway ID: ",
        runwayId,
        sizeof(runwayId));

    convertToUpper(runwayId);

    int index = findRunway(runwayId);

    if (index < 0)
    {
        printf("Runway not found.\n");
        return;
    }

    /*
       An occupied runway cannot be
       placed under maintenance.
    */

    if (runways[index].flightNo[0] != '\0')
    {
        printf(
            "Release the assigned flight first.\n");

        return;
    }

    if (strcmp(
            runways[index].status,
            "MAINTENANCE") == 0)
    {
        strcpy(
            runways[index].status,
            "AVAILABLE");
    }
    else
    {
        strcpy(
            runways[index].status,
            "MAINTENANCE");
    }

    saveData();

    printf("Runway status updated.\n");
}

void runwayMenu(void)
{
    while (1)
    {
        printf("\n===== RUNWAY MANAGEMENT =====\n");

        printf("1. View Runways\n");
        printf("2. Assign Runway\n");
        printf("3. Release Runway\n");
        printf("4. Toggle Maintenance\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 4);

        switch (choice)
        {
        case 1:
            viewRunways();
            break;

        case 2:
            assignRunway();
            break;

        case 3:
            releaseRunway();
            break;

        case 4:
            toggleRunwayMaintenance();
            break;

        case 0:
            return;
        }
    }
}

/* =========================================================
   PASSENGER MANAGEMENT
   ========================================================= */

void viewPassengers(void)
{
    int i;

    if (passengerCount == 0)
    {
        printf("No passenger records available.\n");
        return;
    }

    printf("\n%-8s %-25s %-10s %-10s\n",
           "ID",
           "NAME",
           "FLIGHT",
           "CHECK-IN");

    for (i = 0; i < passengerCount; i++)
    {
        printf("%-8d %-25s %-10s %-10s\n",
               passengers[i].id,
               passengers[i].name,
               passengers[i].flightNo,

               passengers[i].checkedIn
                   ? "YES"
                   : "NO");
    }
}

void addPassenger(void)
{
    Passenger newPassenger = {0};

    if (passengerCount >= MAX_PASSENGERS)
    {
        printf("Passenger storage is full.\n");
        return;
    }

    newPassenger.id =
        readInt("Enter passenger ID: ", 1, 999999);

    if (findPassenger(newPassenger.id) >= 0)
    {
        printf("Passenger ID already exists.\n");
        return;
    }

    readLine(
        "Enter passenger name: ",
        newPassenger.name,
        sizeof(newPassenger.name));

    readLine(
        "Enter flight number: ",
        newPassenger.flightNo,
        sizeof(newPassenger.flightNo));

    convertToUpper(newPassenger.flightNo);

    if (findFlight(newPassenger.flightNo) < 0)
    {
        printf("Flight not found.\n");
        return;
    }

    newPassenger.checkedIn = 0;

    passengers[passengerCount] = newPassenger;

    passengerCount++;

    saveData();

    printf("Passenger added successfully.\n");
}

void updatePassenger(void)
{
    int id =
        readInt("Enter passenger ID: ", 1, 999999);

    int index = findPassenger(id);

    if (index < 0)
    {
        printf("Passenger not found.\n");
        return;
    }

    printf("\n1. Change Name\n");
    printf("2. Change Flight\n");
    printf("3. Toggle Check-in\n");

    int choice =
        readInt("Enter choice: ", 1, 3);

    if (choice == 1)
    {
        readLine(
            "Enter new name: ",
            passengers[index].name,
            sizeof(passengers[index].name));
    }

    else if (choice == 2)
    {
        char newFlight[12];

        readLine(
            "Enter new flight number: ",
            newFlight,
            sizeof(newFlight));

        convertToUpper(newFlight);

        if (findFlight(newFlight) < 0)
        {
            printf("Flight not found.\n");
            return;
        }

        strcpy(
            passengers[index].flightNo,
            newFlight);

        /*
           If the passenger has baggage,
           its flight must also change.
        */

        int i;

        for (i = 0; i < baggageCount; i++)
        {
            if (baggage[i].passengerId == id)
            {
                strcpy(
                    baggage[i].flightNo,
                    newFlight);
            }
        }
    }

    else
    {
        passengers[index].checkedIn =
            !passengers[index].checkedIn;
    }

    saveData();

    printf("Passenger updated successfully.\n");
}

void searchPassenger(void)
{
    int id =
        readInt("Enter passenger ID: ", 1, 999999);

    int index = findPassenger(id);

    if (index < 0)
    {
        printf("Passenger not found.\n");
        return;
    }

    printf("\nPassenger ID : %d\n",
           passengers[index].id);

    printf("Name         : %s\n",
           passengers[index].name);

    printf("Flight       : %s\n",
           passengers[index].flightNo);

    printf("Checked In   : %s\n",
           passengers[index].checkedIn
               ? "YES"
               : "NO");
}

void passengerMenu(void)
{
    while (1)
    {
        printf("\n===== PASSENGER MANAGEMENT =====\n");

        printf("1. Add Passenger\n");
        printf("2. View Passengers\n");
        printf("3. Update Passenger\n");
        printf("4. Search Passenger\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 4);

        switch (choice)
        {
        case 1:
            addPassenger();
            break;

        case 2:
            viewPassengers();
            break;

        case 3:
            updatePassenger();
            break;

        case 4:
            searchPassenger();
            break;

        case 0:
            return;
        }
    }
}

/* =========================================================
   BAGGAGE TRACKING
   ========================================================= */

void viewBaggage(void)
{
    int i;

    if (baggageCount == 0)
    {
        printf("No baggage records available.\n");
        return;
    }

    printf("\n%-8s %-12s %-10s %-18s\n",
           "BAG ID",
           "PASSENGER",
           "FLIGHT",
           "STATUS");

    for (i = 0; i < baggageCount; i++)
    {
        printf("%-8d %-12d %-10s %-18s\n",
               baggage[i].id,
               baggage[i].passengerId,
               baggage[i].flightNo,
               baggage[i].status);
    }
}

void addBaggage(void)
{
    Baggage newBag = {0};

    if (baggageCount >= MAX_BAGGAGE)
    {
        printf("Baggage storage is full.\n");
        return;
    }

    newBag.id =
        readInt("Enter baggage ID: ", 1, 999999);

    if (findBaggage(newBag.id) >= 0)
    {
        printf("Baggage ID already exists.\n");
        return;
    }

    newBag.passengerId =
        readInt("Enter passenger ID: ", 1, 999999);

    int passengerIndex =
        findPassenger(newBag.passengerId);

    if (passengerIndex < 0)
    {
        printf("Passenger not found.\n");
        return;
    }

    strcpy(
        newBag.flightNo,
        passengers[passengerIndex].flightNo);

    strcpy(
        newBag.status,
        "CHECK-IN");

    baggage[baggageCount] = newBag;

    baggageCount++;

    saveData();

    printf("Baggage added successfully.\n");
}

void updateBaggage(void)
{
    int id =
        readInt("Enter baggage ID: ", 1, 999999);

    int index = findBaggage(id);

    if (index < 0)
    {
        printf("Baggage not found.\n");
        return;
    }

    printf("\n1. SECURITY\n");
    printf("2. SORTING\n");
    printf("3. LOADED\n");
    printf("4. ARRIVED\n");
    printf("5. MISSING\n");

    int choice =
        readInt("Select new status: ", 1, 5);

    const char *statusList[] =
    {
        "",
        "SECURITY",
        "SORTING",
        "LOADED",
        "ARRIVED",
        "MISSING"
    };

    strcpy(
        baggage[index].status,
        statusList[choice]);

    saveData();

    printf("Baggage status updated.\n");
}

void searchBaggage(void)
{
    int id =
        readInt("Enter baggage ID: ", 1, 999999);

    int index = findBaggage(id);

    if (index < 0)
    {
        printf("Baggage not found.\n");
        return;
    }

    printf("\nBaggage ID  : %d\n",
           baggage[index].id);

    printf("Passenger ID: %d\n",
           baggage[index].passengerId);

    printf("Flight      : %s\n",
           baggage[index].flightNo);

    printf("Status      : %s\n",
           baggage[index].status);
}

void baggageMenu(void)
{
    while (1)
    {
        printf("\n===== BAGGAGE TRACKING =====\n");

        printf("1. Add Baggage\n");
        printf("2. View Baggage\n");
        printf("3. Update / Track Baggage\n");
        printf("4. Search Baggage\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 4);

        switch (choice)
        {
        case 1:
            addBaggage();
            break;

        case 2:
            viewBaggage();
            break;

        case 3:
            updateBaggage();
            break;

        case 4:
            searchBaggage();
            break;

        case 0:
            return;
        }
    }
}

/*SECURITY SYSTEM*/

void viewAlerts(void)
{
    int i;

    if (alertCount == 0)
    {
        printf("No security alerts available.\n");
        return;
    }

    printf("\n%-5s %-15s %-50s %-12s\n",
           "ID",
           "TYPE",
           "MESSAGE",
           "STATUS");

    for (i = 0; i < alertCount; i++)
    {
        printf("%-5d %-15s %-50s %-12s\n",
               alerts[i].id,
               alerts[i].type,
               alerts[i].message,
               alerts[i].status);
    }
}

void addAlert(
    const char *type,
    const char *message)
{
    int i;
    int maximumId = 0;

    if (alertCount >= MAX_ALERTS)
        return;

    for (i = 0; i < alertCount; i++)
    {
        if (alerts[i].id > maximumId)
        {
            maximumId = alerts[i].id;
        }
    }

    alerts[alertCount].id =
        maximumId + 1;

    snprintf(
        alerts[alertCount].type,
        sizeof(alerts[alertCount].type),
        "%s",
        type);

    snprintf(
        alerts[alertCount].message,
        sizeof(alerts[alertCount].message),
        "%s",
        message);

    strcpy(
        alerts[alertCount].status,
        "OPEN");

    alertCount++;
}

void securityScan(void)
{
    int result = rand() % 4;

    printf("\nRunning AERIS security scan...\n");

    if (result == 0)
    {
        addAlert(
            "SECURITY",
            "Suspicious baggage requires inspection.");

        printf(
            "Suspicious baggage detected.\n"
            "Security alert created.\n");
    }

    else if (result == 1)
    {
        addAlert(
            "NETWORK",
            "Unusual airport network activity detected.");

        printf(
            "Network anomaly detected.\n"
            "Security alert created.\n");
    }

    else
    {
        printf(
            "Scan completed. "
            "No new threats detected.\n");
    }

    saveData();
}

void resolveAlert(void)
{
    int i;

    int id =
        readInt("Enter alert ID: ", 1, 999999);

    for (i = 0; i < alertCount; i++)
    {
        if (alerts[i].id == id)
        {
            strcpy(
                alerts[i].status,
                "RESOLVED");

            saveData();

            printf("Alert resolved successfully.\n");

            return;
        }
    }

    printf("Alert not found.\n");
}

void securityMenu(void)
{
    while (1)
    {
        printf("\n===== SECURITY SYSTEM =====\n");

        printf("1. View Security Alerts\n");
        printf("2. Run Security Scan\n");
        printf("3. Resolve Alert\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 3);

        switch (choice)
        {
        case 1:
            viewAlerts();
            break;

        case 2:
            securityScan();
            break;

        case 3:
            resolveAlert();
            break;

        case 0:
            return;
        }
    }
}

/*AIRPORT EVENTS*/

void simulateAirportEvent(void)
{
    int event = rand() % 4;

    if (event == 0)
    {
        int i;

        printf(
            "\nEVENT: Heavy rainfall detected.\n"
            "Some flights have been delayed.\n");

        /*
           Delay alternate flights to simulate
           weather disruption.
        */

        for (i = 0; i < flightCount; i++)
        {
            if (i % 2 == 0)
            {
                strcpy(
                    flights[i].status,
                    "DELAYED");
            }
        }

        addAlert(
            "WEATHER",
            "Heavy rain affecting airport operations.");
    }

    else if (event == 1)
    {
        printf(
            "\nEVENT: Runway inspection required.\n");

        /*
           Select a random runway.
        */

        int runwayIndex =
            rand() % runwayCount;

        /*
           Only put the runway into maintenance
           if no aircraft is currently using it.
        */

        if (runways[runwayIndex].flightNo[0] == '\0')
        {
            strcpy(
                runways[runwayIndex].status,
                "MAINTENANCE");

            printf(
                "%s has entered maintenance mode.\n",
                runways[runwayIndex].id);
        }
        else
        {
            printf(
                "Selected runway is currently occupied.\n");
        }

        addAlert(
            "RUNWAY",
            "Routine runway inspection initiated.");
    }

    else if (event == 2)
    {
        printf(
            "\nEVENT: Baggage congestion reported.\n");

        addAlert(
            "BAGGAGE",
            "Baggage sorting area congestion reported.");
    }

    else
    {
        printf(
            "\nEVENT: Airport operations are normal.\n");
    }

    saveData();
}

void eventMenu(void)
{
    while (1)
    {
        printf(
            "\n===== AIRPORT EVENTS & ALERTS =====\n");

        printf("1. View Current Alerts\n");
        printf("2. Simulate Airport Event\n");
        printf("0. Back\n");

        int choice =
            readInt("Enter choice: ", 0, 2);

        switch (choice)
        {
        case 1:
            viewAlerts();
            break;

        case 2:
            simulateAirportEvent();
            break;

        case 0:
            return;
        }
    }
}

/*SYSTEM MONITORING*/

void systemMonitor(void)
{
    int i;

    int unavailableGates = 0;
    int unavailableRunways = 0;
    int openAlerts = 0;

    for (i = 0; i < gateCount; i++)
    {
        if (strcmp(
                gates[i].status,
                "AVAILABLE") != 0)
        {
            unavailableGates++;
        }
    }

    for (i = 0; i < runwayCount; i++)
    {
        if (strcmp(
                runways[i].status,
                "AVAILABLE") != 0)
        {
            unavailableRunways++;
        }
    }

    for (i = 0; i < alertCount; i++)
    {
        if (strcmp(
                alerts[i].status,
                "OPEN") == 0)
        {
            openAlerts++;
        }
    }

    printf(
        "\n=====================================\n");

    printf(
        "         AERIS SYSTEM MONITOR\n");

    printf(
        "=====================================\n");

    printf(
        "Flights                : %d\n",
        flightCount);

    printf(
        "Aircraft               : %d\n",
        aircraftCount);

    printf(
        "Passengers             : %d\n",
        passengerCount);

    printf(
        "Baggage Records        : %d\n",
        baggageCount);

    printf(
        "Gates Unavailable      : %d / %d\n",
        unavailableGates,
        gateCount);

    printf(
        "Runways Unavailable    : %d / %d\n",
        unavailableRunways,
        runwayCount);

    printf(
        "Open Alerts            : %d\n",
        openAlerts);

    printf(
        "=====================================\n");
}


/* =========================================================
   WELCOME SCREEN
   ========================================================= */

void displayWelcomeScreen(void)
{
    printf(
        "\n"
        "==============================================\n"
        "                                              \n"
        "       AERIS - AIRPORT OPERATING SYSTEM       \n"
        "                                              \n"
        "==============================================\n");

    printf(
        "Initializing Airport Systems...\n\n");

    printf("Flight Control ............ ONLINE\n");
    printf("Aircraft System ........... ONLINE\n");
    printf("Gate Management ........... ONLINE\n");
    printf("Runway Control ............ ONLINE\n");
    printf("Passenger System .......... ONLINE\n");
    printf("Baggage Tracking .......... ONLINE\n");
    printf("Security System ........... ONLINE\n");

    printf(
        "\nAERIS initialized successfully.\n");
}

/*MAIN PROGRAM*/

int main(void)
{
    /*
       Initialize random number generator.
    */

    srand((unsigned int)time(NULL));

    /*
       Load previous airport records.
    */

    loadData();

    /*
       Add startup entry to system log.
    */

    logEvent("AERIS system started");

    /*
       Display startup screen.
    */

    displayWelcomeScreen();

    /*
       Main AERIS loop.
    */

    while (1)
    {
        printf(
            "\n"
            "==============================================\n"
            "                AERIS MAIN MENU               \n"
            "==============================================\n");

        printf("1.  Flight Management\n");
        printf("2.  Aircraft Management\n");
        printf("3.  Gate Management\n");
        printf("4.  Runway Management\n");
        printf("5.  Passenger Management\n");
        printf("6.  Baggage Tracking\n");
        printf("7.  Security System\n");
        printf("8.  Airport Events & Alerts\n");
        printf("9.  System Monitoring\n");
        printf("10. Calculator\n");
        printf("0.  Save & Exit\n");

        printf(
            "==============================================\n");

        int choice =
            readInt("Select option: ", 0, 10);

        switch (choice)
        {
        case 1:

            flightMenu();

            break;

        case 2:

            aircraftMenu();

            break;

        case 3:

            gateMenu();

            break;

        case 4:

            runwayMenu();

            break;

        case 5:

            passengerMenu();

            break;

        case 6:

            baggageMenu();

            break;

        case 7:

            securityMenu();

            break;

        case 8:

            eventMenu();

            break;

        case 9:

            systemMonitor();

            break;

        case 0:

            /*
               Save everything before exiting.
            */

            saveData();

            logEvent(
                "AERIS system shut down");

            printf(
                "\nSaving airport data...\n");

            printf(
                "All data saved successfully.\n");

            printf(
                "\nAERIS shutting down...\n");

            printf(
                "Goodbye.\n");

            return 0;
        }
    }
}