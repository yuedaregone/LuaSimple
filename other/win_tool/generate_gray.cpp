// DotMatrix.cpp : 定义控制台应用程序的入口点。
//
#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <locale.h>
#include <iostream>
#include <map>
#include <vector>
#include <fstream>

void fun_turnWordToDot()
{
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
	wmemcpy(font.lfFaceName, L"宋体", sizeof(L"宋体") / sizeof(wchar_t));

	setlocale(LC_ALL, "chs");
	wprintf(L"字体大小:\n");
	int fontSize = 16;
	std::cin >> fontSize;
	getchar();
	font.lfHeight = font.lfWidth = fontSize;
	wchar_t ch = L'蔡';

	while (1)
	{
		wscanf(L"%c", &ch);
		if (ch == 0)
		{
			break;
		}
		wchar_t ch1 = 0;
		while ((ch1 = getchar()) != '\n')
			continue;
		HFONT hFont = CreateFontIndirect(&font);
		HFONT oldFont = (HFONT)SelectObject(dc, hFont);
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
					{
						printf("*");
					}
					else
					{
						printf(".");
					}
				}
			}
			printf("\n");
		}

		SelectObject(dc, oldFont);
		DeleteObject(hFont);
	}
	ReleaseDC(NULL, dc);
}

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
	wmemcpy(font.lfFaceName, L"宋体", sizeof(L"宋体") / sizeof(wchar_t));
	setlocale(LC_ALL, "chs");
	HFONT hFont = CreateFontIndirect(&font);
	HFONT oldFont = (HFONT)SelectObject(dc, hFont);

	for (int i = 33; i < 127; ++i)
	{
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

int _tmain(int argc, _TCHAR* argv[])
{
	fun_turnWordToDot();

	std::map<int, std::vector<char> > ascallGray;
	fun_calcGrayAscall(ascallGray);

	wchar_t* pathFile = argv[0];
	char buff[1024] = { 0 };
	wcstombs(buff, pathFile, 1024);
	std::string strPath = buff;
	strPath = strPath.substr(0, strPath.rfind("\\"));
	
	std::ofstream stream(strPath + "\\config");
	std::map<int, std::vector<char> >::iterator it = ascallGray.begin();
	for (; it != ascallGray.end(); ++it)
	{
		stream << it->first;
		stream << '|';
		std::vector<char>::iterator itr = it->second.begin();
		for (; itr != it->second.end(); ++itr)
		{
			stream << *itr;
			stream << " ";
		}
		stream << "\n";
	}				
	stream.close();	
	return 0;
}

