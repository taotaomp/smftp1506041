/*编译时要加上 -lcrypto*/
#include <openssl/des.h>
#include <string.h>
#include <malloc.h>


/**************************
传输加密函数
描述:结果容器的大小要等于离原始字符串的长度最近的且大于原始字符串的一个8的倍数
raw_String:原始字符串
str_encrypt:结果容器
key:秘钥
flag:加密解密判断标识 0:加密;非0:解密
**************************/
int EncryptValue(char *raw_String,char *str_encrypt,char *key,int flag){
	DES_cblock DES_key;				//DES秘钥容器
	DES_key_schedule schedule;		//DES_key_schedule 加密所用到的变量
	int ecb_encrypt_flag;			//加密解密判断标识的容器

	unsigned char input[8];			//DES的ecb加密方式一次只能加密8个字节
    unsigned char output[8];
    int raw_String_Length;			//原始字符串长度
    int cycle_time;					//循环次数（加密次数）因为1次只能加密8个字节，所以超出8个字节的字符串需要分次加密
    int model;						//用于存放原始字符串长度除以8得到的余数，余数，余数
    int i,j;
    int j_model_index;				//记录当原始字符串长度不能被8整除时，进行最后一次加密开始时，应该加密的原始字符串中的开始位置
    char temp_container[1280];
    unsigned char temp[8];

    if(0 == flag){					//判断标识，确定此函数是加密还是解密函数
    	ecb_encrypt_flag = DES_ENCRYPT;
    }else{
    	ecb_encrypt_flag = DES_DECRYPT;
    }

	memcpy(DES_key,key,8);						//将参数的key赋值给容器
	DES_set_key_unchecked(&DES_key, &schedule);	//通过秘钥生成schedule

	raw_String_Length = strlen(raw_String);	//得到原始字符串长度
	model = raw_String_Length % 8;				//得到余数

	if(model == 0){								//判断原始字符串的长度是否能被8整除
		cycle_time = raw_String_Length/8;		//能，则需要加密的次数，为原始字符串长度除以8
		for(i = 0,j = 0 ; i < cycle_time ; i++,j = j+8){	
			input[0] = raw_String[j];			//8位为一组:00000000->黑盒->xxxxxxxx
        	input[1] = raw_String[j+1];
        	input[2] = raw_String[j+2];
        	input[3] = raw_String[j+3];
       		input[4] = raw_String[j+4];
        	input[5] = raw_String[j+5];
        	input[6] = raw_String[j+6];
			input[7] = raw_String[j+7];
			DES_ecb_encrypt(&input, &output, &schedule, ecb_encrypt_flag);
			str_encrypt[j] = output[0];
			str_encrypt[j+1] = output[1];
        	str_encrypt[j+2] = output[2];
        	str_encrypt[j+3] = output[3];
			str_encrypt[j+4] = output[4];
        	str_encrypt[j+5] = output[5];
			str_encrypt[j+6] = output[6];
        	str_encrypt[j+7] = output[7];
		}
	}else{
		cycle_time = (raw_String_Length/8) + 1;	//不能，则需要加密的次数，为原始字符串长度除以8，再+1
		for(i = 0,j = 0 ; i < cycle_time-1 ; i++,j = j+8){	//这里cycle_time-1是指不加密最后一次，最后一次加密由于可能长度不同，需要单独处理
			input[0] = raw_String[j];
        	input[1] = raw_String[j+1];
        	input[2] = raw_String[j+2];
        	input[3] = raw_String[j+3];
       		input[4] = raw_String[j+4];
        	input[5] = raw_String[j+5];
        	input[6] = raw_String[j+6];
			input[7] = raw_String[j+7];
			DES_ecb_encrypt(&input, &output, &schedule, ecb_encrypt_flag);
			str_encrypt[j] = output[0];
			str_encrypt[j+1] = output[1];
        	str_encrypt[j+2] = output[2];
        	str_encrypt[j+3] = output[3];
			str_encrypt[j+4] = output[4];
        	str_encrypt[j+5] = output[5];
			str_encrypt[j+6] = output[6];
        	str_encrypt[j+7] = output[7];
		}
		j_model_index = j;		//进行最后一次加密开始时，应该加密的原始字符串中的开始位置
		//最后一次加密
		for(i = 0,j = j_model_index; i < model; i++,j++){
			input[i] = raw_String[j];
		}
		input[i] = '\0';		//使input最后一位为'\0'

		DES_ecb_encrypt(&input, &output, &schedule, ecb_encrypt_flag);
		for(i = 0,j = j_model_index; i < 8; i++,j++){		//将output全部放到str_encrypt容器中
			str_encrypt[j] = output[i];
		}

	}
	return 0;
}

/**********************************
秘钥生成函数
key_container:秘钥容器
**********************************/
void generateKey(char *key_container){
	DES_cblock DES_key;				//DES秘钥容器
	DES_random_key(&DES_key);		//随机生成秘钥
	memcpy(key_container,DES_key,8);//赋值
}
/*
int main(){
	char key_container[8];
	char *value1 = "123456789";
	char *value2 ;
	char *value3 ;
	value2 = (char*)malloc(16);
	value3 = (char*)malloc(16);

	generateKey(key_container);

	EncryptValue(value1,value2,key_container,0);
	puts(value2);
	EncryptValue(value2,value3,key_container,1);
	puts(value3);
}*/