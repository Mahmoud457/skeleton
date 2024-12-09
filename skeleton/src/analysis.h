#ifndef CS241_ANALYSIS_H
#define CS241_ANALYSIS_H

#include <pcap.h>
#include "hashset.h"
#include <stdatomic.h>

void analyse(struct pcap_pkthdr *header,
              const unsigned char *packet,
              int verbose, HashSet* synIps, atomic_int* synCount, atomic_int* ArpCount, atomic_int* googleCount, atomic_int* bbcCount);

#endif
