

#include <glut.h>
#include "VideoProcess.hpp"
#include "vmath.h"
#include "arm_neon.h"

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "configable.h"

#include "MvDetect.hpp"
#include "app_ctrl.h"

using namespace vmath;

int CVideoProcess::m_mouseEvent = 0;
int CVideoProcess::m_mousex = 0;
int CVideoProcess::m_mousey = 0;
CVideoProcess * CVideoProcess::pThis = NULL;
bool CVideoProcess::m_bTrack = false;
bool CVideoProcess::m_bMtd = false;
bool CVideoProcess::m_bBlobDetect = false;
bool CVideoProcess::m_bMoveDetect = false;
int CVideoProcess::m_iTrackStat = 0;
int CVideoProcess::m_iTrackLostCnt = 0;
int64 CVideoProcess::tstart = 0;
static int count=0;
int ScalerLarge,ScalerMid,ScalerSmall;


#if __MV__DETECT_VIBE__

#include <cstddef>
#include <ctime>
#include <iostream>
#include "ViBe.h"
#include "distances/Manhattan.h"
#include "system/types.h"
using namespace ViBe;
ViBeSequential<1, Manhattan<1> > * vibe;

#endif


int CVideoProcess::MAIN_threadCreate(void)
{
	int iRet = OSA_SOK;
	iRet = OSA_semCreate(&mainProcThrObj.procNotifySem ,1,0) ;
	OSA_assert(iRet == OSA_SOK);


	mainProcThrObj.exitProcThread = false;

	mainProcThrObj.initFlag = true;

	mainProcThrObj.pParent = (void*)this;

	iRet = OSA_thrCreate(&mainProcThrObj.thrHandleProc, mainProcTsk, 0, 0, &mainProcThrObj);

	return iRet;
}


static void copyMat2Mat(cv::Mat src, cv::Mat dst, cv::Point pt)
{
	int i, j;
	uchar *psrc, *pdst;
#pragma UNROLL(4)
	for(i=0; i<src.rows; i++)
	{
		psrc = (uchar*)src.ptr<cv::Vec4b>(i,0);
		pdst =  (uchar*)dst.ptr<cv::Vec4b>(pt.y+i, pt.x);
		memcpy(pdst, psrc, src.cols*sizeof(cv::Vec4b));
	}
}

TARGETBOX mBox[MAX_TARGET_NUMBER];
//static int trktime = 0;
void CVideoProcess::main_proc_func()
{
	OSA_printf("%s: Main Proc Tsk Is Entering...\n",__func__);
	unsigned int framecount=0;
	
	float speedx,speedy;
	float optValue;
	UTC_Rect AcqRect;

	static bool Movedetect=false;
	Point pt1,pt2,erspt1,erspt2,erspt3,erspt4;
	static UTC_ACQ_param acqRect;
	CMD_EXT tmpCmd={0};
	double value;

#if __MV__DETECT_VIBE__
	static bool firstFrame = true;
	
#endif

#if 1
	
	static int timeFlag = 2;
	static int speedcount = 0;

	time_t timep;  
	struct tm *p;  	
	char file[128];
	const int MvDetectAcqRectWidth  =  80;
	const int MvDetectAcqRectHeight =  80;
	
#endif
	while(mainProcThrObj.exitProcThread ==  false)
	{
		/******************************************/
		if(m_pwFile==NULL && wFileFlag){
			time(&timep);  
			p = gmtime(&timep);
			sprintf(file,"MVspeed_%d-%d-%d.txt",p->tm_hour, p->tm_min, p->tm_sec);		
			m_pwFile = fopen(file,"a+");
			if(m_pwFile==NULL)
				printf("Open File err.\n");
		}
		
		OSA_semWait(&mainProcThrObj.procNotifySem, OSA_TIMEOUT_FOREVER);

		//OSA_printf("%s:pp = %d ,\n",__func__, mainProcThrObj.pp);
		Mat frame = mainFrame[mainProcThrObj.pp^1];
		bool bTrack = mainProcThrObj.cxt[mainProcThrObj.pp^1].bTrack;
		bool bMtd = mainProcThrObj.cxt[mainProcThrObj.pp^1].bMtd;
		bool bBlobDetect = mainProcThrObj.cxt[mainProcThrObj.pp^1].bBlobDetect;
		bool bMoveDetect = mainProcThrObj.cxt[mainProcThrObj.pp^1].bMoveDetect;
		int chId = mainProcThrObj.cxt[mainProcThrObj.pp^1].chId;
		int iTrackStat = mainProcThrObj.cxt[mainProcThrObj.pp^1].iTrackStat;

		int channel = frame.channels();
		Mat frame_gray;

		cv::Mat	salientMap, sobelMap;

		mainProcThrObj.pp ^=1;
		if(!OnPreProcess(chId, frame))
			continue;

		if(!m_bTrack && !m_bMtd && !m_bBlobDetect&&!m_bMoveDetect){
			OnProcess(chId, frame);
			continue;
		}

		if(chId != m_curChId)
			continue;

		frame_gray = Mat(frame.rows, frame.cols, CV_8UC1);

		if(channel == 2)
		{
			extractYUYV2Gray2(frame, frame_gray);
		}
		else
		{
			memcpy(frame_gray.data, frame.data, frame.cols * frame.rows*channel*sizeof(unsigned char));
		}
#if 1
		if(wFileFlag == true && m_pwFile != NULL){
			if(bTrack)
			{
				timeFlag--;
				if(timeFlag == 0)
					timeFlag = 0;
			}
			else
				timeFlag = 2;
		}
#endif		
		if(bTrack)
		{
		#if __TRACK__
			iTrackStat = ReAcqTarget();
			if(Movedetect&&(iTrackStat==0))
			{
				#if 1
					IMG_MAT image;
					image.data_u8 = frame_gray.data;
					image.width = frame_gray.cols;
					image.height = frame_gray.rows;
					image.channels = 1;
					image.step[0] = image.width;
					image.dtype = 0;
					image.size = frame_gray.cols*frame_gray.rows;
					Movedetect = UtcAcqTarget(m_track,image,acqRect,&TRKMoveAcpSR);
					if(Movedetect)
						MoveAcpSR=TRKMoveAcpSR;
				#endif
				moveStat=0;
				Movedetect=FALSE;

				if(moveDetectRect)
					rectangle( m_display.m_imgOsd[1],
						Point( preAcpSR.x, preAcpSR.y ),
						Point( preAcpSR.x+preAcpSR.width, preAcpSR.y+preAcpSR.height),
						cvScalar(0,0,0,0), 1, 8 );
				
				unsigned int currentx=MoveAcpSR.x+MoveAcpSR.width/2;
				unsigned int currenty=MoveAcpSR.y+MoveAcpSR.height/2;
					
				if((m_rcTrack.x+m_rcTrack.width>MoveAcpSR.x&&
					m_rcTrack.x<MoveAcpSR.x+MoveAcpSR.width)||
					(m_rcTrack.y+m_rcTrack.height>MoveAcpSR.y&&
					m_rcTrack.y<MoveAcpSR.y+MoveAcpSR.height))
					{
						MoveAcpSR.width=MoveAcpSR.width*1.4;
						MoveAcpSR.height=MoveAcpSR.height*1.4;
						
						if(MoveAcpSR.width<m_rcTrack.width/2)
						{
							MoveAcpSR.width=m_rcTrack.width/2;
						}
						else if(MoveAcpSR.width>m_rcTrack.width)
						{
							MoveAcpSR.width=m_rcTrack.width;
						}
						if(MoveAcpSR.width<16)
						{
							MoveAcpSR.width=16;
						}
						if(MoveAcpSR.height<m_rcTrack.height/2)
						{
							MoveAcpSR.height=m_rcTrack.height/2;
						}
						else if(MoveAcpSR.height>m_rcTrack.height)
						{
							MoveAcpSR.height=m_rcTrack.height;
						}
						if(MoveAcpSR.height<16)
						{
							MoveAcpSR.height=16;
						}
						if(MoveAcpSR.width*1.0/MoveAcpSR.height>1.333)
						{
							MoveAcpSR.height=MoveAcpSR.width/1.333;
						}
						else if(MoveAcpSR.height*1.0/MoveAcpSR.width>1.333)
						{
							MoveAcpSR.width=MoveAcpSR.height/1.333;
						}
						
						m_rcTrack.x=currentx-MoveAcpSR.width/2;
						m_rcTrack.y=currenty-MoveAcpSR.height/2;
						m_rcTrack.width =MoveAcpSR.width;
						m_rcTrack.height=MoveAcpSR.height;
					}
					else
						moveStat=1;		

			}
			int64 trktime = 0;
			if(algOsdRect == true)
				trktime = getTickCount();//OSA_getCurTimeInMsec();
			if(m_iTrackStat==2)
			{
				//m_searchmod=1;
			}
			else
			{
				m_searchmod=0;
			}
			m_iTrackStat = process_track(iTrackStat, frame_gray, m_dc, m_rcTrack);

			UtcGetSceneMV(m_track, &speedx, &speedy);
			UtcGetOptValue(m_track, &optValue);
			//OSA_printf("*************speedxy=(%f,%f)\n",speedx,speedy);
			#if 1
			if(wFileFlag == true && m_pwFile != NULL){
				if(timeFlag == 1){
					speedcount +=1;
					fprintf(m_pwFile,"###############################%d\n",speedcount);
				}
				fprintf(m_pwFile,"%0.3f,%0.3f,%d,%0.3f\n",speedx,speedy,m_iTrackStat,optValue*10);
			}
			#endif
			if(m_iTrackStat == 2)
				m_iTrackLostCnt++;
			else
				m_iTrackLostCnt = 0;


			if(m_iTrackStat == 2)
			{								
				if(m_iTrackLostCnt > 3)
				{					
					m_rcTrack.x = m_ImageAxisx - m_rcTrack.width/2;
					m_rcTrack.y = m_ImageAxisy - m_rcTrack.height/2;
					//OSA_printf(">3 -> TrkStat = %d  ImageAxisxy = (%f,%f)\n",m_iTrackStat,m_rcTrack.x,m_rcTrack.y);
				}
				else
				{
					m_iTrackStat = 1;
					m_rcTrack.x = m_ImageAxisx - m_rcTrack.width/2;
					m_rcTrack.y = m_ImageAxisy - m_rcTrack.height/2;
					//OSA_printf("<3 ->ImageAxisxy = (%d,%d)\n",m_ImageAxisx,m_ImageAxisy);
				}
			}

			if(m_display.disptimeEnable == 1)
			{
				putText(m_display.m_imgOsd[1],m_strDisplay,
						Point( m_display.m_imgOsd[1].cols-450, 30),
						FONT_HERSHEY_TRIPLEX,0.8,
						cvScalar(0,0,0,0), 1
						);
				sprintf(m_strDisplay, "TrkStat=%d speedxy: (%0.2f,%0.2f)", m_iTrackStat, speedx,speedy);

				putText(m_display.m_imgOsd[1],m_strDisplay,
						Point( m_display.m_imgOsd[1].cols-450, 30),
						FONT_HERSHEY_TRIPLEX,0.8,
						cvScalar(255,255,0,255), 1
						);
			}
			
			//printf("********m_iTrackStat=%d\n",m_iTrackStat);
			//OSA_printf("ALL-Trk: time = %d ms \n", OSA_getCurTimeInMsec() - trktime);
			if(algOsdRect == true){
				float time = ( (getTickCount() - trktime)/getTickFrequency())*1000;;//OSA_getCurTimeInMsec() - trktime;
				static float totaltime = 0;
				static int count11 = 1;
				totaltime += time;
				if((count11++)%100 == 0)
				{
					OSA_printf("ALL-TRK: time = %f ms \n", totaltime/100 );
					count11 = 1;
					totaltime = 0.0;
				}
			}
		#endif
		}
		else if(bMtd)
		{
			tstart = getTickCount();

			Rect roi;

			if(tvzoomStat)
				{
					roi.x=frame_gray.cols/4;
					roi.y=frame_gray.rows/4;
					roi.width=frame_gray.cols/2;
					roi.height=frame_gray.rows/2;
					//OSA_printf("roiXY(%d,%d),WH(%d,%d)\n",roi.x,roi.y,roi.width,roi.height);
				}
			else
				{
					roi.x=0;
					roi.y=0;
					roi.width=frame_gray.cols;
					roi.height=frame_gray.rows;
				}

			//m_MMTDObj.MMTDProcess(frame_gray, m_tgtBox, m_display.m_imgOsd[1], 0);
			m_MMTDObj.MMTDProcessRect(frame_gray, m_tgtBox, roi, m_display.m_imgOsd[1], 0);
			for(int i=0;i<MAX_TARGET_NUMBER;i++)
			{
				m_mtd[chId]->tg[i].cur_x=m_tgtBox[i].Box.x+m_tgtBox[i].Box.width/2;
				m_mtd[chId]->tg[i].cur_y=m_tgtBox[i].Box.y+m_tgtBox[i].Box.height/2;
				m_mtd[chId]->tg[i].valid=m_tgtBox[i].valid;
				//OSA_printf("ALL-MTD: time  ID %d  valid=%d x=%d y=%d\n",i,m_tgtBox[i].valid,m_tgtBox[i].Box.x,m_tgtBox[i].Box.y);
			}
			
		}
		else if (bBlobDetect)
		{
			Mat dysrc = frame_gray(Rect(frame_gray.cols/2-75, frame_gray.rows/2-75, 150, 150));					
			BlobDetect(dysrc, adaptiveThred, m_blobRect);
			//OSA_printf("ALL-BlobDetect: time = %f sec \n", ( (getTickCount() - tstart)/getTickFrequency()) );
		}
		else if (bMoveDetect)
		{
			#if 0

				IMG_MAT image;
				image.data_u8 = frame_gray.data;
				image.width = frame_gray.cols;
				image.height = frame_gray.rows;
				image.channels = 1;
				image.step[0] = image.width;
				image.dtype = 0;
				image.size = frame_gray.cols*frame_gray.rows;

				if(moveDetectRect)
				{
					rectangle( m_display.m_imgOsd[1],
						Point( preAcpSR.x, preAcpSR.y ),
						Point( preAcpSR.x+preAcpSR.width, preAcpSR.y+preAcpSR.height),
						cvScalar(0,0,0,0), 1, 8 );

					rectangle( m_display.m_imgOsd[1],
							Point( preWarnRect.x, preWarnRect.y ),
							Point( preWarnRect.x+preWarnRect.width, preWarnRect.y+preWarnRect.height),
							cvScalar(0,0,0,0), 1, 8 );
				}
				acqRect.axisX = m_ImageAxisx;
				acqRect.axisY = m_ImageAxisy;

				if(m_SensorStat == 0){
					acqRect.rcWin.width = 1800;
					acqRect.rcWin.height = 900;
					acqRect.rcWin.x = VIDEO_IMAGE_WIDTH_0/2 - acqRect.rcWin.width/2;
					acqRect.rcWin.y = VIDEO_IMAGE_HEIGHT_0/2 -acqRect.rcWin.height/2;
				}
				//OSA_printf("acq axis  x ,y :(%d,%d)\n",acqRect.axisX,acqRect.axisY);
//OSA_printf("x,y,width,height : (%d,%d,%d,%d)\n",acqRect.rcWin.x,acqRect.rcWin.y,acqRect.rcWin.width,acqRect.rcWin.height);
				memcpy(&preWarnRect,&acqRect.rcWin,sizeof(UTC_Rect));
				
				if(1)//(moveDetectRect)
					rectangle( m_display.m_imgOsd[1],
							Point( preWarnRect.x, preWarnRect.y ),
							Point( preWarnRect.x+preWarnRect.width, preWarnRect.y+preWarnRect.height),
							cvScalar(0,0,255,255), 2, 8 );

				#if 0
					Movedetect = UtcAcqTarget(m_track,image,acqRect,&MoveAcpSR);
				#else
					Movedetect = UtcTrkPreAcqSR(m_track,image,acqRect,&MoveAcpSR);
				#endif
				if(Movedetect)
				{
					getImgRioDelta(image.data_u8,image.width ,image.height,MoveAcpSR,&value);
					OSA_printf("%s:line  %d   double = %f \n",__func__,__LINE__,value);	
					if(value < 300.0)
						Movedetect = 0;
				}
				
				if(Movedetect)
				{
					OSA_printf("%s:line  %d   double = %f \n",__func__,__LINE__,value);	
					printf("%s,line:%d		xy(%d,%d),wh(%d,%d)\n",__func__,__LINE__,
						preAcpSR.x,preAcpSR.y,preAcpSR.width,preAcpSR.height);	
					preAcpSR.x = MoveAcpSR.x*m_display.m_imgOsd[1].cols/frame.cols;
					preAcpSR.y = MoveAcpSR.y*m_display.m_imgOsd[1].rows/frame.rows;
					preAcpSR.width = MoveAcpSR.width*m_display.m_imgOsd[1].cols/frame.cols;
					preAcpSR.height = MoveAcpSR.height*m_display.m_imgOsd[1].rows/frame.rows;

					if(0)//(moveDetectRect)
						rectangle( m_display.m_imgOsd[1],
							Point( preAcpSR.x, preAcpSR.y ),
							Point( preAcpSR.x+preAcpSR.width, preAcpSR.y+preAcpSR.height),
							cvScalar(255,0,0,255), 1, 8 );
	
				
					tmpCmd.AvtTrkStat = eTrk_mode_sectrk;
					tmpCmd.AvtPosX[0] = preAcpSR.x + preAcpSR.width/2;
					tmpCmd.AvtPosY[0] = preAcpSR.y + preAcpSR.height/2;
					app_ctrl_setTrkStat(&tmpCmd);		
					tmpCmd.MtdState[0] = 0;
					app_ctrl_setMtdStat(&tmpCmd);	
					rectangle( m_display.m_imgOsd[1],
							Point( preWarnRect.x, preWarnRect.y ),
							Point( preWarnRect.x+preWarnRect.width, preWarnRect.y+preWarnRect.height),
							cvScalar(0,0,0,0), 2, 8 );
										
				}
				
				#if 0
				if(m_display.disptimeEnable == 1){
				
					UtcGetSceneMV(m_track, &speedx1, &speedy1);
					
					putText(m_display.m_imgOsd[1],m_strDisplay1,
							Point( m_display.m_imgOsd[1].cols-450, 105),
							FONT_HERSHEY_TRIPLEX,0.8,
							cvScalar(0,0,0,0), 1
							);
					sprintf(m_strDisplay1, "speedxy: (%0.2f,%0.2f)", speedx1,speedy1);

					putText(m_display.m_imgOsd[1],m_strDisplay1,
							Point( m_display.m_imgOsd[1].cols-450, 105),
							FONT_HERSHEY_TRIPLEX,0.8,
							cvScalar(255,255,0,255), 1
							);
			
				}
				#endif
#endif

		#if __MOVE_DETECT__
			#if __DETECT_SWITCH_Z__
				float x = 1920.0/960.0;
				float y = 1080.0/540.0;
				m_pMovDetector->setROIScalXY(x,y,0);
				cv::Mat gray;
				cv::resize(frame_gray,gray, cv::Size(960, 540));
				
				if(m_pMovDetector != NULL)
					m_pMovDetector->setFrame(gray,0);	//chId
			#else
				#if __MV__DETECT_VIBE__
				cv::resize(frame_gray,frame_gray, cv::Size(640, 512));
				if (firstFrame) {
					  /* Instantiation of ViBe. */
					  vibe = new ViBeSequential<1, Manhattan<1> > (frame_gray.rows, frame_gray.cols, frame_gray.data);
					  firstFrame = false;
					}
				
				cv::Mat segmentationMap(frame_gray.size(), CV_8UC1);
				unsigned ttnode = OSA_getCurTimeInMsec();
				vibe->segmentation(frame_gray.data, segmentationMap.data);
				printf("delta1  = %d \n",OSA_getCurTimeInMsec() - ttnode);
	    		vibe->update(frame_gray.data, segmentationMap.data);
				printf("delta2  = %d \n",OSA_getCurTimeInMsec() - ttnode);
				
				imshow("Input video", frame_gray);
				imshow("Segmentation by ViBe", segmentationMap);
				waitKey(1);
				#endif
			#endif
		#endif
		}

		if(chId != m_curChId)
			continue;
		
		OnProcess(chId, frame);
		framecount++;

	/************************* while ********************************/
	}
	OSA_printf("%s: Main Proc Tsk Is Exit...\n",__func__);
}

int CVideoProcess::MAIN_threadDestroy(void)
{
	int iRet = OSA_SOK;

	mainProcThrObj.exitProcThread = true;
	OSA_semSignal(&mainProcThrObj.procNotifySem);

	iRet = OSA_thrDelete(&mainProcThrObj.thrHandleProc);

	mainProcThrObj.initFlag = false;
	OSA_semDelete(&mainProcThrObj.procNotifySem);

	return iRet;
}


CVideoProcess::CVideoProcess()
	:m_track(NULL),m_curChId(0),m_curSubChId(-1),adaptiveThred(40)		
{
	pThis = this;
	m_pwFile = NULL;

	memset(m_mtd, 0, sizeof(m_mtd));
	memset(&mainProcThrObj, 0, sizeof(MAIN_ProcThrObj));
	
	detState = TRUE;
	trackEnd = FALSE;
	trackStart = TRUE;
	nextDetect = FALSE;
	lastFrameBox=0;
	moveStat = FALSE;
	m_acqRectW			= 	60;
	m_acqRectH			= 	60;
	m_ImageAxisx		=	VIDEO_IMAGE_WIDTH_0/2; //trkrefine after lost
	m_ImageAxisy		=	VIDEO_IMAGE_HEIGHT_0/2;//trkrefine after lost
	m_intervalFrame 			= 0;
	m_intervalFrame_change 	= 0;
	m_bakChId = m_curChId;
	trackchange		=0;
	m_searchmod		=0;
	tvzoomStat		=0;
	wFileFlag			=0;
	preAcpSR	={0};
	
	#if __MOVE_DETECT__
		#if __DETECT_SWITCH_Z__
			m_pMovDetector	=NULL;
		#else
			
		 vibe = NULL;
		
		#endif
	#endif
	
	memset(m_tgtBox, 0, sizeof(TARGETBOX)*MAX_TARGET_NUMBER);
}

CVideoProcess::~CVideoProcess()
{
	pThis = NULL;
	if(m_pwFile != NULL){
		fclose(m_pwFile);
		m_pwFile = NULL;
	}
}

int CVideoProcess::creat()
{
	int i = 0;
	#if __TRACK__
	trackinfo_obj=(Track_InfoObj *)malloc(sizeof(Track_InfoObj));
	#endif
	m_display.create();

	MultiCh.m_user = this;
	MultiCh.m_usrFunc = callback_process;
	MultiCh.creat();

	MAIN_threadCreate();
	
	OSA_mutexCreate(&m_mutex);	

	OnCreate();

	
#if __MOVE_DETECT__
	#if __DETECT_SWITCH_Z__
		if(m_pMovDetector == NULL)
			m_pMovDetector = MvDetector_Create();
		OSA_assert(m_pMovDetector != NULL);
	#else

	
	#endif
#endif
	return 0;
}

int CVideoProcess::destroy()
{
	stop();
	OSA_mutexDelete(&m_mutex);
	MAIN_threadDestroy();

	MultiCh.destroy();
	m_display.destroy();

	OnDestroy();

	if(m_pwFile != NULL){
		fclose(m_pwFile);
		m_pwFile = NULL;
	}


#if __MOVE_DETECT__
	#if __DETECT_SWITCH_Z__
		DeInitMvDetect();
	#else
		
	#endif
#endif

	return 0;
}

void CVideoProcess::mouse_event(int button, int state, int x, int y)
{
	m_mousex = x;
	m_mousey = y;
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
			pThis->OnMouseLeftDwn(x, y);
		else
			pThis->OnMouseLeftUp(x, y);
	}

	if(button == GLUT_RIGHT_BUTTON)
	{
		if(state == GLUT_DOWN)
			pThis->OnMouseRightDwn(x, y);
		else
			pThis->OnMouseRightUp(x, y);
	}
}

void CVideoProcess::keyboard_event(unsigned char key, int x, int y)
{
	pThis->OnKeyDwn(key);

	if(key == 27){
		pThis->destroy();
		exit(0);
	}
}

void CVideoProcess::keySpecial_event( int key, int x, int y)
{
	pThis->OnKeyDwn((unsigned char)key);
}

void CVideoProcess::visibility_event(int state)
{
	OSA_printf("visibility event: %d\n", state);
}

void CVideoProcess::close_event(void)
{
	OSA_printf("close event\n");
	pThis->destroy();
}

int CVideoProcess::init()
{
	DS_InitPrm dsInit;

	memset(&dsInit, 0, sizeof(DS_InitPrm));
	dsInit.mousefunc = mouse_event;
#if (!__IPC__)
	dsInit.keyboardfunc = keyboard_event;
#endif
	//dsInit.keySpecialfunc = keySpecial_event;
	dsInit.timerfunc = call_run;
	//dsInit.idlefunc = call_run;
	dsInit.visibilityfunc = visibility_event;
	dsInit.timerfunc_value = 0;
	dsInit.timerInterval = 16;//ms
	dsInit.closefunc = close_event;
	dsInit.bFullScreen = true;
	dsInit.winPosX = 200;
	dsInit.winPosY = 100;
	dsInit.winWidth = vdisWH[0][0];
	dsInit.winHeight = vdisWH[0][1];

	m_display.init(&dsInit);

	m_display.m_bOsd = true;
	m_dc = m_display.m_imgOsd[0];
	m_dccv=m_display.m_imgOsd[1];
	OnInit();
	prichnalid=1;//fir

	moveDetectRect = false;
#if __TRACK__
	trackinfo_obj->trackfov=TVBIGFOV;
#endif

#if __MOVE_DETECT__
	#if __DETECT_SWITCH_Z__
		initMvDetect();
	#endif
#endif
	
	return 0;
}


int CVideoProcess::dynamic_config(int type, int iPrm, void* pPrm)
{
	int iret = 0;
	unsigned int render=0;
	OSA_mutexLock(&m_mutex);

	if(type < CDisplayer::DS_CFG_Max){
		iret = m_display.dynamic_config((CDisplayer::DS_CFG)type, iPrm, pPrm);
	}

	switch(type)
	{
	case VP_CFG_MainChId:
		m_curChId = iPrm;
		m_iTrackStat = 0;
		mainProcThrObj.bFirst = true;
		m_display.dynamic_config(CDisplayer::DS_CFG_ChId, 0, &m_curChId);
		break;
	case VP_CFG_SubChId:
		m_curSubChId = iPrm;
		m_display.dynamic_config(CDisplayer::DS_CFG_ChId, 1, &m_curSubChId);
		break;
	case VP_CFG_TrkEnable:
		m_bTrack = iPrm;
		m_bakChId = m_curChId;
		m_iTrackStat = 0;
		m_iTrackLostCnt = 0;
		mainProcThrObj.bFirst = true;
		if(pPrm == NULL)
		{			
			UTC_RECT_float rc;
			rc.width 	=  60;//m_acqRectW;
			rc.height 	=  60;//m_acqRectH;
			rc.x 		=  m_ImageAxisx - rc.width/2;
			rc.y 		=  m_ImageAxisy - rc.height/2;
			m_rcTrack  = rc;
			m_rcAcq 	  = rc;
		}
		else
		{
			m_rcTrack = *(UTC_RECT_float*)pPrm;
		}
		break;
	case VP_CFG_MmtEnable:
		m_bMtd = iPrm;
		break;
	case VP_CFG_BlobEnable:
		m_bBlobDetect = iPrm;
		break;
	case VP_CFG_SubPicpChId:
		m_curSubChId = iPrm;
		if(pPrm!=NULL)
		render=*(int *)pPrm;
		m_display.dynamic_config(CDisplayer::DS_CFG_ChId, render, &m_curSubChId);
		break;
	case VP_CFG_MvDetect:
		m_bMoveDetect = iPrm;
		break;
	default:
		break;
	}

	if(iret == 0)
		OnConfig();

	OSA_mutexUnlock(&m_mutex);

	return iret;
}

#if 1
int CVideoProcess::configEnhFromFile()
{
	string CalibFile;
	CalibFile = "config.yml";

	char calib_x[11] = "config_x";


	FILE *fp = fopen(CalibFile.c_str(), "rt");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);
		
		if(len < 10)
			return -1;
		else
		{
			
			FileStorage fr(CalibFile, FileStorage::READ);
			if(fr.isOpened())
			{
				
					sprintf(calib_x, "enhmod_%d", 0);
					Enhmod= (int)fr[calib_x];

					sprintf(calib_x, "enhparm_%d", 1);
					Enhparm= (float)fr[calib_x];

					sprintf(calib_x, "mmtdparm_%d", 2);
					DetectGapparm= (int)fr[calib_x];

					sprintf(calib_x, "mmtdparm_%d", 3);
					MinArea= (int)fr[calib_x];

					sprintf(calib_x, "mmtdparm_%d", 4);
					MaxArea= (int)fr[calib_x];

					sprintf(calib_x, "mmtdparm_%d", 5);
					stillPixel= (int)fr[calib_x];

					sprintf(calib_x, "mmtdparm_%d", 6);
					movePixel= (int)fr[calib_x];

					sprintf(calib_x, "mmtdparm_%d", 7);
					lapScaler= (float)fr[calib_x];
				

					sprintf(calib_x, "mmtdparm_%d", 8);
					lumThred= (int)fr[calib_x];
					
					sprintf(calib_x, "timedisp_%d", 9);
					m_display.disptimeEnable= (int)fr[calib_x];
				return 0;
			}else
				return -1;
		}
	}else
		return -1;
}
#endif


int CVideoProcess::configAvtFromFile()
{
	string cfgAvtFile;
	cfgAvtFile = "avt.yml";

    char cfg_avt[16] = "cfg_avt_";
    int configId_Max=128;
    float cfg_blk_val[128];

	FILE *fp = fopen(cfgAvtFile.c_str(), "rt");
	if(fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		int len = ftell(fp);
		fclose(fp);

		if(len < 10)
			return -1;
		else
		{
			FileStorage fr(cfgAvtFile, FileStorage::READ);
			if(fr.isOpened())
			{
				for(int i=0; i<configId_Max; i++){
					sprintf(cfg_avt, "cfg_avt_%d", i);
					cfg_blk_val[i] = (float)fr[cfg_avt];
					//printf(" update cfg [%d] %f \n", i, cfg_blk_val[i]);
				}
			}else
				return -1;
		}
#if __TRACK__
		UTC_DYN_PARAM dynamicParam;
		if(cfg_blk_val[0] > 0)
			dynamicParam.occlusion_thred = cfg_blk_val[0];
		else
			dynamicParam.occlusion_thred = 0.28;
		if(cfg_blk_val[1] > 0)
			dynamicParam.retry_acq_thred = cfg_blk_val[1];
		else
			dynamicParam.retry_acq_thred = 0.38;
		UtcSetDynParam(m_track, dynamicParam);
		float up_factor;
		if(cfg_blk_val[2] > 0)
			up_factor = cfg_blk_val[2];
		else
			up_factor = 0.0125;
		UtcSetUpFactor(m_track, up_factor);
		TRK_SECH_RESTRAINT resTraint;
		if(cfg_blk_val[3] > 0)
			resTraint.res_distance = (int)(cfg_blk_val[3]);
		else
			resTraint.res_distance = 80;
		if(cfg_blk_val[4] > 0)
			resTraint.res_area = (int)(cfg_blk_val[4]);
		else
			resTraint.res_area = 5000;
		//printf("UtcSetRestraint: distance=%d area=%d \n", resTraint.res_distance, resTraint.res_area);
		UtcSetRestraint(m_track, resTraint);

		int gapframe;
		if(cfg_blk_val[5] > 0)
			gapframe = (int)cfg_blk_val[5];
		else
			gapframe = 10;
		UtcSetIntervalFrame(m_track, gapframe);

        bool enhEnable;
		if(cfg_blk_val[6] > -1)
			enhEnable = (bool)cfg_blk_val[6];
		else
			enhEnable = 1;
		UtcSetEnhance(m_track, enhEnable);

		float cliplimit;
		if(cfg_blk_val[7] > 0)
			cliplimit = cfg_blk_val[7];
		else
			cliplimit = 4.0;
		UtcSetEnhfClip(m_track, cliplimit);	

		bool dictEnable;

		dictEnable = (bool)cfg_blk_val[8];

		UtcSetPredict(m_track, dictEnable);

		
		int moveX,moveY;

		moveX = (int)cfg_blk_val[9];
		moveY = (int)cfg_blk_val[10];

		UtcSetMvPixel(m_track,moveX,moveY);

		int moveX2,moveY2;
		
		moveX2 = (int)cfg_blk_val[11];
		moveY2 = (int)cfg_blk_val[12];		

		UtcSetMvPixel2(m_track,moveX2,moveY2);


		int segPixelX,segPixelY;

		segPixelX = (int)cfg_blk_val[13];
		segPixelY = (int)cfg_blk_val[14];		
		UtcSetSegPixelThred(m_track,segPixelX,segPixelY);

		if(cfg_blk_val[15] == 1)
			algOsdRect = true;
		else
			algOsdRect = false;


		ScalerLarge = (int)cfg_blk_val[16];
		ScalerMid = (int)cfg_blk_val[17];		
		ScalerSmall = (int)cfg_blk_val[18];
		UtcSetSalientScaler(m_track, ScalerLarge, ScalerMid, ScalerSmall);

		int Scatter ;
		Scatter = (int)cfg_blk_val[19];
		UtcSetSalientScatter(m_track, Scatter);

		float ratio;
		ratio = cfg_blk_val[20];
		UtcSetSRAcqRatio(m_track, ratio);

		bool FilterEnable;
		FilterEnable = (bool)cfg_blk_val[21];
		UtcSetBlurFilter(m_track,FilterEnable);

		bool BigSecEnable;
		BigSecEnable = (bool)cfg_blk_val[22];
		UtcSetBigSearch(m_track, BigSecEnable);

		int SalientThred;
		SalientThred = (int)cfg_blk_val[23];
		UtcSetSalientThred(m_track,SalientThred);

		int ScalerEnable;
		ScalerEnable = (int)cfg_blk_val[24];
		UtcSetMultScaler(m_track, ScalerEnable);

		bool DynamicRatioEnable;
		DynamicRatioEnable = (bool)cfg_blk_val[25];
		UtcSetDynamicRatio(m_track, DynamicRatioEnable);


		UTC_SIZE acqSize;
		acqSize.width = (int)cfg_blk_val[26];
		acqSize.height = (int)cfg_blk_val[27];		
		UtcSetSRMinAcqSize(m_track,acqSize);

		if(cfg_blk_val[28] == 1)
			TrkAim43 = true;
		else
			TrkAim43 = false;

		bool SceneMVEnable;
		SceneMVEnable = (bool)cfg_blk_val[29];
		UtcSetSceneMV(m_track, SceneMVEnable);

		bool BackTrackEnable;
		BackTrackEnable = (bool)cfg_blk_val[30];
		UtcSetBackTrack(m_track, BackTrackEnable);

		bool  bAveTrkPos;
		bAveTrkPos = (bool)cfg_blk_val[31];
		UtcSetAveTrkPos(m_track, bAveTrkPos);


		float fTau;
		fTau = cfg_blk_val[32];
		UtcSetDetectftau(m_track, fTau);


		int  buildFrms;
		buildFrms = (int)cfg_blk_val[33];
		UtcSetDetectBuildFrms(m_track, buildFrms);
		
		int  LostFrmThred;
		LostFrmThred = (int)cfg_blk_val[34];
		UtcSetLostFrmThred(m_track, LostFrmThred);

		float  histMvThred;
		histMvThred = cfg_blk_val[35];
		UtcSetHistMVThred(m_track, histMvThred);

		int  detectFrms;
		detectFrms = (int)cfg_blk_val[36];
		UtcSetDetectFrmsThred(m_track, detectFrms);

		int  stillFrms;
		stillFrms = (int)cfg_blk_val[37];
		UtcSetStillFrmsThred(m_track, stillFrms);

		float  stillThred;
		stillThred = cfg_blk_val[38];
		UtcSetStillPixThred(m_track, stillThred);


		bool  bKalmanFilter;
		bKalmanFilter = (bool)cfg_blk_val[39];
		UtcSetKalmanFilter(m_track, bKalmanFilter);

		float xMVThred, yMVThred;
		xMVThred = cfg_blk_val[40];
		yMVThred = cfg_blk_val[41];
		UtcSetKFMVThred(m_track, xMVThred, yMVThred);

		float xStillThred, yStillThred;
		xStillThred = cfg_blk_val[42];
		yStillThred = cfg_blk_val[43];
		UtcSetKFStillThred(m_track, xStillThred, yStillThred);

		float slopeThred;
		slopeThred = cfg_blk_val[44];
		UtcSetKFSlopeThred(m_track, slopeThred);

		float kalmanHistThred;
		kalmanHistThred = cfg_blk_val[45];
		UtcSetKFHistThred(m_track, kalmanHistThred);

		float kalmanCoefQ, kalmanCoefR;
		kalmanCoefQ = cfg_blk_val[46];
		kalmanCoefR = cfg_blk_val[47];
		UtcSetKFCoefQR(m_track, kalmanCoefQ, kalmanCoefR);

		bool  bSceneMVRecord;
		bSceneMVRecord = (bool)cfg_blk_val[48];
		
		if(bSceneMVRecord == true)
			wFileFlag = true;
		
		UtcSetSceneMVRecord(m_track, bSceneMVRecord);

		UtcSetRoiMaxWidth(m_track, 400);

		UtcSetPLT_BS(m_track, tPLT_WRK, BoreSight_Mid);
#endif
		return 0;

	}
	else
		return -1;

	
	return 0;
}

int CVideoProcess::Algconfig()
{
//enhment
	if(0==configEnhFromFile())
	{
		switch(Enhmod)
		{
			case 0:
				m_display.enhancemod=0;
				break;
			case 1:
				m_display.enhancemod=1;
				break;
			case 2:
				m_display.enhancemod=2;
				break;
			default:
				m_display.enhancemod=1;
		}
		if((Enhparm>0.0)&&(Enhparm<5.0))
			m_display.enhanceparam=Enhparm;
		else
			m_display.enhanceparam=3.5;

		if((DetectGapparm<0) || (DetectGapparm>15))
			DetectGapparm = 10;
		m_MMTDObj.SetSRDetectGap(DetectGapparm);


		m_MMTDObj.SetConRegMinMaxArea(MinArea, MaxArea);

		m_MMTDObj.SetMoveThred(stillPixel, movePixel);

		m_MMTDObj.SetLapScaler(lapScaler);

		m_MMTDObj.SetSRLumThred(lumThred);
		OSA_printf("DetectGapparm, MinArea, MaxArea,stillPixel, movePixel,lapScaler,lumThred = %d,%d,%d,%d,%d,%f,%d\n",DetectGapparm, MinArea, MaxArea,stillPixel, movePixel,lapScaler,lumThred);

	}
	else
	{
		m_display.enhancemod=1;
		m_display.enhanceparam=3.5;
	}
	
//avtrack
#if __TRACK__
	configAvtFromFile();
#endif

}

int CVideoProcess::run()
{
	MultiCh.run();
	m_display.run();
	
	#if __TRACK__
	m_track = CreateUtcTrk();
	#endif
	//Algconfig();
	for(int i=0; i<MAX_CHAN; i++){
		m_mtd[i] = (target_t *)malloc(sizeof(target_t));
		if(m_mtd[i] != NULL)
			memset(m_mtd[i], 0, sizeof(target_t));

		OSA_printf(" %d:%s mtd malloc %p\n", OSA_getCurTimeInMsec(),__func__, m_mtd[0]);
	}

	m_MMTDObj.SetTargetNum(MAX_TARGET_NUMBER);
	OnRun();
	return 0;
}

int CVideoProcess::stop()
{
	if(m_track != NULL)
	{
		#if __TRACK__
			DestroyUtcTrk(m_track);
		#endif
	}
	m_track = NULL;
	
	m_display.stop();
	MultiCh.stop();

	OnStop();
	return 0;
}

void CVideoProcess::call_run(int value)
{
	pThis->process_event(0, 0, NULL);
}

void CVideoProcess::process_event(int type, int iPrm, void *pPrm)
{

	Ontimer();
	if(type == 0)//timer event from display
	{
	}
	
}

int CVideoProcess::callback_process(void *handle, int chId, Mat frame)
{
	CVideoProcess *pThis = (CVideoProcess*)handle;
	return pThis->process_frame(chId, frame);
}

static void extractYUYV2Gray(Mat src, Mat dst)
{
	int ImgHeight, ImgWidth,ImgStride, stride16x8;

	ImgWidth = src.cols;
	ImgHeight = src.rows;
	ImgStride = ImgWidth*2;
	stride16x8 = ImgStride/16;

	OSA_assert((ImgStride&15)==0);
//#pragma omp parallel for
	for(int y = 0; y < ImgHeight; y++)
	{
		uint8x8_t  * __restrict__ pDst8x8_t;
		uint8_t * __restrict__ pSrc8_t;
		pSrc8_t = (uint8_t*)(src.data+ ImgStride*y);
		pDst8x8_t = (uint8x8_t*)(dst.data+ ImgWidth*y);
		for(int x=0; x<stride16x8; x++)
		{
			uint8x8x2_t d;
			d = vld2_u8((uint8_t*)(pSrc8_t+16*x));
			pDst8x8_t[x] = d.val[0];
		}
	}
}

void CVideoProcess::extractYUYV2Gray2(Mat src, Mat dst)
{
	int ImgHeight, ImgWidth,ImgStride;

	ImgWidth = src.cols;
	ImgHeight = src.rows;
	ImgStride = ImgWidth*2;
	uint8_t  *  pDst8_t;
	uint8_t *  pSrc8_t;

	pSrc8_t = (uint8_t*)(src.data);
	pDst8_t = (uint8_t*)(dst.data);
//#pragma UNROLL 4
//#pragma omp parallel for
	for(int y = 0; y < ImgHeight*ImgWidth; y++)
	{
		pDst8_t[y] = pSrc8_t[y*2];
	}
}


#if __TRACK__
void CVideoProcess::Track_fovreacq(int fov,int sensor,int sensorchange)
{
	//UTC_RECT_float rect;
	unsigned int currentx=0;
	unsigned int currenty=0;
	unsigned int TvFov[3] 	= {120,48,16};//Big-Mid-Sml:2400*5%,960*5%,330*5%
	unsigned int FrFov[5] 	= {200,120,50,16,6};//Big-Mid-Sml-SuperSml-Zoom:4000*5%,2400*5%,1000*5%,330*5%,120*5%

	if(sensorchange == 1){
		currentx = m_ImageAxisx;
		currenty = m_ImageAxisy;
	}
	else
	{		
			
		currentx=trackinfo_obj->trackrect.x+trackinfo_obj->trackrect.width/2;
		currenty=trackinfo_obj->trackrect.y+trackinfo_obj->trackrect.height/2;
		
	}
	int prifov=trackinfo_obj->trackfov;
	
	double ratio=tan(3.1415926*fov/36000)/tan(3.1415926*prifov/36000);
	
	unsigned int w=trackinfo_obj->trackrect.width/ratio;
	unsigned int h=trackinfo_obj->trackrect.height/ratio;
	if(sensorchange)
	{
		w=w*vcapWH[sensor^1][0]/vcapWH[sensor][0];
		h=h*vcapWH[sensor^1][1]/vcapWH[sensor][1];

	}
	
	//OSA_printf("prifov=%d fov=%d  the w=%d  h=%d ratio=%f wt=%f,\n",prifov,fov,w,h,ratio,trackinfo_obj->trackrect.width);
	/*
	if(w>trkWinWH[sensor][4][0])
		w=trkWinWH[sensor][4][0];
	else if(w<trkWinWH[sensor][0][0])
		w=trkWinWH[sensor][0][0];

	if(h>trkWinWH[sensor][4][1])
		h=trkWinWH[sensor][4][1];
	else if(h<trkWinWH[sensor][0][1])
		h=trkWinWH[sensor][0][1];
	*/
	
	trackinfo_obj->trackfov=fov;

	trackinfo_obj->reAcqRect.width=w;
	trackinfo_obj->reAcqRect.height=h;
	trackinfo_obj->reAcqRect.x=currentx-w/2;
	trackinfo_obj->reAcqRect.y=currenty-h/2;

	//OSA_printf("XY(%f,%f),WH(%f,%f)\n",trackinfo_obj->reAcqRect.x,trackinfo_obj->reAcqRect.y,trackinfo_obj->reAcqRect.width,trackinfo_obj->reAcqRect.height);
}
void CVideoProcess::Track_reacq(UTC_RECT_float & rcTrack,int acqinterval)
{
	m_intervalFrame=acqinterval;
	m_rcAcq=rcTrack;
}


int CVideoProcess::ReAcqTarget()
{
	//printf("m_intervalFrame = %d \n\n",m_intervalFrame);
	int iRet = m_iTrackStat;
	if(m_bakChId != m_curChId){
		iRet = 0;
		m_rcTrack = m_rcAcq;
		m_bakChId = m_curChId;
		m_iTrackLostCnt = 0;	
	}
	
	if(m_intervalFrame > 0){
		m_intervalFrame--;
		if(m_intervalFrame == 0){
			iRet = 0;
			m_rcTrack = m_rcAcq;
			m_iTrackLostCnt = 0;
			OSA_printf("----------------Setting m_intervalFrame----------------\n");
		}
	}

	return iRet;

}

#endif

extern void cutColor(cv::Mat src, cv::Mat &dst, int code);

#define TM
#undef TM 
int CVideoProcess::process_frame(int chId, Mat frame)
{
	int format = -1;
	if(frame.cols<=0 || frame.rows<=0)
		return 0;

//	tstart = getTickCount();
	int  channel= frame.channels();


	

#ifdef TM
	cudaEvent_t	start, stop;
		float elapsedTime;
		( (		cudaEventCreate	(	&start)	) );
		( (		cudaEventCreate	(	&stop)	) );
		( (		cudaEventRecord	(	start,	0)	) );
#endif

	if(channel == 2){
//		cvtColor(frame,frame,CV_YUV2BGR_YUYV);
		format = CV_YUV2BGR_YUYV;
	}
	else {
//		cvtColor(frame,frame,CV_GRAY2BGR);
		format = CV_GRAY2BGR;
	}

	OSA_mutexLock(&m_mutex);


	#if 1

	if(chId == m_curChId /*&& (m_bTrack ||m_bMtd )*/)
	{
		mainFrame[mainProcThrObj.pp] = frame;
		mainProcThrObj.cxt[mainProcThrObj.pp].bTrack = m_bTrack;
		mainProcThrObj.cxt[mainProcThrObj.pp].bMtd = m_bMtd;
		mainProcThrObj.cxt[mainProcThrObj.pp].bBlobDetect = m_bBlobDetect;
		mainProcThrObj.cxt[mainProcThrObj.pp].bMoveDetect = m_bMoveDetect;
		mainProcThrObj.cxt[mainProcThrObj.pp].iTrackStat = m_iTrackStat;
		mainProcThrObj.cxt[mainProcThrObj.pp].chId = chId;
		if(mainProcThrObj.bFirst){
			mainFrame[mainProcThrObj.pp^1] = frame;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bTrack = m_bTrack;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bMtd = m_bMtd;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bBlobDetect = m_bBlobDetect;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].bMoveDetect = m_bMoveDetect;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].iTrackStat = m_iTrackStat;
			mainProcThrObj.cxt[mainProcThrObj.pp^1].chId = chId;
			mainProcThrObj.bFirst = false;
		}
		OSA_semSignal(&mainProcThrObj.procNotifySem);
	}

	#endif
		
	//OSA_printf("chid =%d  m_curChId=%d m_curSubChId=%d\n", chId,m_curChId,m_curSubChId);
	if(chId == m_curChId || chId == m_curSubChId)
		{
		
			m_display.display(frame,  chId, format);
		
		}
	

	OSA_mutexUnlock(&m_mutex);


		

//	OSA_printf("process_frame: chId = %d, time = %f sec \n",chId,  ( (getTickCount() - tstart)/getTickFrequency()) );
	//获得结束时间，并显示结果

#ifdef TM
		(	(	cudaEventRecord(	stop,	0	)	)	);
		(	(	cudaEventSynchronize(	stop)	)	);

		(	cudaEventElapsedTime(	&elapsedTime,	start,	stop)	);
		OSA_printf("ChId = %d, Time to YUV2RGB:	%3.1f	ms \n", chId, elapsedTime);

		(	(	cudaEventDestroy(	start	)	)	);
		(	(	cudaEventDestroy(	stop	)	)	);
#endif
	return 0;
}


#if __TRACK__
int CVideoProcess::process_track(int trackStatus, Mat frame_gray, Mat frame_dis, UTC_RECT_float &rcResult)
{
	IMG_MAT image;

	image.data_u8 = frame_gray.data;
	image.width = frame_gray.cols;
	image.height = frame_gray.rows;
	image.channels = 1;
	image.step[0] = image.width;
	image.dtype = 0;
	image.size = frame_gray.cols*frame_gray.rows;

	if(trackStatus != 0)
	{
		rcResult = UtcTrkProc(m_track, image, &trackStatus);
	}
	else
	{
		//printf("track********x=%f y=%f w=%f h=%f  ax=%d xy=%d\n",rcResult.x,rcResult.y,rcResult.width,rcResult.height);
		UTC_ACQ_param acq;
		acq.axisX =m_ImageAxisx;// image.width/2;//m_ImageAxisx;//
		acq.axisY =m_ImageAxisy;//image.height/2;//m_ImageAxisy;//
		acq.rcWin.x = (int)(rcResult.x);
		acq.rcWin.y = (int)(rcResult.y);
		acq.rcWin.width = (int)(rcResult.width);
		acq.rcWin.height = (int)(rcResult.height);

		if(acq.rcWin.width<0)
			{
				acq.rcWin.width=0;

			}
		else if(acq.rcWin.width>= image.width)
			{
				acq.rcWin.width=60;
			}
		if(acq.rcWin.height<0)
			{
				acq.rcWin.height=0;

			}
		else if(acq.rcWin.height>= image.height)
			{
				acq.rcWin.height=60;
			}
		if(acq.rcWin.x<0)
			{
				acq.rcWin.x=0;
			}
		else if(acq.rcWin.x>image.width-acq.rcWin.width)
			{

				acq.rcWin.x=image.width-acq.rcWin.width;
			}
		if(acq.rcWin.y<0)
			{
				acq.rcWin.y=0;
			}
		else if(acq.rcWin.y>image.height-acq.rcWin.height)
			{

				acq.rcWin.y=image.height-acq.rcWin.height;
			}

		//OSA_printf("---*****Enter UtcTrkAcq*****---\n");
		//rcResult = UtcTrkAcq(m_track, image, acq);
		#if 1
		if(moveStat == true)
		{
			printf("=========movestat = %d\n",moveStat);
			rcResult = UtcTrkAcq(m_track, image, acq);
			moveStat = false;
		}
		else
			rcResult = UtcTrkAcqSR(m_track, image, acq, true);
		#endif
		trackStatus = 1;
	}

	return trackStatus;
}

#endif

vector<Rect> Box(MAX_TARGET_NUMBER);
int CVideoProcess::process_mtd(ALGMTD_HANDLE pChPrm, Mat frame_gray, Mat frame_dis)
{
	

#if 0
	if(pChPrm != NULL && (pChPrm->state > 0))
	{
//		medium.create(frame_gray.rows, frame_gray.cols, frame_gray.type());

//		MediumFliter(frame_gray.data, medium.data, pChPrm->i_width, pChPrm->i_height);

		GaussFliter(frame_gray.data, pChPrm->Img[0], pChPrm->i_width, pChPrm->i_height);
		

		for(i = 0; i < MAX_SCALER; i++)
		{
			DownSample(pChPrm, pChPrm->Img[i], pChPrm->Img[i+1],
										pChPrm->i_width>>i, pChPrm->i_height>>i);
		}

		IMG_sobel(pChPrm->Img[1], pChPrm->sobel, pChPrm->i_width>>1,	 pChPrm->i_height>>1);

		AutoDetectTarget(pChPrm, frame_gray.data);

		FilterMultiTarget(pChPrm);

	}
#endif
	return 0;
}

#if __MOVE_DETECT__
#if __DETECT_SWITCH_Z__
void	CVideoProcess::initMvDetect()
{
	int	i;
	OSA_printf("%s:mvDetect start ", __func__);
	OSA_assert(m_pMovDetector != NULL);

	m_pMovDetector->init(NotifyFunc, (void*)this);
	
	std::vector<cv::Point> polyWarnRoi ;
	polyWarnRoi.resize(4);
	polyWarnRoi[0]	= cv::Point(0,0);
	polyWarnRoi[1]	= cv::Point(1920,0);
	polyWarnRoi[2]	= cv::Point(1920,1080);
	polyWarnRoi[3]	= cv::Point(0,1080);
	for(i=0; i<DETECTOR_NUM; i++)
	{
		m_pMovDetector->setWarningRoi(polyWarnRoi,	i);
		m_pMovDetector->setDrawOSD(m_dccv, i);
		m_pMovDetector->enableSelfDraw(false, i);
		m_pMovDetector->setWarnMode(WARN_MOVEDETECT_MODE, i);
	} 
}

void	CVideoProcess::DeInitMvDetect()
{
	if(m_pMovDetector != NULL)
		m_pMovDetector->destroy();
}

void CVideoProcess::NotifyFunc(void *context, int chId)
{
	//int num;
	CVideoProcess *pParent = (CVideoProcess*)context;
	//pParent->m_display.m_bOsd = true;
	
	pThis->m_pMovDetector->getMoveTarget(pThis->detect_vect,0);

	//pParent->m_display.UpDateOsd(1);
}
#endif
#endif

void CVideoProcess::getImgRioDelta(unsigned char* pdata,int width ,int height,UTC_Rect rio,double * value)
{
	unsigned char * ptr = pdata;
	int Iij;
	//gray max ,gray min ,gray average,gray delta
	double Imax = 0, Imin = 255, Iave = 0, Idelta = 0;
	for(int i=rio.y;i<rio.height+rio.y;i++)
	{
		for(int j=rio.x;j<rio.width+rio.x;j++)
		{
			Iij	= ptr[i*width+j];
			if(Iij > Imax)
				Imax = Iij;
			if(Iij < Imin)
				Imin = Iij;
			Iave = Iave + Iij;
		}
	}
	Iave = Iave/(rio.width*rio.height);
	for(int i=rio.y;i<rio.height+rio.y;i++)
	{
		for(int j=rio.x;j<rio.width+rio.x;j++)
		{
			Iij 	=  ptr[i*width+j];
			Idelta = Idelta + (Iij-Iave)*(Iij-Iave);
		}
	}
	Idelta = Idelta/(rio.width*rio.height);
	*value = Idelta;
}


