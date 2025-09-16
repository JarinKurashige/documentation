# CAN-Trace

## Prerequisites

- Install VS-code
- Install VS-code extensions:
  - C/C++ intelli sense, debugging
  - C/C++ extension pack
  - CMake
  - CMake tools
 
## Build
- Add folder to Workspace in VS-code and open terminal here and execute the commands below

mkdir build

cd build

cmake ..

cmake --build .

## Use

Example:
```
.\bbb2pcap.exe .\SectionData.dat .\test.pcap 2 1744103382
```

This command creates a PCAP file from a 2k section dat file with a time stamp offset of 1744103382 (Tue Apr 08 2025 09:09:42 GMT+0000)




