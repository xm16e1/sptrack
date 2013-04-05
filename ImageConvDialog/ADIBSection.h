// ADIBSection.h: CADIBSection クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADIBSECTION_H__7EAE441E_61FF_4D9D_A52C_2E470E2D225A__INCLUDED_)
#define AFX_ADIBSECTION_H__7EAE441E_61FF_4D9D_A52C_2E470E2D225A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CADIBSection  
{
public:
	CADIBSection();
	virtual ~CADIBSection();
	CADIBSection(const int sizex, const int sizey);
	CADIBSection(const CADIBSection *pAdib);

	void Create(const int sizex, const int sizey);
	void ChangeSize(const int sizex, const int sizey);

	void CopyCADIB(const CADIBSection *pAdib);
	void ResizeCopyCADIB(const int sizex, const int sizey, const CADIBSection *pAdib, const int method=0);
	void ExtractCopyCADIB(const int x1, const int y1, const int x2, const int y2, const CADIBSection *pAdib) ;
	void Trimming(const int x1, const int y1, const int x2, const int y2);

	void Clear();

	int GetSizex() const {return m_sizex;};
	int GetSizey() const {return m_sizey;};
	inline void CADIBSection::SetPixel(const int x, const int y, 
		const unsigned char colR, const unsigned char colG, const unsigned char colB);
//	inline unsigned int  GetPixel(const int x, const int y) const;

	inline unsigned char GetValueR(const int x, const int y) const;
	inline unsigned char GetValueG(const int x, const int y) const;
	inline unsigned char GetValueB(const int x, const int y) const;

	inline double GetValueI(const int x, const int y) const;

//	inline unsigned int XRGB(unsigned char r, unsigned char g, unsigned char b) const;
	unsigned char *GetArrayAddress() const {return m_array;};
	int GetArrayByteSize() const {return m_sizex*m_sizey*3;};

	void SetColor(const int col);

	inline void GetValue(const int x, const int y,
		unsigned char &r, unsigned char &g, unsigned char &b) const;

	void AbsDifference(const CADIBSection *pAdib1, const CADIBSection *pAdib2);
	void Superimpose(const CADIBSection *pAdib1, const CADIBSection *pAdib2, const double ratio=0.5);

	void Line(double x1, double y1, double x2, double y2,
		const double width, const unsigned int r, const unsigned int g, const unsigned int b);
	void Text(const char *text, const double x0, const double y0, const double height,
		const unsigned int r, const unsigned int g, const unsigned int b);
	void Rect(const int x, const int y, const int length, 
		const unsigned int r, const unsigned int g, const unsigned int b);
	void Clip(double &val, const double upperLimit);

	void Grayscale();
	void FlipImage();
	void Average4Pixels();

	void GetValueHSV(const int x, const int y, double &H, double &S, double &V) const;
	void GetVHDEdge(const int x, const int y, int &edgeV, int &edgeH, int &edgeD) const;

	void Draw2DGraph(const double *buff, const int sizex, const int sizey, 
			const int nLevel, const double *aValue, const COLORREF *aColor,
			const double scale, const int flag);
	void Draw2DGraphColor(const double value, 
			const int nLevel, const double *aValue, const COLORREF *aColor,
			unsigned char &r, unsigned char &g, unsigned char &b);

	void Gradient(const int x, const int y, double *val, double *angle);

private:
	int m_sizex;
	int m_sizey;
	unsigned char *m_array;

};

inline void CADIBSection::SetPixel(const int x, const int y, 
		const unsigned char colR, const unsigned char colG, const unsigned char colB)
{
	m_array[(y*m_sizex+x)*3+0]=colR;
	m_array[(y*m_sizex+x)*3+1]=colG;
	m_array[(y*m_sizex+x)*3+2]=colB;
}
/*
inline unsigned int CADIBSection::GetPixel(const int x, const int y) const 
{
	return m_array[y*m_sizex+x];
}
*/
inline unsigned char CADIBSection::GetValueR(const int x, const int y) const
{
	return m_array[(y*m_sizex+x)*3 +0];
}

inline unsigned char CADIBSection::GetValueG(const int x, const int y) const
{
	return m_array[(y*m_sizex+x)*3 +1];
}

inline unsigned char CADIBSection::GetValueB(const int x, const int y) const
{
	return m_array[(y*m_sizex+x)*3 +2];
}

inline void CADIBSection::GetValue(const int x, const int y,
							  unsigned char &r, unsigned char &g, unsigned char &b) const	//(x,y)におけるRGBを返す
{
	int pos=(y*m_sizex+x)*3;
	r=m_array[pos+0];
	g=m_array[pos+1];
	b=m_array[pos+2];
}

inline double CADIBSection::GetValueI(const int x, const int y) const
{
	double intensity=	(double)m_array[(y*m_sizex+x)*3 +0]*0.299 +
						(double)m_array[(y*m_sizex+x)*3 +1]*0.587 +
						(double)m_array[(y*m_sizex+x)*3 +2]*0.114;
	return intensity;
}




/*
inline unsigned int CADIBSection::XRGB(unsigned char r, unsigned char g, unsigned char b) const
{ 
	return (unsigned int)r | (unsigned int)g <<8 | (unsigned int) b<<16;
}
*/
#endif // !defined(AFX_ADIBSECTION_H__7EAE441E_61FF_4D9D_A52C_2E470E2D225A__INCLUDED_)
