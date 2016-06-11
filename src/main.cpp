#include "socket.hpp"

#include <iostream>

#ifdef OS_WIN
    #include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
    #include <sys/time.h>
    #include <time.h>
#else
    #include <sys/time.h>
#endif

#ifndef OS_WIN
    #include <sys/types.h>
    #include <unistd.h>
    #include <signal.h>
#endif

using namespace std;
using namespace Socket;

// 10MB
#define BUFFER_SIZE 10 * (1024 * 1024)
#define PORT 9883

char buf[BUFFER_SIZE];
char replybuf[1];

void runServer();
void runClient(string ip, int times);

int main(int argc, char *argv[]) {
    socket_init();
    if (argc != 3 && argc != 2) {
        runServer();
    } else {
        if (argc == 2) {
            runClient(string(argv[1]), 10);
        } else if (argc == 3) {
            runClient(string(argv[1]), atoi(argv[2]));
        }
    }
    
    return 0;
}

void runServer() {
    ServerSocket socket(PORT);
    socket.open();
    while (1) {
        ClientSocket client = socket.accept();
        cout << "Client Connected!" << endl;
        while (client.read(buf, BUFFER_SIZE) != -1) {
            client.send(replybuf, 1);
            
#ifdef VERBOSE
            cout << "Packet Received!" << endl;
#endif

        }
        cout << "Client Disconnected!" << endl;
    }
}

void clientSendSingle(ClientSocket *s);
char* readable_size(double size, char *buf);
long current_time_millis();

void runClient(string ip, int times) {
    cout << "Populating Buffer..." << endl;
    unsigned int v = rand(), *ptr = (unsigned int *)buf;
    for(int i = 0; i < BUFFER_SIZE / 4; i++)
        ptr[i] = (v << 16) ^ rand();
    
    cout << "Connecting to Server..." << endl;
    ClientSocket socket(ip, PORT);
    socket.connect();
    
    cout << "Connected!" << endl;
    int runningTotal;
    long start_time = current_time_millis();
    for (runningTotal = 0; runningTotal < times; runningTotal++) {
        clientSendSingle(&socket);
    }
    long end_time = current_time_millis();
    long inBytes = (long)(times) * (long)(BUFFER_SIZE);
    char sizeNotation[2];
    readable_size((double)inBytes, (char *)&sizeNotation);
    cout << "Total Sent: " << sizeNotation << endl;
    long time_difference = end_time - start_time;
    cout << "Time taken: " << time_difference << "ms" << endl;
    double time_seconds = (double)(time_difference) / 1000.0;
    double bits = (double)(inBytes) * 8;
    
    cout << "Speed: " << bits / time_seconds << " b/s" << endl;
    readable_size((double)(inBytes) / time_seconds, (char *)&sizeNotation);
    cout << "Speed: " << sizeNotation << "/s" << endl;
}

void clientSendSingle(ClientSocket *s) {
#ifdef VERBOSE
    cout << "Sending Packet" << endl;
#endif
    s->send(buf, BUFFER_SIZE);
    s->read(replybuf, 1);
}

char* readable_size(double size, char *buf) {
    int i = 0;
    size = size * 8;        // Bytes to Bits
    const char* units[] = {"b", "Kb", "Mb", "Gb"};
    while (size > 1024) {
        size /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, size, units[i]);
    return buf;
}

long current_time_millis() {
#ifdef OS_WIN
    SYSTEMTIME time;
    GetSystemTime(&time);
    return (long)((time.wSecond * 1000) + time.wMilliseconds);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
#endif
}