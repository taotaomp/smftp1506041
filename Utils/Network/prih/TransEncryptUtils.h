#ifndef __TRANSENCRYPT__
#define __TRANSENCRYPT__

extern int EncryptValue(char *raw_String,char *str_encrypt,char *key,int flag);
extern void generateKey(char *key_container);

#endif