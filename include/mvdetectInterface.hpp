
#ifndef MVDECTINTERFACE_H_
#define MVDECTINTERFACE_H_

#include "infoHead.h"

typedef void ( *LPNOTIFYFUNC)(void *context, int chId);

class	CMvDectInterface
{
public:
	CMvDectInterface(){};
	virtual ~CMvDectInterface(){};

public:
	virtual	int	init(LPNOTIFYFUNC	notifyFunc, void *context){return 1;};
	virtual	int destroy(){return 1;};
	virtual	void	setFrame(cv::Mat	src, 	int chId = 0){};//杈撳叆瑙嗛甯?
	virtual	void	enableSelfDraw(bool	bEnable, int chId = 0){};
	virtual	void   setROIScalXY(float scaleX = 1.0, float scaleY = 1.0, int chId = 0){};//璁剧疆缂╂斁绯绘暟锛岄粯璁や负1.0

	virtual	void	clearWarningRoi(int chId	= 0){};//娓呴櫎璀︽垝鍖?
	virtual	void	setWarningRoi(std::vector<cv::Point2i>	warnRoi,	int chId	= 0){};//璁剧疆缂╂斁鍓嶇殑璀︽垝鍖哄煙
	virtual	void	setTrkThred(TRK_THRED		trkThred,	int chId	= 0){};
	virtual	void	setDrawOSD(cv::Mat	dispOSD, int chId = 0){};
	virtual	void	setWarnMode(WARN_MODE	warnMode,	int chId	= 0){};//璁剧疆璀︽垝妯″紡

	virtual	void	getLostTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//涓㈠け鐩爣
	virtual	void	getInvadeTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//鍏ヤ镜鐩爣
	virtual	void	getMoveTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//绉诲姩鐩爣
	virtual	void	getBoundTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//瓒婄晫鐩爣
	virtual	void	getWarnTarget(std::vector<TRK_RECT_INFO>	&resTarget,	int chId	= 0){};//璀︽垝鍖哄懆杈规墍鏈夌洰鏍?

};

CMvDectInterface *MvDetector_Create();
void MvDetector_Destory(CMvDectInterface *obj);


#endif /* MVDECTINTERFACE_H_ */
