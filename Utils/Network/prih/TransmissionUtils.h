#ifndef __TRANSMISSION__
#define __TRANSMISSION__ 

extern int packetTLV(char*container,char *type,int length,char *value);
extern void unpacketTLV(char *raw_string,char *container[]);

#endif


