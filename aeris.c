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

/*-------------INPUT FUNCTIONS----------------*/
void readLine(const char *prompt, char *buffer, size_t size){
    int ch;
    if(prompt!=NULL){
        printf("%s", prompt);
    }
    if(fgets(buffer, (int)size, stdin) !=NULL){
        size_t length = strcspn(buffer, "\n");
        if(buffer[length] == '\n'){
            buffer[length] == '\0';
        }
        else{
            while((ch = getchar()) != '\n' && ch!=EOF)
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

int readInt(const char *prompt, int minimum, int maximum){
    char input[64];
    char extra;
    int number;

    while(1)
    {
        readLine(prompt, input, sizeof(input));
        if sscanf(input, "%d %c", &number, &extra == 1 && number>=minimum && number<=maximum)
        {
            return number;
        }
        printf("Invalid input. Enter a number from %d to %d.\n", minimum,maximum);
    }
}

void convertToUpper(char *text)
{
    while(*text)
    {
        *text = (char)toupper((unsigned char)*text);
        text++;
    }
}

/*----------------SYSTEM LOGGING-----------------*/

void logEvent(const char *message)
{
    FILE *file = fopen(LOG_FILE, "a");
    if(file == NULL){
        return;
    }

    time_t currentTime = time(NULL);
    struct tm *timeInfo = localtime(&currentTime);

    if (timeInfo!=NULL){
        fprintf(file,"[%04d-%02d-%02d %02d:%02d:%02d] %s\n",
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

/*----------FILE HANDLING------------*/

void loadArray(
    const char *filename,
    void *array,
    size_t recordSize,
    int maximumRecords,
    int *count)
{
    FILE *file = fopen(filename, "rb");

    if(file == NULL)
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
        printf("Warning: could not save %s\n", filename);
        return;
    }
    fwrite(
        array,
        recordSize,
        (size_t)count,
        file);
    fclose(file);
}

/* INITIAL AIRPORT RESOURCES */

void initializeResources(void)
{
    int i;
    /* when no gates exist, create 8 default gates */
    if(gateCount == 0)
    {
        gateCount = 8;
        for(i=0;i<gateCount;i++)
        {
            snprintf(
                gates[i].id,
                sizeof(gates[i].id),
                "G%02d",
                i+1);
            strcpy(gates[i].status, "AVAILABLE");
            gates[i].flightNo[0] = '\0';
        }
    }
    /* If no runway exists, create 3 runways */
    if(runwayCount == 0)
    {
        runwayCount = 3;
        for(i=0;i<runwayCount;i++)
        {
            snprintf(runways[i].id,
            sizeof(runways[i].id),
            "R%02d",
            i+1);
        strycpy(runways[i].status, "AVAILABLE");
        runways[i].flightNo[0] = '\0';
        }
    }
}

/*----------------LOAD ALL DATA----------------*/
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

/*------------SAVE ALL THE DATA-----------*/

void saveData(void)
{
    saveArray(
        FLIGHT_FILE,
        flights,
        sizeof(Flight),
        MAX_FLIGHTS,
        &flightCount);
    
    saveArray(
        AIRCRAFT_FILE,
        aircraft,
        sizeof(Aircraft),
        MAX_AIRCRAFT,
        &aircraftCount);
    
    saveArray(
        PASSENGER_FILE,
        passengers,
        sizeof(Passenger),
        MAX_PASSENGERS,
        &passengerCount);

    saveArray(
        BAGGAGE_FILE,
        baggage,
        sizeof(Baggage),
        MAX_BAGGAGE,
        &baggageCount);
    
    saveArray(
        GATE_FILE,
        gates,
        sizeof(Gate),
        MAX_GATES,
        &gateCount);
    
    saveArray(
        RUNWAY_FILE,
        runways,
        sizeof(Runway),
        MAX_RUNWAYS,
        &runwayCount);
    
    saveArray(
        ALERT_FILE,
        alerts,
        sizeof(Alert),
        MAX_ALERTS,
        &alertCount);
}

/*---------SEARCH FUNCTIONS------------*/

int findFlight(const char *flightNo)
{
    int i;
    for(i=0;i<flightCount;i++)
    {
        if(strcmp(flight[i].flightNo, flightNo) == 0)
            return i;
    }
    return -1;
}

int findAircraft(const char *id)
{
    int i;
    for(i=0;i<aircraftCount;i++)
    {
        if(strcmp(aircraft[i].id, id)==0)
            return i;
    }
    return -1;
}

int findGate(const char *id)
{
    int i;
    for(i=0;i<gateCount;i++)
    {
        if(strcmp(gates[i].id, id)==0)
            return i;
    }
    return -1;
}

int findRunway(const char *id)
{
    int i;
    for(i=0;i<runwayCount;i++)
    {
        if(strcmp(runways[i].id, id)==0)
            return i;
    }
    return -1;
}

int findPassenger(int id)
{
    int i;
    for(i=0;i<passengerCount;i++)
    {
        if(passengers[i].id == id)
            return i;
    }
    return -1;
}

int findBaggage(int id)
{
    int i;
    for(i=0;i<baggageCount;i++)
    {
        if(baggage[i].id == id)
            return i;
    }
    return -1;
}

/*---------FLIGHT MANAGEMENT-----------*/



