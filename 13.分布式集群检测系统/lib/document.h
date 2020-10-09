/*************************************************************************
	> File Name: document.h
	> Author: 
	> Mail: 
	> Created Time: 五 10/ 9 11:06:40 2020
 ************************************************************************/

#ifndef _DOCUMENT_H
#define _DOCUMENT_H

#include <iostream>

#include <zlib.h>
#include <sys/socket.h>

class Document {
public:
	int backup(char *filename, char *to_backup);
	int unback(char *backed, char *to_unback);
	int send_to(int fd, char *filename);
	int recv_to(int fd, char *filename);
	int size(char *filename);
private:
};

int Document::backup(char *filename, char *to_backup) {
	FILE *ffile;
    unsigned long int flen, clen;
    unsigned char *fbuf = NULL;
    unsigned char *cbuf = NULL;
    
    //通过命令行参数将filenames文件的数据压缩后存放到backfilenames文件中
    if ((ffile = fopen(filename, "rb")) == NULL) {
        return -1;
    }
    //装载源文件数据到缓冲区
    fseek(ffile, 0, SEEK_END);//跳到文件末尾
    flen = ftell(ffile);//获取文件长度
    fseek(ffile, 0, SEEK_SET);
    if ((fbuf = (unsigned char *)malloc(sizeof(unsigned char) * flen)) == NULL) {
        fclose(ffile);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, ffile);
    //压缩数据
    clen = compressBound(flen);
    if ((cbuf = (unsigned char *)malloc(sizeof(unsigned char) * clen)) == NULL) {
        fclose(ffile);
        return -1;
    }
    if (compress(cbuf, &clen, fbuf, flen) != Z_OK) {
        return -1;
    }
    fclose(ffile);
    if ((ffile = fopen(to_backup, "a+")) == NULL) {
        return -1;
    }
    //保存压缩后的数据到目标文件
    fwrite(&flen, sizeof(unsigned long int), 1, ffile);//写入源文件长度
    fwrite(&clen, sizeof(unsigned long int), 1, ffile);//写入目标数据长度
    fwrite(cbuf, sizeof(unsigned char), clen, ffile);
    fclose(ffile);
    free(fbuf);
    free(cbuf);
    return 1;
}

int Document::unback(char *backed, char *to_unback) {
	//解压函数
    FILE *ffile;
    unsigned long int flen, ulen;
    unsigned char *fbuf = NULL;
    unsigned char *ubuf = NULL;
    //通过命令行参数将文件的数据解压缩后存放到文件中
    if ((ffile = fopen(backed, "rb")) == NULL) {
        return -1;
    }
    //装载源文件数据到缓冲区
    fread(&ulen, sizeof(unsigned long int), 1, ffile);//获取缓冲区大小
    fread(&flen, sizeof(unsigned long int), 1, ffile);//获取数据流大小
    
    if ((fbuf = (unsigned char*)malloc(sizeof(unsigned char) * flen)) == NULL) {
        fclose(ffile);
        return -1;
    }
    fread(fbuf, sizeof(unsigned char), flen, ffile);
    //解压缩数据
    if ((ubuf = (unsigned char*)malloc(sizeof(unsigned char) * ulen)) == NULL) {
        fclose(ffile);
        return -1;
    }
    if (uncompress(ubuf, &ulen, fbuf, flen) != Z_OK) {
        return -1;
    }
    fclose(ffile);
    if ((ffile = fopen(to_unback, "a+")) == NULL) {
        return -1;
    }
    //保存解压缩后的数据到目标文件
    fwrite(ubuf, sizeof(unsigned char), ulen, ffile);
    fclose(ffile);
    free(fbuf);
    free(ubuf);
    return 1;
}

int Document::send_to(int fd, char *filename) {
	FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *data = (char *)malloc(len + 1);
    fread(data, len, 1, fp);
    printf("%s\n", data);
    if (send(fd, data, strlen(data), 0) < 0) {
    }
    fclose(fp);
    memset(data, 0, sizeof(len + 1));
    return 1;
}

int Document::recv_to(int fd, char *filename) {
	return 0;
}

int Document::size(char *filename) {
	FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    return len;
}


#endif
