/**********************************************************************************
 * Name: Chelsea Marie Hicks
 * 
 * Description: The buildrooms file will create 7 rooms that are connected and
 * can be used to play the adventure game in the adventure.c file
***********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

//Array of possible room names
char roomNames[10][9] = {
    "Forest",
    "Garden",
    "Reef",
    "Library",
    "Cliff",
    "Attic",
    "Shack",
    "Dungeon",
    "Lake",
    "Shore"
};

//Room struct containing room ID, name, room type, number of connections, and struct room
//for tracking connections between rooms
struct Room {
    int roomID;
    char name[9];
    char type[11];
    int numConnections;
    struct Room* roomsConnected[6];
};


//Function checks if the random room number has been used already
bool roomNumUsed(int roomNumbers[], int index, int numFiles) {
    //Loop through the roomNumbers to see if that index is found
    for(int i = 0; i < numFiles; i++) {
        if(roomNumbers[i] == index) {
            return true;
        }
    }
    return false;
}

//Checks whether each room has at least 3 connections 
bool isGraphFull(struct Room roomsArray[]) {
    int fullGraph = 0;

    //Check the number of connections each room has
    for(int i = 0; i < 7; i++) {
        //If every room has 3 or more connections, fullGraph will equal 7
        if(roomsArray[i].numConnections >= 3) {
            fullGraph++;
        }
    }
    //If graph is full, return true to break while loop in main
    if(fullGraph == 7) {
        return true;
    }
    return false;
}

//Checks whether a connection can still be added 
bool connectionsViable(struct Room* room) {
    //Returns false if the max number of rooms has not been reached
    if(room->numConnections < 6) {
        return true;
    }
    return false;
}

//Returns true if two rooms are actually the same based on roomID
bool sameRoom(struct Room* roomA, struct Room* roomB) {
    if(roomA->roomID == roomB->roomID) {
        return true;
    }
    return false;
}

//Selects and returns a random room to make a connection, but doesn't verfy if it can be added
struct Room* getRandomRoom(struct Room roomsArray[]) {
    int randNum = rand() % 7;

    return &roomsArray[randNum];
}

//Checks whether two rooms are already connected or not
bool connectionExists(struct Room* roomA, struct Room* roomB) {
    bool connection = false;

    //Loop through each possible connection in a room's connection array
    //and check if the roomB roomID matches any of the rooms in the list
    for(int i = 0; i < roomA->numConnections; i ++) {
        struct Room* connectedRoom = roomA->roomsConnected[i];
        //If the roomID of roomB matches the roomID of roomA connection
        if(roomB->roomID == connectedRoom->roomID) {
            return true;
        }
    }
    return false;   
}

//Makes a connection between the rooms after verifying in addConnection that this
//connection can happen, increases the number of connections for the room as well
void makeConnection(struct Room* roomA, struct Room* roomB) {
    roomA->roomsConnected[roomA->numConnections] = roomB;
    roomA->numConnections++;
}

//Function adds a random connection from one room to another
//Code from course assignment doc
void addConnection(struct Room roomsArray[]) {
    //Create two room structs
    struct Room* roomA;
    struct Room* roomB;;

    //Find a room that can still have connections
    while(true) {
        roomA = getRandomRoom(roomsArray);

        //Check if roomA can have any other connections
        if(connectionsViable(roomA) == true) {
            break;
        }
    }

    //Find a room that can still have connections, isn't the same as roomA, and isn't already connected
    do {
        roomB = getRandomRoom(roomsArray);
    }
    while(connectionsViable(roomB) == false || sameRoom(roomA, roomB) == true || connectionExists(roomA, roomB) == true);

    //Make a connection between the rooms
    makeConnection(roomA, roomB);
    makeConnection(roomB, roomA);
}

//Function opens new files, writes the necessary information to the file, and closes the file
void createFiles(struct Room roomsArray[], char* directoryName) {
    char fileName[100];
    //For loop goes through rooms, creates a file, and adds pertinent data to file
    for(int i = 0; i < 7; i++) {
        //Create new file using current room data
        struct Room currRoom = roomsArray[i];
        sprintf(fileName, "./%s/room%d", directoryName, i+1);
        FILE* newFile; 
        newFile = fopen(fileName, "w");

        //Check if file opens and if so, write in name, connections, and type 
        if(newFile != NULL) {
            //Print name into the file
            fprintf(newFile, "ROOM NAME: %s\n", currRoom.name);

            //Go through for loop for as many connections there are for the room
            for(int j = 0; j < currRoom.numConnections; j++) {
                //Print connections
                fprintf(newFile, "CONNECTION %d: %s\n", j+1, currRoom.roomsConnected[j]->name);
            }

            //Print room type to the file
            fprintf(newFile, "ROOM TYPE: %s\n", currRoom.type);
        }

        //Close the file once it has all the necessary contents
        fclose(newFile);
    }
}

int main(int argc, char* argvp[]) {
    //Using comp clock for randomization
    srand(time(NULL));

    //Create the directory
    char directory[40] = "hicksche.rooms";
    sprintf(directory, "%s.%d", directory, getpid());
    
    //Open the directory to be written into
    int roomFiles = mkdir(directory, 0755);

    //Start adding rooms to the directory
    if(roomFiles == 0) {
        //Array to track the rooms made
        int roomNums[7];

        //Room list of the 7 rooms
        struct Room rooms[7];

        //While room files are less than 7, continue adding rooms
        while(roomFiles < 7) {
            //Generate random number between 0-9
            int randNum = rand() % 10;

            //If the room using that random number hasn't been made, then make it
            if(roomNumUsed(roomNums, randNum, roomFiles) == false) {
                //Add index of room to roomNums tracker
                roomNums[roomFiles] = randNum;

                //Create room and set its ID, name, and type
                struct Room addRoom;
                addRoom.roomID = roomFiles;
                strcpy(addRoom.name, roomNames[randNum]);

                //Set room types 
                if(roomFiles == 0) {
                    strcpy(addRoom.type, "START_ROOM");
                }
                else if(roomFiles == 1) {
                    strcpy(addRoom.type, "END_ROOM");
                }
                else {
                    strcpy(addRoom.type, "MID_ROOM");
                }
                //Set number of connections to 0
                addRoom.numConnections = 0;

                rooms[roomFiles] = addRoom;
                roomFiles++;
            }
        }
        //Generate connections between rooms until every room has at least 3
        while(isGraphFull(rooms) == false) {
            addConnection(rooms);
        } 
        //Create and add room files to the directory
        createFiles(rooms, directory);  
    }
    return 0;
}
