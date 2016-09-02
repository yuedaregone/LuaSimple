#ifndef __IMAGE_H__
#define __IMAGE_H__

typedef struct
{
	unsigned char* data;
	int size;
	int offset;
}tImageSource;

class MImage
{
public:
	typedef enum
	{
		typePNG = 0,
		typeUnkown
	}MImageType;
	MImage();
	~MImage();
public:
	bool initWithFile(const char* fileName, MImageType _type);
	unsigned char* getData() { return m_pData; }
	unsigned short getWidth() { return m_nWidth; }
	unsigned short getHeight() { return m_nHeight; }
	size_t getDataSize();
	bool isHasAlpha() { return m_bHasAlpha; }
private:
	bool initWithPngData(void* _data, size_t _len);
private:
	unsigned char* m_pData;
	unsigned short m_nHeight;
	unsigned short m_nWidth;
	int m_nBitsPerComponent;
	bool m_bHasAlpha;
	bool m_bPreMulti;
};
#endif