
#ifndef _GLOBAL_STATUS_H_
#define _GLOBAL_STATUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "app_proj_xgs026.h"

#ifndef MTD_TARGET_NUM
#define MTD_TARGET_NUM  8
#endif

typedef enum {
	eSen_TV	= 0x00,
	eSen_FR    = 0x01,
	eSen_Max   = 0x02
}eSenserStat;
typedef enum {
	ePicp_top_left = 0x00,
	ePicp_top_right = 0x01,
	ePicp_bot_left = 0x02,
	ePicp_bot_right = 0x03,
}ePicpPosStat;

typedef enum Dram_SysMode
{
    INIT_MODE           = 0x00,
    CHECK_MODE      = 0x01,
    AXIS_MODE           = 0x02,
    NORMAL_MODE     = 0x03,
} eSysMode;

typedef enum Dram_zoomCtrl
{
    eZoom_Ctrl_Pause    = 0x00,
    eZoom_Ctrl_SCAL2    = 0x01,
    eZoom_Ctrl_SCAL4    = 0x02,
    eZoom_Ctrl_ADD      = 0x01,
    eZoom_Ctrl_SUB      = 0x02,
} eZoomCtrl;

typedef enum Dram_ImgAlg
{
    eImgAlg_Disable     = 0x00,
    eImgAlg_Enable      = 0x01,
} eImgAlgStat;  // use by img zoom/enh/stb/mtd/rst

typedef enum Dram_MMTSelect
{
    eMMT_No     = 0x00,
    eMMT_Next       = 0x01,
    eMMT_Prev       = 0x02,
    eMMT_Select = 0x03,
} eMMTSelect;

typedef enum Dram_DispGradeStat
{
    eDisp_hide      = 0x00,
    eDisp_show_rect     = 0x01,
    eDisp_show_text     = 0x02,
    eDisp_show_dbg      = 0x04,
} eDispGrade;

typedef enum Dram_DispGradeColor
{
    ecolor_Default  = 0x0,
    ecolor_Black = 0x1,
    ecolor_White    = 0x2,
    ecolor_Red = 0x03,
    ecolor_Yellow = 0x04,
    ecolor_Blue = 0x05,
    ecolor_Green = 0x06,
} eOSDColor;

typedef enum
{
    eTrk_Acq        = 0x00,
    eTrk_Normal = 0x01,
    eTrk_Assi       = 0x02,
    eTrk_Lost       = 0x03,
} eTrkStat;

typedef enum Dram_trkMode
{
    // mode cmd
    eTrk_mode_acq       = 0x00,
    eTrk_mode_target    = 0x01,
    eTrk_mode_mtd       = 0x02,
    eTrk_mode_sectrk    = 0x03,
    eTrk_mode_search    = 0x04,

    eTrk_mode_switch      = 0x100,

} eTrkMode;

typedef enum Dram_trkRefine
{
    eTrk_ref_no     = 0x00,
    eTrk_ref_left   = 0x01,
    eTrk_ref_right  = 0x02,
    eTrk_ref_up     = 0x01,
    eTrk_ref_down   = 0x02,
} eTrkRefine;

typedef enum Dram_saveMode
{
    eSave_Disable       = 0x00,
    eSave_Enable        = 0x01,
    eSave_Cancel        = 0x02,
} eSaveMode;


/** universal status **/
typedef struct
{
	volatile unsigned int  unitVerNum;      // 1.23=>0x0123
	volatile unsigned int  unitFaultStat;   // bit0:tv input bit1:fr input bit2:avt21
	volatile unsigned int  unitFaultStatpri;   // bit0:tv input bit1:fr input bit2:avt21
	volatile unsigned char  SysMode; 	// 0 --- init ; 1 ---normal  2---settiing
	volatile unsigned char  FovCtrl; 
	volatile unsigned char  FovStat;       /* 1 byte ext-input fov:0 Large fov ,1 midle fov,2 small fov,3 electric x2 fov */
	volatile float  unitFovAngle[eSen_Max];
	
	volatile unsigned int  ImgPixelX[eSen_Max];    // for img trk alg
	volatile unsigned int  ImgPixelY[eSen_Max];    // for img trk alg
	
	volatile unsigned int unitTrkStatpri;
	volatile unsigned int  unitTrkStat;     // acp/trk/assi/lost
	volatile  int  unitAxisX[eSen_Max];     // pixel
	volatile  int  unitAxisY[eSen_Max];     // pixel
	volatile  int  NaimX;	// sectrk aim the next obj that need to be tracked
	volatile  int  NaimY;	// sectrk aim the next obj that need to be tracked
	
	volatile unsigned int  unitAimW;      // aim size
	volatile unsigned int  unitAimH;      // aim size
	volatile unsigned int  unitAimX;
	volatile unsigned int  unitAimY;
	
	volatile float unitTrkX;    		// for report and osd text 	what
	volatile float unitTrkY;    		// for report and osd text
	volatile float unitTrkXtmp;    	// for report and osd text
	volatile float unitTrkYtmp;    	// for report and osd text

	volatile unsigned int  AvtTrkStat;      		// eTrkMode 
	volatile unsigned int  AvtTrkAimSize;   	// 0-4
	volatile unsigned int  AvtCfgSave;      	// eSaveMode
	volatile unsigned int  AvtTrkCoast;
	volatile unsigned int  TrkErrFeedback;  	// eTrkMode 

	volatile int  trkerrx;
	volatile int  trkerry;
	volatile  short  TrkPanev;
	volatile  short  TrkTitlev;
	
	volatile  int  AvtMoveX;        		// eTrkRefine (axis or aim)
	volatile  int  AvtMoveY;        		// eTrkRefine (axis or aim)
	volatile  int  AvtPosXTv;        		// eTrkRefine (axis or aim)
	volatile  int  AvtPosYTv;        		// eTrkRefine (axis or aim)
	volatile  int  AvtPosXFir;        		// eTrkRefine (axis or aim)
	volatile  int  AvtPosYFir;        		// eTrkRefine (axis or aim)
	volatile  int  CollPosXFir;        		// eTrkRefine (axis or aim)
	volatile  int  CollPosYFir;        		// eTrkRefine (axis or aim)
	volatile unsigned int  AvtPixelX;        // for ext designated
	volatile unsigned int  AvtPixelY;        // for ext designated

	/***** cmd stat part *****/
	volatile unsigned int  SensorStat;      		// eSenserStat
	volatile unsigned int  changeSensorFlag;
	volatile unsigned int  PicpSensorStat;  		// sensor src id range 0~3 or 0xFF no picp sens
	volatile unsigned int  PicpSensorStatpri; 		// sensor src id range 0~3 or 0xFF no picp sens
	volatile unsigned int  PicpPosStat;			// ePicpPosStat
	volatile unsigned int  ImgZoomStat[eSen_Max];   	// eImgAlgStat	electric
	volatile unsigned int  ImgEnhStat[eSen_Max];    	// eImgAlgStat
	volatile unsigned int  ImgBlobDetect[eSen_Max];    // eImgAlgStat
	volatile unsigned int  ImgFrezzStat[eSen_Max];    	// eImgAlgStat
	volatile unsigned int  ImgVideoTrans[eSen_Max];   // eImgAlgStat
	volatile unsigned int  ImgPicp[eSen_Max];   	 	// eImgAlgStat

	volatile unsigned int  unitMtdValid;    			// 0-disable 1-valid 2-unvalid
	volatile unsigned int  unitMtdPixelX;
	volatile unsigned int  unitMtdPixelY;
	volatile unsigned int  ImgMtdStat[eSen_Max];    	// eImgAlgStat
	volatile unsigned int  ImgMtdSelect[eSen_Max];  	// eMMTSelect or range 0-MTD_TARGET_NUM

	volatile unsigned int  	ImgMmtshow[eSen_Max];	
	volatile unsigned char  	MMTTempStat;		//for ack mmt stat
	volatile unsigned char 	MtdOffsetXY[20]; 		//mtd xy
	volatile unsigned char 	Mtdtargetnum; 		//mtd xy

	/***** cmd osd part *****/
	volatile unsigned int  DispGrp[eSen_Max];       	// eDispGrade
	volatile unsigned int  DispColor[eSen_Max];  	// eOSDColor or eGRPColor
	
	//don't know the usage
	volatile unsigned char 	TrkBomenCtrl; // osd Trk Aim
	volatile unsigned char SecAcqFlag;
	volatile unsigned char SecAcqStat;
	volatile unsigned int  TrkCoastCount;
	volatile unsigned int  FreezeresetCount;
	volatile unsigned int  unitTvCollX;
	volatile unsigned int  unitTvCollY;

	// wait to change or remove
	volatile unsigned char TargetPal; // fr target pal 

	//may be not useful
	volatile unsigned char 	CmdType;  // recv cmd id		no used now
	volatile unsigned int  TvCollimation;   //dianshi zhunzhi
	volatile unsigned int  FrCollimation;   //rexiang zhunzhi
} CMD_EXT;

typedef struct
{
    union
    {
        unsigned char c[4];
        float         f;
        int           i;
    } un_data;
} ACK_REG;

typedef struct
{
    unsigned int  NumItems;
    unsigned int  Block[16];        // ackBlk
    ACK_REG Data[16];
} ACK_EXT;

typedef enum 
{
    MSGID_SYS_INIT  = 0x00000000,           ///< internal cmd system init.
    MSGID_SYS_RESET,
    //MSGID_AVT_RESET,

    MSGID_EXT_INPUT_SYSMODE = 0x00000010,   ///< external cmd, system work mode.
    MSGID_EXT_INPUT_SENSOR,                 ///< external cmd, switch sensor.

    MSGID_EXT_INPUT_FOVSTAT,                ///< external cmd, switch fov.
    MSGID_EXT_INPUT_CFGSAVE,                ///< external cmd, config save(here only for axis save)
    MSGID_EXT_INPUT_AXISPOS,                ///< external cmd, set pos of cross axis.
    MSGID_EXT_INPUT_TRACKALG,             ///< external cmd, set track alg.
    MSGID_EXT_INPUT_TRACK,                  ///< external cmd, start track or stop.
    MSGID_EXT_INPUT_AIMPOS,                 ///< external cmd, set pos of aim area.
    MSGID_EXT_INPUT_AIMSIZE,                ///< external cmd, set size of aim area.
    MSGID_EXT_INPUT_COAST,              ///< external cmd, coast set or cannel
    MSGID_EXT_INPUT_FOVSELECT,        //FOVSELECT
    MSGID_EXT_INPUT_SEARCHMOD,        //FOVSELECT
    MSGID_EXT_INPUT_FOVCMD,        //FOVSELECT
    

    // img control
    MSGID_EXT_INPUT_ENPICP = 0x00000020,        ///< external cmd, open picture close.
    MSGID_EXT_INPUT_ENZOOM,                 ///< external cmd, zoom near or far.
    MSGID_EXT_INPUT_ENPICPZOOM,             ///< external cmd, picp zoom near or far.
    MSGID_EXT_INPUT_ENENHAN,                ///< external cmd, open image enhan or close.
    MSGID_EXT_INPUT_ENMTD,                  ///< external cmd, open mtd or close.
    MSGID_EXT_INPUT_MTD_SELECT,              ///< external cmd, select mtd target.
    MSGID_EXT_INPUT_ENSTB,                  ///< external cmd, open image stb or close.
    MSGID_EXT_INPUT_ENRST,                  ///< external cmd, open image rst or close.
    MSGID_EXT_INPUT_RST_THETA,              ///< external cmd, open image rst or close.
    MSGID_EXT_INPUT_ENBDT,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_ENFREZZ,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_PICPCROP,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_VIDEOEN,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_MMTSHOW,              ///< external cmd, open image bdt or close.
    MSGID_EXT_INPUT_MMTSHOWUPDATE,              ///< external cmd, open image bdt or close.

    // osd control
    MSGID_EXT_INPUT_DISPGRADE = 0x00000030,  ///< external cmd, osd show or hide
    MSGID_EXT_INPUT_DISPCOLOR,              ///< external cmd, osd color
    MSGID_EXT_INPUT_COLOR,                 ///< external cmd, switch input video color.

    // video control
    MSGID_EXT_INPUT_VIDEOCTRL,              ///< external cmd, video record or replay.
    MSGID_EXT_MVDETECT,
}MSG_PROC_ID;

typedef struct
{
	unsigned int  CaptureWidth  [eSen_Max];    // for width
	unsigned int  CaptureHeight [eSen_Max];    // for height
	unsigned int  CaptureType   [eSen_Max];	   //date type
	unsigned int  DispalyWidth   [eSen_Max];    // dispaly width
	unsigned int  DispalyHeight  [eSen_Max];    // display height
}CMD_SYS;

#define 	TVBIGFOV 			(2400)
#define 	FIRBIGFOV 			(40)
#define 	_IMAGE_WIDTH_ 	1920
#define 	_IMAGE_HEIGHT_ 	1080

#ifdef __cplusplus
}
#endif

#endif


