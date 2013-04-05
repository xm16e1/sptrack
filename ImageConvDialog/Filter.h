#pragma once
#include "ADIBSection.h"


class CFilter
{
public:
	CFilter(void);
	~CFilter(void);

	CFilter(CADIBSection *pAdib);
	void SetFilterType(int ftype);
	void DoFiltering(CADIBSection *pAdib);
	void WriteDIB(CADIBSection *pAdib);
	void SetSharpPara(int param);
	void SetMosaicBlock(int block);
//	void KernelInit(int ksize){ m_ikernelsize = ksize; };
private:
	CADIBSection *m_ptmp;
	CADIBSection *del;
	int m_filtertype;
	double m_kernel[9];	//カーネルサイズをを固定してある
	int m_divoption;
	int m_sp;
	int m_mosaicblock;
	double *mosaic;
};
