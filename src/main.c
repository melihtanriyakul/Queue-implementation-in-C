#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "QueueAndFunctions.h"

#define BUFSIZE 1000

int main( int argc, char *argv[] ){

    /* Assigning command line arguments to corresponding variables. */
    int windowSize = atoi( argv[1] );
    float dataSize = atoi( argv[2] );
    float packetSize = atoi( argv[3] );
    int timeOut = atoi( argv[4] );
    int roundTripTime = atoi( argv[5] );
    int numOfTotalPackets = ( dataSize + packetSize - 1 ) / packetSize;
    float dataSizeForPrint = dataSize;

    /* Creating packets. */
    Queue *allPackets = createQueue();
    int i;
    for ( i = 0; i < numOfTotalPackets; i++ ){
        if ( dataSize  >= packetSize ){
            packet * currentPacket = newPacket( i, packetSize, false );
            enQueue( allPackets, currentPacket );
            dataSize = dataSize - packetSize;
        }
        else {
            packet * currentPacket = newPacket( i, dataSize, false );
            enQueue( allPackets, currentPacket );
            dataSize = 0;
        }
    }

    /* Determining packets which will be dropped.*/
    FILE *f;
    f = fopen( argv[6], "r" );
    char buff[BUFSIZE];
    while ( fgets( buff, BUFSIZE - 1, f ) != NULL){
        int a = atoi( buff );
        packet * iter = allPackets->firstPacket;
        while ( iter->next != NULL ){
            if ( iter->packetId == a ){
                iter->ifDropped++;
                break;
            }
            else
                iter = iter->next;
        }
    }
    fclose(f);

    /* Creating WINDOW. */
    Queue * window;
    window = createQueue();
    int numOfDroppedPackets = 0;
    float sentDataSize = 0;
    int seconds = 0;
    int minutes = 0;
    int hours = 0;

    /* Starting simulation. */
    simulationTimes( hours, minutes, seconds );
    printf("<EVENTS>\n");
    printf("- Data size sent so far is %4.2f Byte\n", sentDataSize);
    printQueue( *window );
    seconds++;
    while ( allPackets->lastPacket != NULL || window->lastPacket != NULL ){
        simulationTimes( hours, minutes, seconds );
        printf("<EVENTS>\n");
        if( window->queueSize < windowSize && allPackets->lastPacket != NULL ){
            packet * currentPacket = dequeue(allPackets);
            enQueue( window, currentPacket );
            printf("- A new data packet (id:%d) has now been sent!\n", currentPacket->packetId);
            printf("- Data size sent so far is %4.2f Byte\n", sentDataSize);
            window->queueSize++;
            counterForElapsedTime( window ,roundTripTime );
            printQueue( *window );
        }
        else {
            float ifDataACK = checkForACK( window, roundTripTime );
            numOfDroppedPackets = checkForDroppedPacket( window, timeOut, numOfDroppedPackets );
            checkForSentData( window, allPackets );
            counterForElapsedTime( window ,roundTripTime );
            if ( ifDataACK > 0) {
                sentDataSize += ifDataACK;
                printf("- Data size sent so far is %4.2f Byte\n", sentDataSize);
            }
            else
                printf("- Data size sent so far is %4.2f Byte\n", sentDataSize);
            printQueue( *window );
        }
        seconds++;
    }

    /* Printing the transfer report. */
    int totalTime = numOfTotalPackets * roundTripTime;
    totalTime = totalTime + numOfDroppedPackets * timeOut;
    float averageTime = (float)totalTime / (float)numOfTotalPackets;
    printf("***************************************************\n"
    "*                 TRANSFER REPORT                 *\n"
    "***************************************************\n"
    "Parameter Setting:\n"
    "---------------------------------------------------\n"
    "Window Size                          :           %02d\n"
    "Timeout                              :    %04d Sec.\n"
    "RTT                                  :    %04d Sec.\n"
    "Data Size                            : %4.2f Byte\n"
    "Packet Size                          :   %4.2f Byte\n"
    "---------------------------------------------------\n"
    , windowSize, timeOut, roundTripTime, dataSizeForPrint, packetSize);
    printf("Results:\n"
    "---------------------------------------------------\n"
    "Number of packet to send the data    :         %04.0f\n"
    "Number of packet dropped             :         %04d\n"
    "Average time to send a single packet :  %3.3f Sec.\n"
    "***************************************************", (float)numOfTotalPackets, numOfDroppedPackets, averageTime);
    /* Deallocate allocated memory */
    free( window );
    free( allPackets );
    return 0;
}

