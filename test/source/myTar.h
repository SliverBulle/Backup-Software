#pragma once
#include<string.h>
#include <windows.h>
#define BLOCKSIZE 512
typedef struct tarHeader
{                                 
	char name[100];               //文件名
	char size[10];                //文件大小
	char type[10];                //文件类型
	char supplement[392];         //补充
} tarHeader;

bool Same(char* a, char* b) {
	for (int i = 0; i < BLOCKSIZE; i++) {
		if (a[i] != b[i])
			return false;
	}
	return true;
}

//获得目标路径
void GetShortCutTarget(char* linkPath, char* targetPath, char* StorePath) {

	//生成控制台指令
	char* cmdPath = (char*)malloc(strlen(linkPath) * sizeof(char) * 2 + 20);
	cmdPath[0] = 'd', cmdPath[1] = 'i', cmdPath[2] = 'r', cmdPath[3] = ' ';
	int i, j;
	for (i = 0, j = 4; i < strlen(linkPath); i++, j++) {
		if (linkPath[i] == '/') {
			cmdPath[j] = '\\';
		}
		else {
			cmdPath[j] = linkPath[i];
		}
	}
	cmdPath[j++] = ' ';
	cmdPath[j++] = '>';
	cmdPath[j++] = '>';
	cmdPath[j++] = ' ';
	for (i = 0; i < strlen(StorePath); i++, j++) {
		if (StorePath[i] == '/') {
			cmdPath[j] = '\\';
		}
		else {
			cmdPath[j] = StorePath[i];
		}
	}
	cmdPath[j] = '\0';

	//然后新建一个文件存放cmd结果
	FILE* file = fopen(StorePath, "w");
	fclose(file);

	//执行命令得到结果
	int x=system(cmdPath);

	//获取结果
	char buffer[10000];
	memset(buffer, 0, 10000);
	file = fopen(StorePath, "r");
	fread(buffer, 1000, 1, file);

	//从结果中找到目标地址
	for (int i = 0; true; i++) {
		if (buffer[i] == '[') {
			i++;
			int j = 0;
			for (; buffer[i]!=']'; i++, j++) {
				targetPath[j] = buffer[i];
			}
			targetPath[j] = '\0';
			break;
		}
	}
	fclose(file);
	return;
}

//创建软连接
void CreateShortCut(char* linkPath, char* targetPath) {
	char* cmd = (char*)malloc(BLOCKSIZE);
	memset(cmd, 0, BLOCKSIZE);
	strcpy(cmd, "mklink \"");
	strcat(cmd, linkPath);
	strcat(cmd, "\" \"");
	strcat(cmd, targetPath);
	strcat(cmd, "\"");
	system(cmd);
	return;
}

//打包函数
//三个参数的含义依次是：源文件路径，目的文件路径，是否为根
int CreateTar(const char* tarSource, const char* tarDest, bool root = 1) {
	//错误判断：源路径为空指针
	if (tarSource == NULL){
		printf("源路径为空！\n");
		system("pause");
		exit(0);
	}
	//如果是第一次调用该函数，则创建文件并将其清空
	if (root == true) {
		FILE* fp = fopen(tarDest, "w");
		//错误判断：路径错误
		if (fp == NULL) {
			printf("目的路径有误");
			system("pause");
			exit(0);
		}
		//写入秘钥check
		char check[BLOCKSIZE];
		memset(check, 0, BLOCKSIZE);
		strcpy(check,"CodeCheck");
		fwrite(check, BLOCKSIZE, 1, fp);
		fclose(fp);
	}
	//先打开目的文件,模式使用追加写入
	FILE* dest = fopen(tarDest, "a");
	//从根目录开始依次打包
	DIR* dir = opendir(tarSource);
	if (dir == NULL) {
		printf("%s",tarSource);
		printf("目录错误\n");
		fclose(dest);
		remove(tarDest);
		system("pause");
		exit(0);
	}
	dirent* ptr;
	char buffer[BLOCKSIZE];
	char tmp[10];//这个是用来处理itoa函数返回‘？’错的
	while ((ptr = readdir(dir))!=0) {
		//如果当前指向的是自己或者上一级
		if ((strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) && ptr->d_type == 0x4000) {
			//那么不作处理
			continue;
		}
		//获得新的源
		char newSource[100];
		strcpy(newSource, tarSource);
		strcat(newSource, "/");
		strcat(newSource, ptr->d_name);

		//如果当前指向的是目录
		if (ptr->d_type == 0x4000) {
			//创建文件头
			tarHeader* header = (tarHeader*)malloc(sizeof(tarHeader));
			memset(header, 0, sizeof(tarHeader));
			strcpy(header->name, ptr->d_name);
			_itoa_s(0, tmp, 10, 10);
			strcpy(header->size, tmp);
			_itoa_s(ptr->d_type, tmp, 10, 16);
			strcpy(header->type, tmp);
			//写入文件头
			fwrite(header->name, 100, 1, dest);
			fwrite(header->size, 10, 1, dest);
			fwrite(header->type, 10, 1, dest);
			fwrite(header->supplement, 392, 1, dest);
			//递归的进行一个包的打
			fclose(dest);
			CreateTar(newSource, tarDest, 0);
			dest = fopen(tarDest, "a");
			//写入俩全0块
			memset(buffer, 0, BLOCKSIZE);
			fwrite(buffer, BLOCKSIZE, 1, dest);
			fwrite(buffer, BLOCKSIZE, 1, dest);
		}
		//如果当前指向的是文件
		if (ptr->d_type == 0x8000) {
			//判断当前文件是文件还是软链接
			//获取该文件的绝对路径
			char linkPath[BLOCKSIZE];
			memset(linkPath, 0, BLOCKSIZE);
			strcpy(linkPath, tarSource);
			strcat(linkPath, "/");
			strcat(linkPath, ptr->d_name);
			WIN32_FIND_DATA  fileAttr;
			HANDLE  handle;
			handle = FindFirstFile(linkPath, &fileAttr);
			if (fileAttr.dwFileAttributes == 0x420) {
				//如果这些文件是软链接
				//设置文件头
				tarHeader* header = (tarHeader*)malloc(sizeof(tarHeader));
				memset(header, 0, sizeof(tarHeader));
				strcpy(header->name, ptr->d_name);
				_itoa_s(0, tmp, 10, 10);
				strcpy(header->size, tmp);
				_itoa_s(0x420, tmp, 10, 16);
				strcpy(header->type, tmp);
				//这里用tar头的最后一个模块存储目标路径
				//在该软链接旁，紧急创建一个存储cmd命令结果的文件
				char storePath[BLOCKSIZE];
				memset(storePath, 0, BLOCKSIZE);
				strcpy(storePath, linkPath);
				strcat(storePath, "MidStore");
				GetShortCutTarget(linkPath, header->supplement, storePath);
				remove(storePath);
				//写入文件头
				fwrite(header->name, 100, 1, dest);
				fwrite(header->size, 10, 1, dest);
				fwrite(header->type, 10, 1, dest);
				fwrite(header->supplement, 392, 1, dest);
			}
			else {
				FILE* file = fopen(newSource,"r");
				//获取当前文件的长度
				fseek(file, 0, SEEK_END);
				int size = ftell(file);
				fseek(file, 0, SEEK_SET);
				//设置文件头
				tarHeader* header=(tarHeader*)malloc(sizeof(tarHeader));
				memset(header, 0, sizeof(tarHeader));
				strcpy(header->name, ptr->d_name);
				_itoa_s(size, tmp, 10, 10);
				strcpy(header->size, tmp);
				_itoa_s(ptr->d_type, tmp, 10, 16);
				strcpy(header->type, tmp);
				//写入文件头
				fwrite(header->name, 100, 1, dest);
				fwrite(header->size, 10, 1, dest);
				fwrite(header->type, 10, 1, dest);
				fwrite(header->supplement, 392, 1, dest);
				//对于文件的前半部分，照样写入
				for (int i = 0; i < size / BLOCKSIZE; i++) {
					fread(buffer,BLOCKSIZE,1,file);
					fwrite(buffer, BLOCKSIZE, 1, dest);
				}
				//如果文件没有被完整写入，则要在最后一个阶段补0
				if (size % BLOCKSIZE != 0) {
					memset(buffer, 0, BLOCKSIZE);
					fread(buffer, size % BLOCKSIZE, 1, file);
					fwrite(buffer, BLOCKSIZE, 1, dest);
				}
			}
			//结束以后，写入两个全0块
			memset(buffer, 0, BLOCKSIZE);
			fwrite(buffer, BLOCKSIZE, 1, dest);
			fwrite(buffer, BLOCKSIZE, 1, dest);
		}
	}
	//全部结束以后，写入两个全0块
	if (root) {
		memset(buffer, 0, BLOCKSIZE);
		fwrite(buffer, BLOCKSIZE, 1, dest);
		fwrite(buffer, BLOCKSIZE, 1, dest);
	}
	fclose(dest);
	return 0;
}

//解包函数
int AnalyzeList(const char* tarSource, const char* tarDest,int offset) {
	FILE* source = fopen(tarSource, "r");
	char buffer[BLOCKSIZE];
	fseek(source, offset * BLOCKSIZE, SEEK_SET);
	fread(buffer, BLOCKSIZE, 1, source);
	//获取文件名
	char newDest[100];
	strcpy(newDest, tarDest);
	strcat(newDest, "/");
	strcat(newDest, buffer);
	//判断当前文件的类型
	char type[11];
	for (int pos = 0; pos < 10; pos++)
		type[pos] = buffer[pos + 110];
	//如果当前文件是目录的话
	if (!strcmp(type, "4000")) {
		//创建一个新的文件夹
		CreateDirectory(newDest, NULL);
		//对新文件夹递归解包
		int newOffset;
		newOffset = AnalyzeList(tarSource, newDest, offset + 1);
		fclose(source);
		fread(buffer, BLOCKSIZE, 1, source);
		fread(buffer, BLOCKSIZE, 1, source);
		return newOffset + 3;
	}
	//如果当前文件是文件的话
	if (!strcmp(type, "8000")) {
		//创建一个文件
		FILE* file = fopen(newDest,"w");
		//按文件大小读入
		int size = atoi(buffer + 100);
		//对于前面部分，照样写入
		for (int i = 0; i < size / BLOCKSIZE; i++) {
			fread(buffer, BLOCKSIZE, 1, source);
			fwrite(buffer, BLOCKSIZE, 1, file);
		}
		//对于后面部分，只写入size要求部分
		if (size % BLOCKSIZE != 0) {
			fread(buffer, BLOCKSIZE, 1, source);
			fwrite(buffer, size % BLOCKSIZE, 1, file);
		}
		//处理掉后面的俩全0块
		fread(buffer, BLOCKSIZE, 1, source);
		fread(buffer, BLOCKSIZE, 1, source);
		//判断当前文件是否结束
		int end = -2;
		char check[BLOCKSIZE];
		memset(check, 0, BLOCKSIZE);
		fread(buffer, BLOCKSIZE, 1, source);
		if (Same(buffer, check))
			end++;
		fread(buffer, BLOCKSIZE, 1, source);
		if (Same(buffer, check))
			end++;
		fseek(source, -2 * BLOCKSIZE, SEEK_CUR);
		//如果当前文件就此结束
		if (end == 0) {	
			fclose(source);
			return (size + BLOCKSIZE - 1) / BLOCKSIZE + 3;
		}
		else {
			int newOffset;
			newOffset = ftell(source) / BLOCKSIZE ;
			//递归调用解包
			newOffset=AnalyzeList(tarSource, tarDest, newOffset);
			fclose(source);
			return newOffset+(size + BLOCKSIZE - 1) / BLOCKSIZE + 3;
		}
	}
	//如果当前文件是符号链接的话
	if (!strcmp(type, "420")) {
		//获取符号链接的目标
		CreateShortCut(newDest, buffer + 120);
		//处理掉后面的俩全0块
		fread(buffer, BLOCKSIZE, 1, source);
		fread(buffer, BLOCKSIZE, 1, source);
		//判断当前文件是否结束
		int end = -2;
		char check[BLOCKSIZE];
		memset(check, 0, BLOCKSIZE);
		fread(buffer, BLOCKSIZE, 1, source);
		if (Same(buffer, check))
			end++;
		fread(buffer, BLOCKSIZE, 1, source);
		if (Same(buffer, check))
			end++;
		fseek(source, -2 * BLOCKSIZE, SEEK_CUR);
		//如果当前文件就此结束
		if (end == 0) {
			fclose(source);
			return 3;
		}
		else {
			int newOffset;
			newOffset = ftell(source) / BLOCKSIZE;
			//递归调用解包
			newOffset = AnalyzeList(tarSource, tarDest, newOffset);
			fclose(source);
			return newOffset +  3;
		}
	}
	fclose(source);
	return 0;
}

int AnalyzeTar(const char* tarSource, const char* tarDest) {
	int size;
	FILE* file=fopen(tarSource,"r");
	if (file == NULL) {
		printf("路径有误");
		fclose(file);
		system("pause");
		return -1;
	}
	//获取当前文件的长度
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);
	char buffer[BLOCKSIZE];
	memset(buffer, 0, BLOCKSIZE);
	fread(buffer, BLOCKSIZE, 1, file);
	if (strcmp(buffer, "CodeCheck")) {
		printf("命令错误或密码错误\n");
		fclose(file);
		system("pause");
		return 0;
	}
	fclose(file);
	int offset = 1;
	while (offset < size / 512 - 2) {
		offset += AnalyzeList(tarSource, tarDest, offset);
	}
	return 0;
}