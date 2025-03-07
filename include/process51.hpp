
#ifndef PROCESS021_HPP_
#define PROCESS021_HPP_

#include "VideoProcess.hpp"
#include "osd_graph.h"
#include "osd_cv.h"
class CProcess : public CVideoProcess
{
	UTC_RECT_float rcTrackBak,resultTrackBak;
	TARGET tgBak[MAX_TARGET_NUMBER];
	TARGETDRAW Mdrawbak[MAX_TARGET_NUMBER];
	BlobRect blob_rectBak;
	Osd_cvPoint crossBak;
	Osd_cvPoint crossWHBak;
	cv::Rect acqRectBak;
	Osd_cvPoint freezecrossBak;
	Osd_cvPoint crosspicpBak;
	Osd_cvPoint crosspicpWHBak;
	Osd_cvPoint rectfovBak[2];
	Osd_cvPoint secBak[2];
	DS_Rect rendpos[4];
	int Osdflag[20];
	int osdindex;
	int Mmtsendtime;
	int prisensorstatus;
	int Fovpri[2];
	DS_Rectmmt Mmtpos[5];
	DS_Rect random;
	char trkFPSDisplay[128];
	Osd_cvPoint debugBak;

	char timedisplay[128];

	
public:
	CProcess();
	~CProcess();

	void OnCreate();
	void OnDestroy();
	void OnInit();
	void OnConfig();
	void OnRun();
	void OnStop();
	void Ontimer();
	bool OnPreProcess(int chId, Mat &frame);
	bool OnProcess(int chId, Mat &frame);
	void OnMouseLeftDwn(int x, int y);
	void OnMouseLeftUp(int x, int y);
	void OnMouseRightDwn(int x, int y);
	void OnMouseRightUp(int x, int y);
	void OnKeyDwn(unsigned char key);
	
	CMD_EXT* extInCtrl;
	
	
	static CProcess *sThis;
	void process_osd_test(void *pPrm);


protected:
	void msgdriv_event(MSG_PROC_ID msgId, void *prm);
	void osd_mtd_show(TARGET tg[], bool bShow = true);
	void DrawBlob(BlobRect blobRct, bool bShow = true);
	void DrawCross(cv::Rect rec,int fcolour ,bool bShow /*= true*/);
	void drawmmt(TARGET tg[], bool bShow = true);
	void drawmmtnew(TARGET tg[], bool bShow = true);
	void erassdrawmmt(TARGET tg[], bool bShow = true);
	void erassdrawmmtnew(TARGETDRAW tg[], bool bShow = true);
	void DrawdashCross(int x,int y,int fcolour , bool bShow = true);
	void DrawdashRect(int startx,int starty,int endx,int endy,int colour);
	void DrawMeanuCross(int x,int y,int fcolour , bool bShow,int centerx,int centery);
	int  PiexltoWindowsx(int x,int channel);
	int  PiexltoWindowsy(int y,int channel);
	int  PiexltoWindowsxzoom(int x,int channel);
	int  PiexltoWindowsyzoom(int y,int channel);
	int  PiexltoWindowsxzoom_TrkRect(int x,int channel);
	int  PiexltoWindowsyzoom_TrkRect(int y,int channel);
	int  WindowstoPiexlx(int x,int channel);
	int  WindowstoPiexly(int y,int channel);
	float PiexltoWindowsxf(float x,int channel);
	float PiexltoWindowsyf(float y,int channel);



	 static int  MSGAPI_initial(void);
	 static void MSGAPI_init_device(long lParam );
	 static void MSGAPI_inputsensor(long lParam );
	 static void MSGAPI_picp(long lParam );
	 static void MSGAPI_inputtrack(long lParam );
	 static void MSGAPI_inpumtd(long lParam );
	 static void MSGAPI_inpuenhance(long lParam );
	 static void MSGAPI_inputbdt(long lParam );
	 static void MSGAPI_inputzoom(long lParam );
	 static void  MSGAPI_setAimRefine(long lParam          /*=NULL*/);
	 static void MSGAPI_setAimSize(long lParam );
	 static void MSGAPI_inputfrezz(long lParam );
	 static void MSGAPI_inputmmtselect(long lParam );
	 static void MSGAPI_croppicp(long lParam );
	 static void MSGAPI_inpumtdSelect(long lParam );
	 static void MSGAPI_inputpositon(long lParam );
	 static void MSGAPI_inputcoast(long lParam );
	 static void MSGAPI_inputfovselect(long lParam );
	 static void MSGAPI_inputfovchange(long lParam );
	 static void MSGAPI_inputsearchmod(long lParam );
	 static void MSGAPI_inputvideotect(long lParam );
	 static void MSGAPI_mmtshow(long lParam );
	 static void MSGAPI_FOVcmd(long lParam );
	 static void MSGAPI_SaveCfgcmd(long lParam );
	 static void MSGAPI_setMtdState(long lParam);

	 static void MSGAPI_update_osd(long IParam);
	 static void MSGAPI_update_alg(long IParam);
	 static void MSGAPI_update_camera(long IParam);

private:
	ACK_EXT extOutAck;
	bool     m_bCast;
	UInt32 m_castTm;
	//Multich_graphic grpxChWinPrms;

	void process_status(void);

	void osd_init(void);
	void process_osd1();
	void DrawLine(Mat frame, int startx, int starty, int endx, int endy, int width, UInt32 colorRGBA);
	void DrawHLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA);
	void DrawVLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA);
	void DrawChar(Mat frame, int startx, int starty, char *pChar, UInt32 frcolor, UInt32 bgcolor);
	void DrawString(Mat frame, int startx, int starty, char *pString, UInt32 frcolor, UInt32 bgcolor);
	void DrawStringcv(Mat& frame, int startx, int starty, char *pChar, UInt32 frcolor, UInt32 bgcolor);
	void osd_draw_cross(Mat frame, void *prm);
	void osd_draw_rect(Mat frame, void *prm);
	void osd_draw_rect_gap(Mat frame, void *prm);
	void osd_draw_text(Mat frame, void *prm);
	void osd_draw_cross_black_white(Mat frame, void *prm);

	int process_draw_line(Mat frame, int startx, int starty, int endx, int linewidth,char R, char G, char B, char A);
	int process_draw_text(Mat frame,int startx,int starty,char *pstring,int frcolor,int bgcolor);
	int process_draw_instance(Mat frame);
	int draw_circle_display(Mat frame);

	void	DrawRect(Mat frame,cv::Rect rec,int frcolor);
	void DrawAcqRect(cv::Mat frame,cv::Rect rec,int frcolor,bool bshow);

	void initAcqRect();
	void initAimRect();

public:
	void update_param_alg();
	void update_param_osd();
	void DrawScaleLine(Mat frame,int frcolor);

};



#endif /* PROCESS021_HPP_ */
