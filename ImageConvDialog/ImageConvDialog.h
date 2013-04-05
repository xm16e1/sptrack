// ImageConvDialog.h : PROJECT_NAME アプリケーションのメイン ヘッダー ファイルです。
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"		// メイン シンボル


// CImageConvDialogApp:
// このクラスの実装については、ImageConvDialog.cpp を参照してください。
//

class CImageConvDialogApp : public CWinApp
{
public:
	CImageConvDialogApp();

// オーバーライド
	public:
	virtual BOOL InitInstance();

// 実装

	DECLARE_MESSAGE_MAP()
};

extern CImageConvDialogApp theApp;