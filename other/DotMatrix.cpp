// DotMatrix.cpp : 定义控制台应用程序的入口点。
//
#include <windows.h>
#include <locale.h>
#include <iostream>
#include <map>
#include <vector>
#include "MImage.h"
#include <fstream>
//#include <tchar.h>
//#include "string_common.h"
//#include <atlstr.h>

void fun_calcGrayAscall(std::map<int, std::vector<char> >& ascallGray)
{
	ascallGray.clear();
	std::vector<char> v0;
	v0.push_back(32);
	ascallGray.insert(std::map<int, std::vector<char> >::value_type(0, v0));

	HDC dc = GetDC(NULL);
	LOGFONT font;
	font.lfHeight = 16;
	font.lfWidth = 16;
	font.lfEscapement = 0;
	font.lfOrientation = 0;
	font.lfWeight = FW_THIN;
	font.lfItalic = false;
	font.lfUnderline = false;
	font.lfStrikeOut = false;
	font.lfCharSet = CHINESEBIG5_CHARSET;
	font.lfOutPrecision = OUT_CHARACTER_PRECIS;
	font.lfClipPrecision = CLIP_CHARACTER_PRECIS;
	font.lfQuality = DEFAULT_QUALITY;
	font.lfPitchAndFamily = FF_MODERN;
	memcpy(font.lfFaceName, "宋体", sizeof("宋体") / sizeof(char));
	setlocale(LC_ALL, "chs");
	HFONT hFont = CreateFontIndirect(&font);
	HFONT oldFont = (HFONT)SelectObject(dc, hFont);

	for (int i = 33; i < 127; ++i)
	{
		//if ((i >= 'A' && i <= 'Z') || (i >= 'a' && i <= 'z') || (i >= '0' && i <= '9'))
		{
		//	continue;
		}
		char ch = i;
		MAT2 mat2 = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };

		GLYPHMETRICS gm = { 0 };
		DWORD retByte = GetGlyphOutline(dc, ch, GGO_BITMAP, &gm, 0, NULL, &mat2);
		if (retByte == GDI_ERROR)
		{
			printf("error");
			return;
		}
		unsigned char* buffStr = new unsigned char[retByte];
		memset(buffStr, 0, retByte);
		GetGlyphOutline(dc, ch, GGO_BITMAP, &gm, retByte, buffStr, &mat2);

		int dotNum = 0;
		int nByteCount = ((gm.gmBlackBoxX + 31) >> 5) << 2;
		for (int i = 0; i < gm.gmBlackBoxY; ++i)
		{
			for (int j = 0; j < nByteCount; ++j)
			{
				BYTE w = *((BYTE*)buffStr + i * nByteCount + j);
				for (int k = 8; k > 0; --k)
				{
					BYTE index = 1 << (k - 1);
					BYTE temp = w & index;
					if (temp > 0)
						++dotNum;
				}
			}
		}
		ascallGray[dotNum].push_back(ch);
	}

	SelectObject(dc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, dc);
}

void findAllFile(std::vector<std::string>& vStrFile, std::string lpPath)
{
	std::string szFind;
	std::string szFile;

	WIN32_FIND_DATA FindFileData;
	szFind = lpPath;
	szFind = szFind + "\\*.*";

	HANDLE hFind = ::FindFirstFile(szFind.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind) return;
	while (::FindNextFile(hFind, &FindFileData))
	{
		std::string fileName = FindFileData.cFileName;
		if (fileName.compare(".") == 0 || fileName.compare("..") == 0)
		{
			continue;
		}
		szFile = lpPath;
		szFile = szFile + "\\";
		szFile = szFile + fileName;

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			findAllFile(vStrFile, szFile);
		}
		else
		{
			int index = fileName.find('.');
			std::string houzhui = fileName.substr(index, fileName.length());//fileName.Right(fileName.GetLength() - index);
			if (houzhui.compare(".png") == 0)
			{
				vStrFile.push_back(szFile);
			}
		}
	}
	FindClose(hFind);
}

int main(int argc, char* argv[])
{
	std::map<int, std::vector<char> > ascallGray;
	fun_calcGrayAscall(ascallGray);


	char buff[1024] = { 0 };
	std::string strPath = argv[0];
	strPath = strPath.substr(0, strPath.rfind("\\"));

	std::vector<std::string> vFiles;
	findAllFile(vFiles, strPath.c_str());

	std::vector<std::string>::iterator it = vFiles.begin();
	for (; it != vFiles.end(); ++it)
	{
		std::string& fileTxt = *it;
		fileTxt = fileTxt.substr(0, fileTxt.find_last_of("."));
		fileTxt = fileTxt.substr(fileTxt.find_last_of("\\")+1, fileTxt.length());
		fileTxt += ".txt";

		MImage img;
		img.initWithFile(it->c_str(), MImage::typePNG);
		BYTE* _data = img.getData();
		size_t _size = img.getDataSize();
		unsigned short w = img.getWidth();
		int off = 3;
		if (img.isHasAlpha())
		{
			off = 4;
		}
		//std::ofstream stream(fileTxt);
		FILE* stream = fopen(fileTxt.c_str(), "w+");
		int k = 0;
		int h = w*off;
		for (int i = 0; i < _size; i += off)
		{
			if (i != 0 && i % h == 0)
			{
				k++;
				//stream << "\n";
				fputs("\n", stream);
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
			int index = (int)(gray / 255.0f*grayMax);
			index = index > grayMax ? grayMax : index;
			index = grayMax - index;

			std::map<int, std::vector<char> >::iterator it = ascallGray.find(index);
			if (it != ascallGray.end())
			{
				//stream << it->second.back();
				fputc(it->second.back(), stream);
			}
			else
			{
				bool isFind = false;
				do
				{
					if ((it = ascallGray.find(--index)) != ascallGray.end())
					{
						isFind = true;
						//stream << it->second.back();
						fputc(it->second.back(), stream);
					}
				} while (!isFind && index >= 0);
				if (!isFind)
				{
					//stream << ascallGray.begin()->second.back();
					fputc(ascallGray.begin()->second.back(), stream);
				}
			}
		}
		fclose(stream);
		//stream.close();
	}
	return 0;
}

