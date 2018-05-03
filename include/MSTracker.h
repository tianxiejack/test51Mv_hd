#ifndef _MS_TRACKER_
#define _MS_TRACKER_

#include "opencv2/core/core.hpp"
#include "prehead.h"
#include "MeanShiftKalman.h"
#include "multitarget.h"

using namespace cv;
using namespace std;

#define BORADERWIDTH       25
#define MERGEDISNTANCE     25

typedef struct{
	int valid;       //
	int index;       //Ê£ÓàÄ¿±êÐòºÅ
	int invalid;
	int center_x;
	int center_y;
	int target_W;
	int target_H;
	int trackW;
	int trackH;
	int origLost;    //¼ì²âºóµÄ¶ªÊ§ÐòºÅ
	int filled;
}MSTARGET;

typedef struct{
	MSTARGET detectTarget;
	float distance;
	int fill_index;      //´ýÌî³ä¸ú×ÙÐòºÅ
}DETECTAGERT;



class CMSTracker
{
public:
	CMSTracker();
	~CMSTracker();

public:
	void initMS(Mat image, TARGETBOX *tgBox/*vector<Rect> Boxes*/, int BoxNum);

	void MSTrkAcq(CMeanShiftKalman *pTrackObj, IMGFrame *inputFrame,TRACK_P inputParam);

	void MSTrkProc(CMeanShiftKalman *pTrackObj, IMGFrame *capFrame, IMGFrame *disFrame, int *pOffsetX, int *pOffsetY, double *pSSIM);

	void MSTrkStart(CMeanShiftKalman *pTrackObj, bool start);

	void SetFrameValue(Mat image);

	void MSprocess(Mat image, int BoxNum);

	void MergeMStarget(Mat frame, int tgNums, TARGETBOX *tgBox);

	int DeletMSTarget(MSTARGET *MStg, int leftIndex, int deletIndex);

	void FillMStarget(TARGETBOX *tgBox/*vector<Rect> &Box*//*, bool isTrackIn*//*, bool isNextDetc*/);

	void ClearDTtg();

	void Process(Mat frame, int width, int height, int frameNum, TARGETBOX *tgBox/*vector<Rect> &Box*/,int chId);
	//void ClearBox(TARGETBOX *tgBox);
	void ClearAllMStg(TARGETBOX *tgBox, unsigned char changeChId);

public:
	CMeanShiftKalman	m_meanShift[MAX_TARGET_NUMBER];
	IMGFrame	inFrame;
	TRACK_P trkWnd[MAX_TARGET_NUMBER];
	MSTARGET MStg[MAX_TARGET_NUMBER];
	DETECTAGERT DTtg[MAX_TARGET_NUMBER];
	int validNum;       //ÓÐÐ§ÐòºÅ¸öÊý
	CMultitarget detect;

	int detState;// = FALSE;
	//int keyEvents = 0;
	//int trackEnd = FALSE;
	int trackStart;// = FALSE;
	bool isNextDetect;// = FALSE;
	bool isInTrack;// = TRUE;
	bool tgEnd;// = FALSE;       //±£´æ¼ì²â×îºóÒ»Ö¡box×ø±ê

};

#endif
