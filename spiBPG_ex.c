/*
SUSIE32 '00IN' Plug-in for BPG
*/

#include <inttypes.h>
#include "spiBPG_ex.h"

/* エントリポイント */
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}


/***************************************************************************
 * 画像形式依存の処理.
 * ～Ex関数を画像形式に合わせて書き換える.
 ***************************************************************************/
 
/*
ヘッダを見て対応フォーマットか確認.
対応しているものならtrue,そうでなければfalseを返す.
filnameはファイル名(NULLが入っていることもある).
dataはファイルのヘッダで,サイズは HEADBUF_SIZE.
*/
BOOL IsSupportedEx(char *filename, char *data)
{

	if (data[0] != 'B' ||data[1] != 'P' ||data[2] != 'G'||data[3] != (char)0xFB)
        return FALSE;
	return TRUE;
}

/*
画像ファイルの情報を書き込みエラーコードを返す.
dataはファイルイメージで,サイズはdatasizeバイト.
*/
int GetPictureInfoEx(char *data, struct PictureInfo *lpInfo)
{

	BPGImageInfo img_info_s, *img_info = &img_info_s;
	int ret = bpg_decoder_get_info_from_buf(img_info, NULL, (uint8_t *)data, 16);
	if (ret < 0) {
		return SPI_NOT_SUPPORT;
	}

	lpInfo->left		= 0;
	lpInfo->top			= 0;
	lpInfo->width		= img_info->width;
	lpInfo->height		= img_info->height;
	lpInfo->x_density	= 0;
	lpInfo->y_density	= 0;
	lpInfo->colorDepth	= 24;
	lpInfo->hInfo		= NULL;

	return SPI_ALL_RIGHT;
}
void rgb2bgr(uint8_t* rgb){
	uint8_t tmp = rgb[0];
	rgb[0]=rgb[2];
	rgb[2]=tmp;
}
/*
画像ファイルをDIBに変換し,エラーコードを返す.
dataはファイルイメージで,サイズはdatasizeバイト.
*/
int GetPictureEx(long datasize, HANDLE *pHBInfo, HANDLE *pHBm,
			 SPI_PROGRESS lpPrgressCallback, long lData, char *data)
{
	unsigned int infosize;
	int height,y;
	unsigned char tmp;
	unsigned int width, bit, linesize, imgsize,x;
	BITMAPINFO *pinfo;
	unsigned char *pbmdat;
	BMP_FILE *pbmp = (BMP_FILE *)data;

	if (lpPrgressCallback != NULL)
		if (lpPrgressCallback(0, 1, lData)) /* 0% */
			return SPI_ABORT;

	BPGImageInfo img_info_s, *img_info = &img_info_s;
	BPGDecoderContext *img;
	img = bpg_decoder_open();
	if (bpg_decoder_decode(img, (uint8_t *)data, datasize) < 0) {
		bpg_decoder_close(img);
		return SPI_NOT_SUPPORT;
	}
	if(bpg_decoder_get_info(img, img_info) < 0){
		bpg_decoder_close(img);
		return SPI_NOT_SUPPORT;
	}

	width = img_info->width;
	height = img_info->height;
	bit = 24;
	infosize = sizeof(BITMAPINFOHEADER);

	linesize = (width * (bit / 8) +3) & ~3; /*4byte境界*/
	imgsize = linesize * abs(height);

	*pHBInfo = LocalAlloc(LMEM_MOVEABLE, infosize);
	*pHBm    = LocalAlloc(LMEM_MOVEABLE, imgsize);

	if (*pHBInfo == NULL || *pHBm == NULL) {
		if (*pHBInfo != NULL) LocalFree(*pHBInfo);
		if (*pHBm != NULL) LocalFree(*pHBm);
		bpg_decoder_close(img);
		return SPI_NO_MEMORY;
	}

	pinfo  = (BITMAPINFO *)LocalLock(*pHBInfo);
	pbmdat = (unsigned char *)LocalLock(*pHBm);
	if (pinfo == NULL || pbmdat == NULL) {
		LocalFree(*pHBInfo);
		LocalFree(*pHBm);
		bpg_decoder_close(img);
		return SPI_MEMORY_ERROR;
	}

	/* *pHBInfoにはBITMAPINFOを入れる */
	pinfo->bmiHeader.biSize				= sizeof(BITMAPINFOHEADER);
	pinfo->bmiHeader.biWidth			= width;
	pinfo->bmiHeader.biHeight			= height;
	pinfo->bmiHeader.biPlanes			= 1;
	pinfo->bmiHeader.biBitCount			= bit;
	pinfo->bmiHeader.biCompression		= BI_RGB;
	pinfo->bmiHeader.biSizeImage		= 0;
	pinfo->bmiHeader.biXPelsPerMeter	= 0;
	pinfo->bmiHeader.biYPelsPerMeter	= 0;
	pinfo->bmiHeader.biClrUsed			= 0;
	pinfo->bmiHeader.biClrImportant		= 0;

	
	/* *pHBmにはビットマップデータを入れる */
	//memcpy(pbmdat, data +pbmp->header.bfOffBits, imgsize);
	if( bpg_decoder_start(img, BPG_OUTPUT_FORMAT_RGB24) < 0 ){
		//メモリをアンロック
		LocalUnlock(*pHBInfo);
		LocalUnlock(*pHBm);
		LocalFree(*pHBInfo);
		LocalFree(*pHBm);
		bpg_decoder_close(img);
		return SPI_MEMORY_ERROR;
	}

	for (y = height-1; y >= 0 ; --y){
		if(bpg_decoder_get_line(img, pbmdat + linesize*y) < 0)
			break;
		for (x=0; x < width; x++) {
			rgb2bgr( pbmdat + linesize*y + x*3 );
		}
	}
	bpg_decoder_close(img);

	//メモリをアンロック
	LocalUnlock(*pHBInfo);
	LocalUnlock(*pHBm);

	if (lpPrgressCallback != NULL)
		if (lpPrgressCallback(1, 1, lData)) /* 100% */
			return SPI_ABORT;

	return SPI_ALL_RIGHT;
}
//---------------------------------------------------------------------------
