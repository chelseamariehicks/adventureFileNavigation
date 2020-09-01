/**********************************************************************************
 * Name: Chelsea Marie Hicks
 * ONID 931286984
 * Course: CS 344
 * Assignment: Assignment #1
 * Due Date: July 9, 2020 by 11:59 PM
 * 
 * Description: Program reads a directory of room files created by buildrooms and
 * acts as the interface for playing the adventure game. The player moves along a 
 * path from connected room to connected room and tries to reach the end.
 * 
 * Resources include: CS344 documents, Geeks for Geeks, and StackOverflow
***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

//Room struct containing room ID, name, room type, number of connections, and struct room
//for tracking connections between rooms
struct Room {
    int roomID;
    char name[9];
    char type[11];
    int numConnections;
    struct Room* roomsConnected[6];
    char* connections[6];
};

//Static list of rooms in the game
static struct Room* rooms[7];


//Function returns the name of the latest directory
//Function relies on provided code from CS344 Module 3
char* getLatestDirectory() {
    int mostRecent = -1;

    //Look for directories beginning with the known prefix
    char prefix[40] = "hicksche.rooms";

    //Initialize the latest dirctory name
    static char latestDirName[100];
    memset(latestDirName, '\0', sizeof(latestDirName));

    //Pointers to directory and files
    DIR* directory;
    struct dirent* readFile;

    //Stat struct for directory attributes
    struct stat dirAtt;

    //Open current directory to check subdirectories
    directory = opendir(".");

    //If successful, directory is opened and can loop through all subdirectories
    if(directory > 0) {
        while((readFile = readdir(directory)) != NULL) {
            //If directory begins with prefix get the attributes
            if(strstr(readFile->d_name, prefix) != NULL) {
                stat(readFile->d_name, &dirAtt);

                //If the directory's mod time is greater than what's stored, update 
                //current directory as the latest and update the time track mostRecent
                if((int) dirAtt.st_mtime > mostRecent) {
                    mostRecent = (int) dirAtt.st_mtime;

                    memset(latestDirName, '\0', sizeof(latestDirName));
                    strcpy(latestDirName, readFile->d_name);
                }
            }
        }
    }
    //Close directory
    closedir(directory);

    return latestDirName;
}

//Function reads room files and adds them to the static struct of rooms
//Ran into trouble using getline() and opted to use fgets. This
//StackOverflow page guided me a bit: 
//https://stackoverflow.com/questions/3501338/c-read-file-line-by-line
void readRoomFiles(char* directory) {
    char fileName[100];

    //Loop through the 7 room files and create them here for game play
    for(int i = 0; i < 7; i++) {
        //Dynamically allocate memory for a room
        struct Room* room = (struct Room*) malloc(sizeof(struct Room));

        //Create fileName and open that file
        sprintf(fileName, "./%s/room%d", directory, i+1);
        FILE* currFile = fopen(fileName, "r");

        //Go through all of the files
        if(currFile != NULL) {
            //Set the roomID and number of connections
            room->roomID = i;
            room->numConnections = 0;

            //Used to ensure entire file gets read
            bool contReading = true;

            while(contReading) {
                //Read a line from the file
                char readLine[40];
                fgets(readLine, 40, currFile);

                //Add name to room if line contains the room name
                if(strstr(readLine, "ROOM NAME: ") != NULL) {
                    sscanf(readLine, "ROOM NAME: %s\n", room->name);
                }
                //Add type to room if line contains room type
                else if(strstr(readLine, "ROOM TYPE: ") != NULL) {
                    sscanf(readLine, "ROOM TYPE: %s\n", room->type);
                    //Reset continue reading since last line of file reached
                    contReading = false;
                }
                else {
                    //Add a connection to room if line contains connection
                    char connection[10];
                    sscanf(readLine, "CONNECTION %*d: %s\n", connection);

                    //Allocate memory and store room name in connections for the room
                    room->connections[room->numConnections] = calloc(10, sizeof(char));
                    strcpy(room->connections[room->numConnections], connection);
                
                    //Increase number of connections in that room
                    room->numConnections++;
                }

            }
            //Add room to static list of rooms
            rooms[i] = room;

            fclose(currFile);
        }
    }
}

//Function reads connections from rooms and makes sure they're in 
//roomsConnected array for the struct
void mapConnections() {
    //Nested loop goes through all of the rooms and their connections 
    //to create the connections map
    for(int i = 0; i < 7; i++) {
        for(int j = 0; j < rooms[i]->numConnections; j++) {
            int roomNum = getRoomID(rooms[i]->connections[j]);
            rooms[i]->roomsConnected[j] = rooms[roomNum];
        }
    }
}

//Function returns the roomID of the room with the name provided
int getRoomID(char* roomName) {
    //Loop through all the rooms to find the roomID of the room with that name
    for(int i = 0; i < 7; i++) {
        if(strstr(rooms[i]->name, roomName) != NULL) {
            return i;
        }
    }
    //Return -1 if room name was not found, so no corresponding id
    //can be provided
    return -1;
}

//Function returns the next room selected by the user
int getSelection(int currentRoomNum) {
    //Variable to store the choice of room typed by user
    char choice[40];

    while(true) {
        //Print question to user and get user selection
        printf("WHERE TO? >");
        fgets(choice, 40, stdin);

        //Get the roomID or the name entered
        char roomName[40];
        sscanf(choice, "%s\n", roomName);
        int roomNum = getRoomID(roomName);

        //Validating user input, if user entered too long a name or
        //the name was not found in the rooms, prompt user to enter 
        //name again
        if(roomNum == -1 || strlen(roomName) != strlen(rooms[roomNum]->name)) {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");

            //Get current room and print for user
            struct Room* currRoom = rooms[currentRoomNum];
            printf("CURRENT LOCATION: %s\n", currRoom->name);

            //Initialize string for printing connections
            char connectionsBuffer[100];

            //Loop through all of the connections and add to connections buffer
            for(int i = 0; i < currRoom->numConnections; i++) {
                if(i == 0) {
                    strcpy(connectionsBuffer, currRoom->roomsConnected[i]->name);
                }
                else {
                    strcat(connectionsBuffer, currRoom->roomsConnected[i]->name);
                }

                //If reached the end of the list of connected rooms, place a period
                if(i == currRoom->numConnections - 1) {
                    strcat(connectionsBuffer, ".");
                }
                //Otherwise, place a comma
                else {
                    strcat(connectionsBuffer, ", ");
                }
            }
            //Print the possible connections for the user
            printf("POSSIBLE CONNECTIONS: %s\n", connectionsBuffer);
        }
        else {
            //Room entered by user was found and we can check if the connection exists
            bool connectionExists = false;

            //Loop through the connections array to confirm connection
            for(int i = 0; i < rooms[currentRoomNum]->numConnections; i++) {
                if(roomNum == rooms[currentRoomNum]->roomsConnected[i]->roomID) {
                    connectionExists = true;
                }
            }
            //If connection doesn't exist, prompt user to select another connection
            if(connectionExists == false) {
                printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");

                //Get current room and print for user
                struct Room* currRoom = rooms[currentRoomNum];
                printf("CURRENT LOCATION: %s\n", currRoom->name);

                //Initialize string for printing connections
                char connectionsBuffer[256];

                //Loop through all of the connections and add to connections buffer
                for(int i = 0; i < currRoom->numConnections; i++) {
                    if(i == 0) {
                        strcpy(connectionsBuffer, currRoom->roomsConnected[i]->name);
                    }
                    else {
                        strcat(connectionsBuffer, currRoom->roomsConnected[i]->name);
                    }

                    //If reached the end of the list of connected rooms, place a period
                    if(i == currRoom->numConnections - 1) {
                        strcat(connectionsBuffer, ".");
                    }
                    //Otherwise, place a comma
                    else {
                        strcat(connectionsBuffer, ", ");
                    }
                }
                //Print the possible connections for the user
                printf("POSSIBLE CONNECTIONS: %s\n", connectionsBuffer);
            }
            //Connection exists! Return the roomNum and go back to gameLoop
            else {
                printf("\n");
                return roomNum;
            }
        }
    }
    //We must return something here and this suggest something bad happened
    return -1; 

}

//Function operates the adventure game, tracking the path the user takes from the starting room
//and tracks the number of steps
void gameLoop() {
    //Tracks whether the user has selected the end room
    bool gameOn = true;

    //Initialize step counter and the room number of current room
    int roomNum = 0;
    int stepCount = 0;

    //Array holds the path the user has taken in the game and this will
    //get printed out at the end
    char* path[100];

    //Continue looping through until the user reaches the end room
    while(gameOn) {
        //Get current room and print for user
        struct Room* currRoom = rooms[roomNum];
        printf("CURRENT LOCATION: %s\n", currRoom->name);

        //Initialize string for printing connections
        char connectionsBuffer[100];

        //Loop through all of the connections and add to connections buffer
        for(int i = 0; i < currRoom->numConnections; i++) {
            if(i == 0) {
                strcpy(connectionsBuffer, currRoom->roomsConnected[i]->name);
            }
            else {
                strcat(connectionsBuffer, currRoom->roomsConnected[i]->name);
            }

            //If reached the end of the list of connected rooms, place a period
            if(i == currRoom->numConnections - 1) {
                strcat(connectionsBuffer, ".");
            }
            //Otherwise, place a comma
            else {
                strcat(connectionsBuffer, ", ");
            }
        }
        //Print the possible connections for the user
        printf("POSSIBLE CONNECTIONS: %s\n", connectionsBuffer);

        //Update the roomNum to the next room selected by the user 
        roomNum = getSelection(roomNum);

        //Allocate memory to the path array and add user path to array
        path[stepCount] = calloc(10, sizeof(char));
        strcpy(path[stepCount], rooms[roomNum]->name);

        //Increment step count
        stepCount++;

        //Check if the end room has been reached
        if(strstr(rooms[roomNum]->type, "END_ROOM") != NULL) {
            //Stop the gameOn loop if the end room has been reached
            gameOn = false;
        }
    }

    //Once the gameOn loop has stopped, the game is over and user can be informed
    printf("YOU HAVE FOUND THE END ROOM! CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS: \n", stepCount);

    //Print out the path taken to the user with each room on one line
    for(int i = 0; i < stepCount; i++) {
        printf("%s\n", path[i]);
    }
}

int main() {
    //Get the most recent directory and store it in latestDirName
    char latestDirName[100];
    strcpy(latestDirName, getLatestDirectory());
    
    //Read the room files in the most recent directory
    readRoomFiles(latestDirName);

    //Locate connections between all the rooms
    mapConnections();

    //Actually play the game
    gameLoop();

    //Free memory
    for(int i = 0; i < 7; i++) {
        struct Room* room = rooms[i];
        for(int j = 0; j < room->numConnections; j++) {
            free(room->connections[j]);
        }
        free(rooms[i]);
    }

    return 0;
}