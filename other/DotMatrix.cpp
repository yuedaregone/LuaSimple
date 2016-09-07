// DotMatrix.cpp : 定义控制台应用程序的入口点。
//
#include <iostream>
#include <map>
#include <vector>
#include "MImage.h"
#include <fstream>
#include <stdlib.h>
#include <string.h>

int read_config(std::string& path, std::map<int, std::vector<char> >& ascalls)
{
	FILE* fp = fopen(path.c_str(), "r+");
	if (fp == NULL)
	{
		printf("Can not found config file!");
		return -1;
	}

	char buff[256];
	while (fgets(buff, 256, fp) != NULL)
	{
		std::string line = buff;
		int index = line.find('|');
		int num = atoi(line.substr(0, index).c_str());
		
		std::vector<char> vAscal;
		const char* ascal = line.substr(index + 1, line.length()).c_str();		
		for (int i = 0; i < strlen(ascal); ++i)
		{
			if (num == 0 || ascal[i] != ' ')
			{
				vAscal.push_back(ascal[i]);
			}
		}

		if (vAscal.size() > 0)
		{
			ascalls[num] = vAscal;
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	char buff[1024] = { 0 };
	std::string strPath = argv[0];
	strPath = strPath.substr(0, strPath.rfind("/"));

	std::string config = strPath + "/config";	
	std::map<int, std::vector<char> > ascallGray;
	read_config(config, ascallGray);	

	std::string png_file = argv[1];
	std::string subfix = png_file.substr(png_file.find_last_of(".") + 1, png_file.length());
	if (subfix.compare("png") != 0)
	{
		printf("Input file must be png!");
		return -1;
	}

	std::string fileTxt = png_file;
	fileTxt = fileTxt.substr(0, fileTxt.find_last_of("."));
	fileTxt = strPath + "/" + fileTxt.substr(fileTxt.find_last_of("/")+1, fileTxt.length()) + ".txt";
		
	MImage img;
	img.initWithFile(png_file.c_str(), MImage::typePNG);
	unsigned char* _data = img.getData();
	size_t _size = img.getDataSize();
	unsigned short w = img.getWidth();
	int off = 3;
	if (img.isHasAlpha())
	{
		off = 4;
	}

	FILE* stream = fopen(fileTxt.c_str(), "w+");
	int k = 0;
	int h = w*off;
	for (int i = 0; i < _size; i += off)
	{
		if (i != 0 && i % h == 0)
		{
			k++;
			fputs("\n", stream);
			printf("\n");
			if (k == 1)
			{
				i += h;
				if (i > _size || (_size - i < h))
				{
					break;
				}
				k = 0;
			}
		}

		int grayMax = ascallGray.rbegin()->first;
		float gray = (_data[i] * 30 + _data[i + 1] * 59 + _data[i + 2] * 11) / 100.0f;
		int index = (int)(gray / 255.0f*grayMax * _data[i + 3] / 255.0f);
		index = index > grayMax ? grayMax : index;		

		std::map<int, std::vector<char> >::iterator it = ascallGray.find(index);
		if (it != ascallGray.end())
		{			
			fputc(it->second[0], stream);
			printf("%c", it->second[0]);
		}
		else
		{
			bool isFind = false;
			do
			{
				if ((it = ascallGray.find(--index)) != ascallGray.end())
				{
					isFind = true;
					fputc(it->second[0], stream);
					printf("%c", it->second[0]);
				}
			} while (!isFind && index >= 0);
			if (!isFind)
			{
				fputc(ascallGray.begin()->second[0], stream);
				printf("%c", ascallGray.begin()->second[0]);
			}
		}		
	}
	fclose(stream);
	return 0;
}

