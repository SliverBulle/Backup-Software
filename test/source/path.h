#pragma once
#include<stdio.h>
#define BLOCKSIZE 512
char sourcePath[BLOCKSIZE];
char tarPath[BLOCKSIZE];
char zipPath[BLOCKSIZE];
char codePath[BLOCKSIZE];
char decodePath[BLOCKSIZE];
char dezipPath[BLOCKSIZE];
char destPath[BLOCKSIZE];
int code;

void Init() {
	memset(sourcePath, 0, BLOCKSIZE);
	memset(tarPath, 0, BLOCKSIZE);
	memset(zipPath, 0, BLOCKSIZE);
	memset(codePath, 0, BLOCKSIZE);
	memset(decodePath, 0, BLOCKSIZE);
	memset(dezipPath, 0, BLOCKSIZE);
	memset(destPath, 0, BLOCKSIZE);
	code = -1;
	return;
}

int ReadIn() {
	char oper[BLOCKSIZE];
	int opCode;
	//读入操作码
	scanf("%s", oper);
	if (!strcmp(oper, "backup"))
		opCode = 0;
	else if (!strcmp(oper, "reduct"))
		opCode = 1;
	else
	{
		printf("未定义的命令\n");
		gets_s(oper);
		system("pause");
		exit(0);
	}
	opCode = opCode << 1;
	scanf("%s", oper);
	if (!strcmp(oper, "tar")) {
		opCode += 1;
		scanf("%s", oper);
	}
	opCode = opCode << 1;
	if (!strcmp(oper, "zip")) {
		opCode += 1;
		scanf("%s", oper);
	}
	opCode = opCode << 1;
	if (!strcmp(oper, "code")) {
		opCode += 1;
		scanf("%s", oper);
	}
	//获取路径
	strcpy(sourcePath, oper);
	scanf("%s", destPath);
	if (opCode % 2 == 1)
		scanf("%d", &code);
	return opCode;
}

bool FileExist(char* source) {
	FILE* test= fopen(source, "r");
	if (test == NULL) {
		fclose(test);
		return false;
	}
	fclose(test);
	return true;
}

int Execute(int opCode) {
	Code co; Decode deco;
	Encry enc;
	//处理字符串和中间文件
	{
		strcpy(tarPath, destPath);
		strcat(tarPath, "/");
		strcat(tarPath, "backupMid.mytar");
		strcpy(zipPath, destPath);
		strcat(zipPath, "/");
		strcat(zipPath, "backupMid.myzip");
		strcpy(codePath, destPath);
		strcat(codePath, "/");
		strcat(codePath, "backupMid.mycode");
		strcpy(decodePath, destPath);
		strcat(decodePath, "/");
		strcat(decodePath, "backupMid.mydecode");
		strcpy(dezipPath, destPath);
		strcat(dezipPath, "/");
		strcat(dezipPath, "backupMid.mydezip");
	}
	//根据操作码采取对应操作
	if (opCode / 8 == 0) {//如果是备份操作
		if ((opCode % 8) / 4 == 0) {//如果不进行打包，那么后续操作都没有意义
			CreateTar(sourcePath, tarPath, 1);
			AnalyzeTar(tarPath, destPath);
			remove(tarPath);
		}
		else {
			//进行打包
			CreateTar(sourcePath, tarPath, 1);
			if ((opCode % 4) / 2 == 1) {//如果进行压缩
				co.work_code(tarPath, zipPath);
				remove(tarPath);
			}
			if (opCode % 2 == 1) {//如果进行加密
				if ((opCode % 4) / 2 == 1) {
					enc.work_encry(zipPath, codePath, code, 0);
					remove(zipPath);
				}
				else {
					enc.work_encry(tarPath, codePath, code, 0);
					remove(tarPath);
				}
			}
		}
	}
	else {//如果是还原操作
		if ((opCode % 8) / 4 == 0) {//如果不需要解包
			CreateTar(sourcePath, tarPath, 1);
			AnalyzeTar(tarPath, destPath);
			remove(tarPath);
		}
		else {
			if (opCode % 2 == 1) {//如果需要解密
				enc.work_encry(sourcePath, decodePath, code, 1);
			}
			if ((opCode % 4) / 2 == 1) {//如果需要解压
				if (opCode % 2 == 1) {
					deco.decode_work(decodePath, dezipPath);
					remove(decodePath);
				}
				else {
					deco.decode_work(sourcePath, dezipPath);
				}
			}
			if ((opCode % 4) / 2 == 1) {//如果经历过解压
				AnalyzeTar(dezipPath, destPath);
				if (remove(dezipPath) == -1) {
					cout << errno;
				}

			}
			else if (opCode % 2 == 1) {//如果解过密
				AnalyzeTar(decodePath, destPath);
				remove(decodePath);
			}
			else {
				AnalyzeTar(sourcePath, destPath);
			}
		}
	}
	return 0;
}