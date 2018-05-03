#ifndef   _MEANSHIFT_KALMAN_H
#define   _MEANSHIFT_KALMAN_H

#include "Kalman.h"
#include "prehead.h"

typedef struct _tmpVariable{
	double *pKernel ;		/* 申请核函数内存 */
	double *pW_i;			/* 申请权重内存 */
	double *pIndex_weight;	/* 申请索引权重空间 */
	double *pColorHist;		/* 申请跟踪目标的彩色直方图空间 */
}TmpVariableObj;

class CMeanShiftKalman{
public:
	CMeanShiftKalman();
	virtual ~CMeanShiftKalman();

public:

	void Clear_MeanShift_tracker();
	void draw_rectangle( unsigned char * image, int W, int H, TRACK_P inputParam, unsigned int color, int thickness);
	void Kalman(double *measure, double *control);

	void Initial_MeanShift_BG_tracker( int x0, int y0, int Wx, int Hy, unsigned char * image, int W, int H, double DeltaT,int DP, int MP, int CP );
	int Mean_shift_iteration_BG(	int xi, int yi, int Wx, int Hy, unsigned char * image, int W, int H,
															double * Model_BG_Hist, int bins, int & xo, int & yo, double & rho, TmpVariableObj *pTmpObj);

	double MeanShift_BG_tracker( int xin, int yin, int Win, int Hin, unsigned char * image, int W, int H, int & xout, int & yout, int & Wout, int & Hout );
	
	void	UpdateFrame(TRACK_P inputParam, unsigned char * image, IMGFrame *pImgFrame, int Width, int Height);
	void CalSSIM(unsigned char *pIMG0, unsigned char *pIMG1, int width, int height, double *SSIM);
	void KalmanPredict(int xout, int yout);
	int SetStart(bool bStart);
	int CalTgtSTRUCT(unsigned char *pIMG, TRACK_P inputParam, int width, int height, int *SSIM);

protected:
	int CalcuEpanechnikovKernel( int Wx, int Hy, double * Kernel, double &C_k );
	int CalcuEpanechnikovKernel_Size( int Wx, int Hy, double * Kernel, double &C_k );
	void CalcuModelHist( int xt, int yt, int Wx, int Hy,  unsigned char * image, int W, int H, double * ModelHist, int bins, double *pKernel);
	void CalcuColorHistogram(	int x0, int y0, int Wx, int Hy, unsigned char * image, int W, int H, double * Kernel, double C_k, double * ColorHist, int bins );

public:
	int      m_bins;
	double*  Model_Hist;
	CKalman* m_pKalmanProc;
	BOOL     m_bInited;
	double*  m_pMeasure;
	double*  m_pControl;

	double*  m_pKernel;//注意选取目标区域大小，以免越界
	TmpVariableObj *m_pTmpObj;

	double  *Bg_o_Hist;
	double  *Bg_v_Hist;
	int			m_sizeIncrease;
	double   m_minDist;
	double   m_updateTh;
	double *m_curModelHist, *m_BkModelHist;

	int *Color_BG_Hist, *Color_FG_Hist;

	double *m_pBkKernel;
	double m_BkC_K;

	double *m_pBkKernel_Size;
	double m_BkC_K_Size;

	TRACK_P	m_trackParam;
	TRACK_P	m_inputParam;
	TRACK_P	m_outputParam;
	int		offsetX;
	int		offsetY;
	int		trackRank;
	bool	bTrkAcq;
	bool    TrkState;

};

#endif