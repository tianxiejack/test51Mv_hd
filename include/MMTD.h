#ifndef		_MMTD_HEAD_
#define		_MMTD_HEAD_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include "multitarget.h"

class CMMTD
{
public:
	CMMTD();
	~CMMTD();
	/*
	 * MAX_TGT_NUM in "mmthead.h"
	 * tgtNum < MAX_TGT_NUM
	 */
	void 	SetTargetNum(int tgtNum);
	/*
	 * default
	 * minArea = 80
	 * maxArea = 3600
	 */
	void 	SetConRegMinMaxArea(int minArea = 80, int maxArea = 3600);
	/*
	 * default
	 * srSize = cv::Size(128, 128)
	 */
	void 	SetSalientSize(cv::Size srSize=cv::Size(128, 128));
	/*
	 * default
	 * gapFrames = 3
	 */
	void 	SetSRDetectGap(int gapFrames = 3);
	/*
	 * default
	 * lumThred = 50
	 */
	void 	SetSRLumThred(int lumThred = 50);
	/*\u8bbe\u7f6e\u9759\u6b62\u76ee\u6807\u548c\u8fd0\u52a8\u76ee\u6807\u7684\u9608\u503c\u50cf\u7d20
	 * default
	 * stillPixel = 6
	 * movePixel = 16
	 */
	void SetMoveThred(int stillPixel = 6, int movePixel = 16);
	/*
	 * default
	 * lapScaler = 1.25
	 *
	 */
	void SetLapScaler(float lapScaler = 1.25);

	void	ClearAllMMTD();
	void	MMTDProcess(cv::Mat image, TARGETBOX *tgBox, cv::Mat osdFrame, int bDisp = 0);
	void	MMTDProcessRect(cv::Mat image, TARGETBOX *tgBox, cv::Rect roi, cv::Mat osdFrame, int bDisp = 0);
	void 	SRDetector(cv::Mat image, cv::Mat osdFrame);
	void 	SRDetectorRect(cv::Mat image, cv::Rect roi, cv::Mat osdFrame);

private:
	int  m_tgtNum;
	int	m_bInterval;

};

#endif
