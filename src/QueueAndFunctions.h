#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct packet{
    struct packet * next;
    struct packet * prev;
    int packetId;
    int timeElapsed;
    int ifDropped;
    bool isACKed;
    float dataSize;
}packet;

typedef struct Queue{
    packet *firstPacket, *lastPacket;
    int queueSize;
}Queue;

/*
This function creates a queue and returns it
*/
Queue * createQueue(){
    Queue *sampleQueue = (Queue *)malloc(sizeof(Queue));
    sampleQueue->firstPacket = NULL;
    sampleQueue->lastPacket = NULL;
    sampleQueue->queueSize = 0;
    return sampleQueue;
}

/*
This function creates new packet by taking its properties as
parameters and returns the packet.
*/
packet * newPacket( int packetId, float dataSize, bool isACKed ){
    packet * temp = (packet *)malloc(sizeof(packet));
    temp->dataSize = dataSize;
    temp->isACKed = isACKed;
    temp->ifDropped = 0;
    temp->next = NULL;
    temp->timeElapsed = 0;
    temp->packetId = packetId;
    return temp;
}

/*
This function takes a queue as a parameter and
returns the first packet of the queue.
*/
packet * dequeue( Queue *currentQueue ){
    if( currentQueue->firstPacket == NULL ){
        return NULL;
    }
    packet * temp;
    temp = currentQueue->firstPacket;
    currentQueue->firstPacket = currentQueue->firstPacket->next;

    if( currentQueue->firstPacket == NULL ){
        currentQueue->lastPacket = NULL;
    }
    else
        currentQueue->firstPacket->prev = NULL;
    return temp;
}

/*
This function takes a queue and a packet and adds the packet
to the given queue.
*/
void enQueue( Queue * currentQueue, packet * currentPacket ){
    if( currentQueue->lastPacket == NULL ){
            currentQueue->firstPacket = currentPacket;
            currentQueue->lastPacket = currentPacket;
            currentPacket->prev = NULL;
            currentPacket->next = NULL;
            return;
    }
    currentQueue->lastPacket->next = currentPacket;
    currentPacket->prev = currentQueue->lastPacket;
    currentQueue->lastPacket = currentPacket;
    currentPacket->next = NULL;
}

/*
This function takes a queue as a parameter
and prints the queue backwards.
Created for printing 'Window State'.*/
void printQueue( Queue currentQueue ){
    printf("<WINDOW STATE>: ");
    if ( currentQueue.lastPacket != NULL ){
        while( currentQueue.lastPacket != NULL ){
            if( currentQueue.lastPacket->prev == NULL )
                printf("%d -> %d", currentQueue.lastPacket->packetId, currentQueue.lastPacket->timeElapsed );
            else
                printf("%d -> %d | ", currentQueue.lastPacket->packetId, currentQueue.lastPacket->timeElapsed );
            currentQueue.lastPacket = currentQueue.lastPacket->prev;
        }
    }
    else
        printf("empty");
    printf("\n\n");
}
/*
This function takes a window and a rount-trip time
as parameters, checks the packets in the windows if they
are acknowledged and return the ACK'ed packet's data size. */
float  checkForACK ( Queue * window, int roundTripTime ){
    packet * iter = window->firstPacket;
    while ( iter != NULL ){
        if( iter->ifDropped == 0 ){
            if( iter->timeElapsed == roundTripTime && !iter->isACKed ){
                iter->isACKed = true;
                printf("- Data packet (id:%d) has now been ACK'ed by receiver!\n", iter->packetId);
                return iter->dataSize;
            }
        }
        iter = iter->next;
    }
    return 0;
}

/*
This function takes a window and a queue which has
all the packets as parameters, deletes the ACK'ed
packet from the window and adds the new packet from
allPackets queue.  */
void checkForSentData ( Queue * window, Queue * allPackets ){
    bool packetSent;
    packet * iter = window->firstPacket;
    while( iter != NULL ){
        if( iter->isACKed && ( iter->ifDropped == 0 ) ){
            packet * temp = dequeue( window );
            window->queueSize--;
            if ( !packetSent ){
                packet * currentPacket = dequeue(allPackets);
                if ( currentPacket != NULL ){
                    enQueue( window, currentPacket );
                    printf("- A new data packet (id:%d) has now been sent!\n", currentPacket->packetId);
                    window->queueSize++;
                }
            }
            iter = iter->next;
            free(temp);     /* Deallocate allocated memory */
            packetSent = true;
        }
        else
            break;
    }
}

/*
This function takes a window and the round-trip time
as parameters, counts the elapsed time of the packets
in the window. */
void counterForElapsedTime( Queue * window, int roundTripTime ){
    packet * iter = window->firstPacket;
    while( iter != NULL ){
        if( iter->ifDropped > 0 )
            iter->timeElapsed++;
        else if( iter->timeElapsed != roundTripTime )
            iter->timeElapsed++;
        iter = iter->next;
    }
}

/*
This function takes a window, time out duration and
the number of dropped packets as parameters,
control the packets if they are dropped and returns
the number of dropped packets. */
int  checkForDroppedPacket( Queue * window, int timeOut, int numOfDroppedPackets ){
    packet * iter = window->firstPacket;
    while( iter != NULL ){
        if( iter->ifDropped > 0 ){
            if ( iter->timeElapsed >= timeOut ){
                printf("- It is timeout for data packet (id:%d), so it has been resent now!\n", iter->packetId);
                iter->ifDropped--;
                numOfDroppedPackets++;
                iter->timeElapsed = 0;
            }
        }
        iter = iter->next;
    }
    return numOfDroppedPackets;
}

/*
This function takes hours, minutes and seconds
as parameters and holds the simulation times.
*/
void simulationTimes( int hours, int minutes, int seconds ){
    minutes = seconds / 60;
    hours = minutes / 60;
    seconds = seconds % 60;
    printf("<SIMULATION TIME> (%02d:%02d:%02d)\n", hours, minutes, seconds);
}
#endif

