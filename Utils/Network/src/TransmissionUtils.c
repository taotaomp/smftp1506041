#include<string.h>
#include<stdio.h>

/***************************
TLV结构封装函数
container:打包的结果的容器
type:TLV中的T（数据类型）
length:TLV中的L（长度）
value:TLV中的V（值）
返回值:写入结果的容器的字数
***************************/
int packetTLV(char*container,char *type,int length,char *value){
	container[0]='\0';
	/*int i;
	char value_deal[256];

	container[0]='\0';				//初始化container
	for(i = 0 ; i < length ; i++){
		value_deal[i] = *value;
		value++;
	}
	value_deal[length] = '\0';	//以上是处理输入的内容，去掉尾部多余的东西*/
	return sprintf(container,"%s#%d#%s",type,length,value);
}

/**************************
TLV结构拆解函数
raw_string:原始的TLV字符串
container:字符串拆解容器
**************************/
void unpacketTLV(char *raw_string,char *container[]){
	/*container[0][0] = '\0';			//初始化container
	container[1][0] = '\0';
	container[2][0] = '\0';*/

	container[0] = strtok(raw_string,"#");
	container[1] = strtok(NULL,"#");
	container[2] = strtok(NULL,"#");
}

