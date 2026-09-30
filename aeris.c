# include<stdio.h>
# include<stdlib.h>
# include<time.h>
# include<string.h>
# include<ctype.h>

/*-----------------LIMITS--------------------*/

#define MAX_FLIGHTS 100
#define MAX_AIRCRAFT 100
#define MAX_GATES 20
#define MAX_RUNWAYS 5
#define MAX_PASSENGERS 300
#define MAX_BAGGAGE 400
#define MAX_ALERTS 100

/*-----------------FILES---------------------*/

#define FLIGHT_FILE "flights.dat"
#define AIRCRAFT_FILE "aircraft.dat"
#define PASSENGER_FILE "passengers.dat"
#define BAGGAGE_FILE "baggage.dat"
#define GATE_FILE "gates.dat"
#define RUNWAY_FILE "runways.dat"
#define ALERTS_FILE "alerts.dat"
#define LOG_FILE "aeris.log"

/*-----------------STRCUTURES-----------------*/

typedef struct{
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

typedef struct{
    char id[12];
    char model[30];
    int capacity;
    char status[20];
} Aircraft;

typedef struct{
    char id[8];
    char status[15];
    char flightNo[12];
} Gate;

typedef struct{
    char id[8];
    char status[15];
    char flightNo[12];
} Runway;

typedef struct{
    int id;
    char name[50];
    char flightNo[12];
    int checkedIn;
} Passenger;

typedef struct{
    int id;
    int passengerId;
    char flightNo[12];
    char status[20];
} Baggage;

typedef struct{
    int id;
    char type[20];
    char message[100];
    char status[15];
} Alert;

/*-----------------GLOBAL ARRAYS-------------------*/

Flight flights[MAX_FLIGHTS];
Aircraft aircraft[MAX_AIRCRAFT];
Gate gates[MAX_GATES];
Runway runways[MAX_RUNWAYS];
Passenger passengers[MAX_PASSENGERS];
Baggage baggage[MAX_BAGGAGE];
Alert alerts[MAX_ALERTS];

/* Record Counters */

int flightCount = 0;
int aircraftCount = 0;
int gateCount = 0;
int runwayCount = 0;
int passengerCount = 0;
int baggageCount = 0;
int alertCount = 0;
