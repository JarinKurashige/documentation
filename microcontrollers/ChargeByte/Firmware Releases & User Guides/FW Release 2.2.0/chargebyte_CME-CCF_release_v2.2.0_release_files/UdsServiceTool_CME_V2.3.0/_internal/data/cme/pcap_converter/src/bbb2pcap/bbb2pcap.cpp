
#include <stdint.h>
#include <pcap.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>

#pragma comment(lib, "Packet.lib")
#pragma comment(lib, "wpcap.lib")

#define TIME_STAMP_MAX ((uint32_t) 0xFFFEAE7F) //32bit max time stamp; 2106 supported by PCAP due to timeval type (max value - 24 h)

typedef uint8_t u_char;
typedef uint16_t u_short;

//#define BBB_PRB_SIZE 2048
#define BBB_PRB_MAX_SIZE 0x4000
uint8_t bbb_read_byte(uint16_t addr, uint8_t* buffer, uint16_t bbb_size)
{
    return buffer[addr & (bbb_size - 1)];
}

void bbb_read_array(uint16_t addr, uint16_t length, uint8_t* buffer, uint8_t* out, uint16_t bbb_size)
{
    for (uint16_t i = 0; i < length; i++)
    {
        out[i] = bbb_read_byte(addr + i, buffer, bbb_size);
    }
}

int main(int argc, char* argv[]) {

    if (argc < 5)
    {
        printf("Error: not enough arguments\n");
        printf("bbb2pcap.exe [input.prb] [output.pcap] [bbb kByte size : min 2, max 16] [timestamp]");
        return -1;
    }

    printf("%s [%s] [%s] [%s] [%s]\n", argv[0], argv[1], argv[2], argv[3], argv[4]);

    pcap_t* pd;
    pcap_dumper_t* pdumper;
    uint16_t bbb_size = (uint16_t)strtol(argv[3], NULL, 10);

    if ((bbb_size < 2) || (bbb_size > 16))
    {
        printf("Error: bbb kByte size out of range!\n");        
        printf("bbb2pcap.exe [input.prb] [output.pcap] [bbb kByte size : min 2, max 16]");
        return -1;
    }

    time_t time_stamp_base = (time_t)strtoll(argv[4], NULL, 10);

    //Limit time stamp to a max value
    if (time_stamp_base > TIME_STAMP_MAX)
    {
        printf("Warning: Time stamp input exceeds PCAP limits. Truncating to 32 bits!\n");
        time_stamp_base = TIME_STAMP_MAX;
    }

    printf("Interpreted base time: %s", ctime(&time_stamp_base));
    uint32_t time_stamp_base_u32 = (uint32_t) time_stamp_base;

    //Multiply to get byte size of bbb
    bbb_size = bbb_size * 1024u;
    printf("bbb size %d\n", bbb_size);

    pd = pcap_open_dead(DLT_EN10MB, 65535);
    pdumper = pcap_dump_open(pd, argv[2]);
    
    FILE* bbb_dump_file = NULL;
    errno_t ret = fopen_s(&bbb_dump_file, argv[1], "rb");
    if (bbb_dump_file == NULL)
    {
        printf("Error opening input file %s\n", argv[1]);
        return -1;
    }

    uint16_t start = 0;
    fread_s(&start, sizeof start, sizeof start, 1, bbb_dump_file);

    uint16_t tail = 0;
    fread_s(&tail, sizeof tail, sizeof tail, 1, bbb_dump_file);

    uint8_t prb[BBB_PRB_MAX_SIZE] = "";
    fread_s(prb, bbb_size, bbb_size, 1, bbb_dump_file);

    uint8_t packet_count = 0;
    while (true)
    {
        uint16_t packet_length = prb[(start + 0) & (bbb_size - 1)] | (uint16_t)prb[(start + 1) & (bbb_size - 1)] << 8;
        uint8_t packet_type =    prb[(start + 2) & (bbb_size - 1)];

        uint32_t packet_timestamp = prb[(start + 3) & (bbb_size - 1)] 
            | (uint32_t)prb[(start + 4) & (bbb_size - 1)] << 8
            | (uint32_t)prb[(start + 5) & (bbb_size - 1)] << 16
            | (uint32_t)prb[(start + 6) & (bbb_size - 1)] << 24;

        struct pcap_pkthdr packet_header;
        struct timeval ts = { 0 };

        ts.tv_sec = packet_timestamp / 1000 + time_stamp_base_u32;
        ts.tv_usec = (packet_timestamp % 1000) * 1000;

        packet_header.caplen = packet_length-5;
        packet_header.len = packet_length-5;
        packet_header.ts = ts;

        uint8_t tmp[1500] = "";
        bbb_read_array(start + 7, packet_length - 5, prb, tmp, bbb_size);

        pcap_dump((u_char*)pdumper, &packet_header, tmp);

        uint16_t next_start = start + packet_length + 2;

        if (next_start >= tail)
            break;
        else
            start = next_start;

        packet_count++;
    }
    
    pcap_close(pd);
    pcap_dump_close(pdumper);

    return 0;
}