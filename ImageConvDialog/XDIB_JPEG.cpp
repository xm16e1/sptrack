// JPEG.cpp: JPEG インターフェース関連関数
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XDIBSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <setjmp.h>

extern "C" {
	#include <jpeglib.h>
}

// C++ で setjmp を使うと警告がでるが、JPEG ライブラリを使うには setjmp が
// 必要なので、この警告を出さないようにする
#pragma warning(disable : 4611)

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;


METHODDEF(void)
CXDIBSection_JPEGErrorExit(j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
//	(*cinfo->err->output_message) (cinfo);	// エラーメッセージはここでは表示しない

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

/********************************************************
 * Func: JPEG ライブラリの走査線バッファから、１行分のデータを取得
 ********************************************************/
void
CXDIBSection::GetLineBitsFromJPEGScanline(
	int y,							// (I) y 座標
	const BYTE *pbyJPEGScanLine,	// (I) JPEG ライブラリの走査線バッファ
	int nComponents					// (I) 色成分数
)
{
	ASSERT(m_pBits);
	ASSERT(0 <= y && y < m_sizeBmp.cy);
	ASSERT(pbyJPEGScanLine);
	ASSERT(nComponents == 3 || nComponents == 1);

	y = m_sizeBmp.cy - y - 1;	// 上下反転
	BYTE *pbyBmpScanLine = m_pBits + m_nRowStride * y;	// 1 ピクセル 3 バイト固定
	if (nComponents == 3) {				// FullColor JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			*pbyBmpScanLine++ = pbyJPEGScanLine[2];
			*pbyBmpScanLine++ = pbyJPEGScanLine[1];
			*pbyBmpScanLine++ = pbyJPEGScanLine[0];
			pbyJPEGScanLine += 3;
		}
	}
	else {								// GrayScale JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			*pbyBmpScanLine++ = *pbyJPEGScanLine;
			*pbyBmpScanLine++ = *pbyJPEGScanLine;
			*pbyBmpScanLine++ = *pbyJPEGScanLine;
			pbyJPEGScanLine++;
		}
	}
}

/********************************************************
 * Func: JPEG ライブラリの走査線バッファに、１行分のデータを設定
 ********************************************************/
void
CXDIBSection::SetLineBitsToJPEGScanline(
	int y,						// (I) y 座標
	BYTE *pbyJPEGScanLine,		// (B) JPEG ライブラリの走査線バッファ
	int nComponents				// (I) 色成分数
) const
{
	ASSERT(m_pBits);
	ASSERT(0 <= y && y < m_sizeBmp.cy);
	ASSERT(pbyJPEGScanLine);
	ASSERT(nComponents == 3 || nComponents == 1);

	y = m_sizeBmp.cy - y - 1;	// 上下反転
	const BYTE *pbyBmpScanLine = m_pBits + m_nRowStride * y;	// 1 ピクセル 3 バイト固定
	if (nComponents == 3) {				// FullColor JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			pbyJPEGScanLine[2] = *pbyBmpScanLine++;
			pbyJPEGScanLine[1] = *pbyBmpScanLine++;
			pbyJPEGScanLine[0] = *pbyBmpScanLine++;
			pbyJPEGScanLine += 3;
		}
	}
	else {								// GrayScale JPEG
		for (int x = 0; x < m_sizeBmp.cx; x++) {
			*pbyJPEGScanLine = *pbyBmpScanLine;
			pbyJPEGScanLine++;
			pbyBmpScanLine += 3;
		}
	}
}

/********************************************************
 * Func: JPEG ファイルから DIB を作成
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::CreateFromJPEGFile(
	CDC *pdc,				// (I) ビットマップ作成に使う DC
	LPCSTR lpszFileName		// (I) JPEG ファイル名
)
{
	FILE *fpIn = fopen(lpszFileName, "rb");
	if (fpIn == NULL)
		return FALSE;

	BOOL bResult = CreateFromJPEGFile(pdc, fpIn);
	fclose(fpIn);

	return bResult;
}

/********************************************************
 * Func: JPEG ファイルから DIB を作成
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::CreateFromJPEGFile(
	CDC *pdc,				// (I) ビットマップ作成に使う DC
	FILE *fpIn				// (I) JPEG ファイル読み込み用のファイルポインタ
)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	BYTE *pRowBuf = NULL;	// jpeg データ１行読み込み用バッファ

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = CXDIBSection_JPEGErrorExit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_decompress(&cinfo);
		if (pRowBuf) free(pRowBuf);
		return FALSE;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fpIn);
	jpeg_read_header(&cinfo, TRUE);
	if (Create(pdc, CSize(cinfo.image_width, cinfo.image_height)) == NULL) {
		jpeg_destroy_decompress(&cinfo);
		return FALSE;
	}

	jpeg_start_decompress(&cinfo);

	int nRowBufSize = cinfo.output_width * cinfo.output_components;
	if ((pRowBuf = (BYTE *)malloc(nRowBufSize)) == NULL) {
		jpeg_destroy_decompress(&cinfo);
		return FALSE;
	}
	BYTE *aRowArray[1];			// 走査線の配列(１本だけ)
	aRowArray[0] = pRowBuf;
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, aRowArray, 1);		// JEPG から１行読む
		GetLineBitsFromJPEGScanline(cinfo.output_scanline - 1, pRowBuf, cinfo.output_components);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	free(pRowBuf);

	return TRUE;
}

/********************************************************
 * Func:  DIB を JPEG ファイルにセーブ
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::SaveToJPEGFile(
	LPCSTR lpszFileName,	// (I) セーブするファイル名
	int nQuality,			// (I) 品質(0～100)
	const CRect *prect		// (I) セーブする長方形(NULL なら全体)
) const
{
	FILE *fpOut = fopen(lpszFileName, "wb");
	if (fpOut == NULL)
		return FALSE;

	BOOL bResult = SaveToJPEGFile(fpOut, nQuality, prect);
	fclose(fpOut);

	return bResult;
}

/********************************************************
 * Func:  DIB を JPEG ファイルにセーブ
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::SaveToJPEGFile(
	FILE *fpOut,			// (I) JPEG ファイルを書き込むファイルポインタ
	int nQuality,			// (I) 品質(0～100)
	const CRect *prect		// (I) セーブする長方形(top <= y <= bottom をセーブ、x も同様、NULL なら全体)
) const
{
	const DWORD dwWidth = m_sizeBmp.cx;			// ビットマップの幅(Pixel)
	const DWORD dwHeight = m_sizeBmp.cy;		// ビットマップの高さ(Pixel)
	const DWORD dwFileWidth	 = prect ? prect->Width()  + 1 : dwWidth;	// セーブするビットマップの幅(Pixel)
	const DWORD dwFileHeight = prect ? prect->Height() + 1 : dwHeight;	// セーブするビットマップの高さ(Pixel)
	const DWORD dwFileStartX = prect ? prect->left : 0;				// セーブする左端
	const DWORD dwFileStartY = prect ? prect->top  : 0;				// セーブする上

	if (dwFileStartX + dwFileWidth > dwWidth || dwFileStartY + dwFileHeight > dwHeight)
		return FALSE;

	BYTE *pbyScanLineBuf = (BYTE *)malloc(3 * dwWidth);
	if (pbyScanLineBuf == NULL) {
		return FALSE;
	}

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = CXDIBSection_JPEGErrorExit;
	if (setjmp(jerr.setjmp_buffer)) {
		jpeg_destroy_compress(&cinfo);
		if (pbyScanLineBuf) free(pbyScanLineBuf);
		return FALSE;
	}

	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, fpOut);
	cinfo.image_width = dwFileWidth;
	cinfo.image_height = dwFileHeight;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;		/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, nQuality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);

	BYTE *aRowArray[1];			// 走査線の配列(１本だけ)
	aRowArray[0] = pbyScanLineBuf + cinfo.input_components * dwFileStartX;
	while (cinfo.next_scanline < cinfo.image_height) {
		SetLineBitsToJPEGScanline(cinfo.next_scanline + dwFileStartY, pbyScanLineBuf, cinfo.input_components);
		jpeg_write_scanlines(&cinfo, aRowArray, 1);
	}
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	free(pbyScanLineBuf);

	return TRUE;
}
