/*
SUSIE32 '00IN' Plug-in for BPG
*/

#ifndef spiBPG_ex_h
#define spiBPG_ex_h

#include "spi00in.h"
#include "libbpg.h"

/*-------------------------------------------------------------------------*/
/* この Plugin の情報 */
/*-------------------------------------------------------------------------*/
static const char *pluginfo[] = {
	"00IN",				/* Plug-in API バージョン */
	"BPG to DIB libbpg-" CONFIG_BPG_VERSION,		/* Plug-in名,バージョン及び copyright */
	"*.bpg",			/* 代表的な拡張子 ("*.JPG" "*.JPG;*.JPEG" など) */
	"BPG file(*.BPG)",	/* ファイル形式名 */
};

#pragma pack(push)
#pragma pack(1) //構造体のメンバ境界を1バイトにする
typedef struct BMP_FILE {
	BITMAPFILEHEADER	header;
	BITMAPINFOHEADER	info;
	RGBQUAD				pal[1];
} BMP_FILE;
#pragma pack(pop)

//ヘッダチェック等に必要なサイズ.2KB以内で.
#define HEADBUF_SIZE BPG_DECODER_INFO_BUF_SIZE

BOOL IsSupportedEx(char *filename, char *data);
int GetPictureInfoEx(char *data, struct PictureInfo *lpInfo);
int GetPictureEx(long datasize, HANDLE *pHBInfo, HANDLE *pHBm,
			 SPI_PROGRESS lpPrgressCallback, long lData, char *data);

#endif
