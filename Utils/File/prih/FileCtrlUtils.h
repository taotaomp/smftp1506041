#ifndef __FILECTRL__
#define __FILECTRL__ 

extern int openFile(char *file_name);
extern int closeFile(int fd);
extern int readFile(int fd,char *file_container[]);
extern void writeFile(int fd,char *file_container[],int line_conut);
extern int createFile(char *file_name);
extern int writeFileForSingleLine(int fd,char *fileSingleLine);

#endif
