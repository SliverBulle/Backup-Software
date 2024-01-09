#include<sys/types.h>
#include<dirent.h>
#include<stdio.h>
#include"myTar.h"
#include<string.h>
#include"zip_encry.h"
#include"path.h"

int main(void) {
	//全体初始化
	Init();
	//读入
	int opCode = ReadIn();
	//操作
	Execute(opCode);

}