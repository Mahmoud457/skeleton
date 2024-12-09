#include "analysis.h"
#include "hashset.h"
#include <pcap.h>
#include <netinet/if_ether.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdatomic.h>

int isSyn(const unsigned char *packet);
int isArp(const unsigned char *packet);
int isBlacklisted(const unsigned char *packet, atomic_int* googleCount, atomic_int* bbcCount, int verbose);

void analyse(struct pcap_pkthdr *header,
             const unsigned char *packet,
             int verbose, HashSet* synIps, atomic_int* synCount, atomic_int* ArpCount, atomic_int* googleCount, atomic_int* bbcCount) {
              
              
              
              isBlacklisted(packet, googleCount, bbcCount, verbose);


              if(isArp(packet)!=0){
                atomic_fetch_add(ArpCount, 1);
              }
              
              else if(isSyn(packet) != 0){
                char ip[16];
                sprintf(ip, "%d.%d.%d.%d", packet[12], packet[13], packet[14], packet[15]);

                atomic_fetch_add(synCount, 1);
                addElement(synIps, ip);
              }
              

              




}
int isBlacklisted(const unsigned char *packet, atomic_int* googleCount, atomic_int* bbcCount, int verbose) {


    uint8_t ipHeaderLength = (packet[0] & 0x0F) * 4;
    const unsigned char *tcpHeader = packet + ipHeaderLength;
    uint8_t tcpHeaderLength = (*(tcpHeader + 12) >> 4) * 4;
    const unsigned char *payload = tcpHeader + tcpHeaderLength;

    uint16_t dstPort = ntohs(*(uint16_t *)(tcpHeader + 2));
    if (dstPort != 80) {
        printf("%d\n", dstPort);
        return 0;
    }




    


    if (strstr((const char *)payload, "Host: www.google.co.uk") != NULL) {
        printf("Blacklisted URL Violation detected!\n");

        char srcIP[INET_ADDRSTRLEN];
        char dstIP[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, packet + 12, srcIP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, packet + 16, dstIP, INET_ADDRSTRLEN);

        if(verbose){
          printf("Source IP: %s\n", srcIP);
          printf("Destination IP: %s\n", dstIP);

        }

        

        atomic_fetch_add(googleCount, 1);


        return 1;
    }
    else if (strstr((const char *)payload, "Host: www.bbc.co.uk") != NULL) {
        printf("Blacklisted URL Violation detected!\n");

        char srcIP[INET_ADDRSTRLEN];
        char dstIP[INET_ADDRSTRLEN];

        inet_ntop(AF_INET, packet + 12, srcIP, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, packet + 16, dstIP, INET_ADDRSTRLEN);

        

        if(verbose){
          printf("Source IP: %s\n", srcIP);
          printf("Destination IP: %s\n", dstIP);

        }
        
        atomic_fetch_add(bbcCount, 1);

        return 1;
    }

    return 0;
}


int isSyn(const unsigned char *packet){
  uint8_t ipHeaderLength = (packet[0] & 0x0F) * 4; 
  const unsigned char *tcpHeader = packet + ipHeaderLength;

  uint8_t flag = tcpHeader[13];

  int isSyn = (flag & 0x02) != 0;
  int isAck = (flag & 0x10) != 0;

  return (isSyn && !isAck);
}

int isArp(const unsigned char *packet){
  const struct ether_header *ethHeader = (struct ether_header*)packet;

  if(ntohs(ethHeader -> ether_type)!= 0x0806){
    return 0;
  }

  const struct ether_arp *arpHeader = (struct ether_arp *)(packet + sizeof(struct ether_header));

  if(ntohs(arpHeader->ea_hdr.ar_op)==ARPOP_REPLY){
    return 1;
  }
  return 0;


}
