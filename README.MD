# Speedtest

Speedtest is a simple, small application for Windows, Mac and Linux that performs a network speed-test between two machines.

## Usage
(On Windows, replace `./speedtest` with `speedtest.exe`)

Server: 
```
$ ./speedtest
```

Client:
```
$ ./speedtest <server-ip> [iterations]
```
Where `server-ip` is the IP Address or Hostname of the server machine.   
Where `iterations` is the amount of 10MB iterations to do (default = 10).

### Example:
Server:
```
$ ./speedtest
Client Connected
Client Disconnected
```

Client:
```
$ ./speedtest 10.0.0.1
Populating Buffer...
Connecting to Server...
Connected!
Total Sent: 800.0Mb
Time Taken: 967ms
Speed: 8.67488e+008 b/s
Speed: 827.30 Mb/s
```

Notice how `Mb` is Megabits, not Megabytes. This example was conducted with a 1.0Gbps link to the server, although the actual speed is slower due to 
other network and processor overhead. For a speed test on a single packet, run `./speedtest 10.0.0.1 1`. For a speed test with 100 packets, run `./speedtest 10.0.0.1 100`
Each packet is 10MB (80Mb)

## Building
```
./gradlew build
```

This will build for both x86 and x64. Your build files will be under `build/exe/<arch>/release/`.   
Debug executables are under `build/exe/<arch>/debug`, and will print out more log messages which may or may not decrease the accuracy of the results due to overhead.