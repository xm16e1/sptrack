// DIBSection.cpp: CXDIBSection クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "XDIBSection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

/********************************************************
 * Func: CXDIBSection クラスのコンストラクタ
 * メンバ変数を初期化
 ********************************************************/
CXDIBSection::CXDIBSection()
{
	m_hBmp = NULL;
	m_pBits = NULL;
	m_sizeBmp = CSize(0, 0);
}

/********************************************************
 * Func: CXDIBSection クラスのデストラクタ
 * ビットマップを解放
 ********************************************************/
CXDIBSection::~CXDIBSection()
{
	if (m_hBmp)
		::DeleteObject(m_hBmp);		// これで m_pBits も解放される
}

/********************************************************
 * Func: BITMAPINFOHEADER の内容を設定
 * 常に 24 ビットカラー
 ********************************************************/
static void
FillBitmapInfoHeader(
	LPBITMAPINFOHEADER pbmih,		// (B) 内容を設定するバッファ
	CSize size						// (I) ビットマップのサイズ
)
{
	pbmih->biSize = sizeof(BITMAPINFOHEADER);
	pbmih->biWidth = size.cx;
	pbmih->biHeight = size.cy;
	pbmih->biPlanes = 1;
	pbmih->biBitCount = 24;				// 24 ビット固定
	pbmih->biCompression = BI_RGB;
	pbmih->biSizeImage = 0;
	pbmih->biXPelsPerMeter = 0;
	pbmih->biYPelsPerMeter = 0;
	pbmih->biClrUsed = 0;
	pbmih->biClrImportant = 0;
}

/********************************************************
 * Func: 指定されたサイズのビットマップを作成
 ********************************************************/
BOOL					// TRUE->OK、FALSE->エラー
CXDIBSection::Create(
	CDC *pdc,			// (I) ビットマップ作成に使う DC
	CSize size			// (I) 作成するビットマップのサイズ
)
{
	if (m_hBmp != NULL) {
		TRACE("CXDIBSection created twice\n");
		return FALSE;
	}

	BITMAPINFO bmi;

	m_sizeBmp = size;
	FillBitmapInfoHeader(&bmi.bmiHeader, size);
	m_hBmp = ::CreateDIBSection(pdc->GetSafeHdc(), &bmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL, 0);

	m_nRowStride = FitTo4Byte(3 * size.cx);

	return m_hBmp != NULL;
}

/********************************************************
 * Func: 4の倍数に切り上げる
 ********************************************************/
int				// 4の倍数に切り上げた結果
CXDIBSection::FitTo4Byte(
	int n				// (I) 元の数
)
{
	int nMod = n % 4;
	if (nMod != 0)
		n += (4 - nMod);

	return n;
}

/********************************************************
 * Func: 2の倍数に切り上げる
 ********************************************************/
int				// 2の倍数に切り上げた結果
CXDIBSection::FitTo2Byte(
	int n				// (I) 元の数
)
{
	if (n % 2 != 0)
		++n;

	return n;
}
/********************************************************
 * Func: ビットマップを伸縮して描画
 ********************************************************/
void
CXDIBSection::StretchDraw(
	CDC *pdc,					// (O) 描画する DC
	const CRect *prectDraw		// (I) 描画先の矩形
) const
{
	CDC dcMem;
	VERIFY(dcMem.CreateCompatibleDC(pdc));

	HGDIOBJ hBmpOld = ::SelectObject(dcMem.GetSafeHdc(), GetHBitmap());
	pdc->StretchBlt(prectDraw->left, prectDraw->top, prectDraw->Width(), prectDraw->Height(), &dcMem, 0, 0, m_sizeBmp.cx, m_sizeBmp.cy, SRCCOPY);
	::SelectObject(dcMem.GetSafeHdc(), hBmpOld);
}


void CXDIBSection::CopyToCADIBSection(CADIBSection *pAdib) const
{
	int x;
	int y;
	unsigned int col;

	if(pAdib->GetSizex()!=m_sizeBmp.cx || pAdib->GetSizey()!=m_sizeBmp.cy){
		pAdib->ChangeSize(m_sizeBmp.cx, m_sizeBmp.cy);
	}

	for(y=0; y<m_sizeBmp.cy; y++) {
		for(x=0; x<m_sizeBmp.cx; x++) {
			col=(unsigned int)GetPixel(x,y);
			pAdib->SetPixel(x, y, GetRValue(col), GetGValue(col), GetBValue(col));

		}
	}
}


void CXDIBSection::CopyFromCADIBSection(const CADIBSection *pAdib)
{
	int x;
	int y;
	for(y=0; y<m_sizeBmp.cy; y++) {
		for(x=0; x<m_sizeBmp.cx; x++) {

			SetPixel(x, y, 
				RGB(pAdib->GetValueR(x,y), pAdib->GetValueG(x,y), pAdib->GetValueB(x,y)));

		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// CreateFromBMP.cpp: Bitmap ファイルから作成
//
//////////////////////////////////////////////////////////////////////

/********************************************************
 * Func: BMP ファイルから画像を読み込む
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::ReadBmpFile(
	CDC *pdc,				// (I) ビットマップ作成に使う DC
	HANDLE hBmp,			// (I) 読み込み元のビットマップハンドル
	const CSize *pSize		// (I) このサイズにスケーリングして作成。NULLなら、BMPファイルと同じサイズ
)
{
	BITMAP bm;
	if (GetObject(hBmp, sizeof(bm), &bm) == 0)
		return FALSE;

	CSize sizeCreate;	// 作成するDIBのサイズ
	if (pSize != NULL)
		sizeCreate = *pSize;
	else
		sizeCreate = CSize(bm.bmWidth, bm.bmHeight);

	if (!Create(pdc, sizeCreate))
		return FALSE;

	BOOL bRet = FALSE;
	HDC hdcSrc	= ::CreateCompatibleDC(pdc->m_hDC);
	HDC hdcDest = ::CreateCompatibleDC(pdc->m_hDC);
	if (hdcSrc != NULL && hdcDest != NULL) {
		HGDIOBJ hbmpOrg1 = ::SelectObject(hdcSrc, hBmp);
		HGDIOBJ hbmpOrg2 = ::SelectObject(hdcDest, m_hBmp);
		if (pSize != NULL) {
			::SetStretchBltMode(hdcDest, HALFTONE);
			::StretchBlt(hdcDest, 0, 0, sizeCreate.cx, sizeCreate.cy, hdcSrc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
		}
		else
			::BitBlt(hdcDest, 0, 0, sizeCreate.cx, sizeCreate.cy, hdcSrc, 0, 0, SRCCOPY);
		::SelectObject(hdcSrc,	hbmpOrg1);
		::SelectObject(hdcDest, hbmpOrg2);
		bRet = TRUE;
	}

	if (hdcSrc != NULL)
		DeleteDC(hdcSrc);
	if (hdcDest != NULL)
		DeleteDC(hdcDest);

	return bRet;
}

/********************************************************
 * Func: BMP ファイルから DIB を作成する
 * フルカラー BMP ファイルのみ対応
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::CreateFromBMPFile(
	CDC *pdc,				// (I) ビットマップ作成に使う DC
	LPCSTR lpszFileName,	// (I) BMP ファイル名
	const CSize *pSize		// (I) このサイズにスケーリングして作成。NULLなら、BMPファイルと同じサイズ
)
{
	HANDLE hBmp = LoadImage(NULL, lpszFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBmp == NULL)
		return FALSE;

	BOOL bRet = ReadBmpFile(pdc, hBmp, pSize);
	DeleteObject(hBmp);

	return bRet;
}

/********************************************************
 * Func: ビットマップファイルにセーブ
 ********************************************************/
BOOL						// TRUE->OK、FALSE->エラー
CXDIBSection::SaveToBitmapFile(
	LPCSTR lpszFileName		// (I) セーブするファイル名
) const
{
	BOOL bRet = TRUE;
	try {
		CStdioFile f(lpszFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		DIBSECTION dibs;
		GetObject(m_hBmp, sizeof(dibs), &dibs);

		DWORD dwImageSize = m_nRowStride * dibs.dsBmih.biHeight;
		ASSERT(dibs.dsBmih.biSizeImage == 0 || dibs.dsBmih.biSizeImage == dwImageSize);

		BITMAPFILEHEADER bmfh;
		ZeroMemory(&bmfh, sizeof(bmfh));
		bmfh.bfType = *(WORD *)"BM";
		bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwImageSize;
		bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		f.Write(&bmfh, sizeof(bmfh));

		f.Write(&dibs.dsBmih, sizeof(BITMAPINFOHEADER));

		f.Write(m_pBits, dwImageSize);

	}
	catch (CFileException *e) {
		bRet = FALSE;
		CHAR	szCause[256];
		e->GetErrorMessage(szCause, 256);
		TRACE("Error: %s", szCause);
		e->Delete();
	}

	return bRet;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// JPEG.cpp: JPEG インターフェース関連関数
//
//////////////////////////////////////////////////////////////////////

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
