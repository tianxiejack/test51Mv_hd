
#include"app_ctrl.h"
#include"osa.h"
#include "msgDriv.h"
#include"dx.h"
#include "configable.h"

CMD_EXT *msgextInCtrl;
#define Coll_Save 0 //   1:quit coll is to save  cross  or  0:using save funtion to cross axis
#define FrColl_Change 1 //0:frcoll v1.00 1:frcoll v1.01     //ver1.01 is using 

static int pristatus=0;
void getMmtTg(unsigned char index,int *x,int *y);



void  app_ctrl_getSysData(CMD_EXT * exthandle)
{
    OSA_assert(exthandle!=NULL);
    if(msgextInCtrl==NULL)
	return ;
    memcpy(exthandle,msgextInCtrl,sizeof(CMD_EXT));
    return ;
}



void app_ctrl_setTrkStat(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;
	
    if ((pInCmd->AvtTrkStat != pIStuts->AvtTrkStat) || pIStuts->AvtTrkStat == eTrk_mode_sectrk)
    {
        pIStuts->AvtTrkStat = pInCmd->AvtTrkStat;
	 if((pIStuts->AvtTrkStat==eTrk_mode_search)||(pIStuts->AvtTrkStat==eTrk_mode_sectrk))
	{
		pIStuts->AvtPixelX = pInCmd->ImgPixelX[pIStuts->SensorStat] ;
		pIStuts->AvtPixelY = pInCmd->ImgPixelY[pIStuts->SensorStat] ;

		pIStuts->NaimX = pInCmd->NaimX;
		pIStuts->NaimY = pInCmd->NaimY;
	}

	MSGDRIV_send(MSGID_EXT_INPUT_TRACK, 0);
    }
   return ;
}



void app_ctrl_setSysmode(CMD_EXT * pInCmd)
{
	 if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
        if(pInCmd->SysMode != pIStuts->SysMode)
            pIStuts->SysMode = pInCmd->SysMode;

        return ;
}

unsigned char app_ctrl_getSysmode()
{
	if(msgextInCtrl==NULL)
		return 255;
	return  msgextInCtrl->SysMode;
}



void app_ctrl_setAimPos(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;

	if (pIStuts->AvtMoveX != pInCmd->AvtMoveX ||pIStuts->AvtMoveY != pInCmd->AvtMoveY){
	    pIStuts->AvtMoveX = pInCmd->AvtMoveX;
	    pIStuts->AvtMoveY = pInCmd->AvtMoveY;
	}
	MSGDRIV_send(MSGID_EXT_INPUT_AIMPOS, 0);
	return ;
}


void app_ctrl_setMmtSelect(CMD_EXT * pIStuts,unsigned char index)
{	
	int curx,cury;
	getMmtTg(index, &curx, &cury);
	
	pIStuts->AvtTrkStat = eTrk_mode_sectrk;
	pIStuts->NaimX = curx;
	pIStuts->NaimY = cury;
	app_ctrl_setTrkStat(pIStuts);

	pIStuts->AvtPosXTv = pIStuts->unitAxisX[eSen_TV];
	pIStuts->AvtPosYTv = pIStuts->unitAxisY[eSen_TV];
	app_ctrl_setAxisPos(pIStuts);
	return ;
}


void app_ctrl_setEnhance(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;

	if(pInCmd->ImgEnhStat[pInCmd->SensorStat] != pIStuts->ImgEnhStat[pInCmd->SensorStat])
	{
		pIStuts->ImgEnhStat[pInCmd->SensorStat] = pInCmd->ImgEnhStat[pInCmd->SensorStat];
		if(pIStuts->ImgEnhStat[pInCmd->SensorStat]==0)
		{
			pIStuts->ImgEnhStat[pInCmd->SensorStat^1]=0;
		}
		MSGDRIV_send(MSGID_EXT_INPUT_ENENHAN, 0);
	}
	return ;
}


void app_ctrl_setAxisPos(CMD_EXT * pInCmd)
{
       if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
	if(pInCmd->axisMoveStepX  || pInCmd->axisMoveStepY)
	{
		pIStuts->axisMoveStepX = pInCmd->axisMoveStepX;
		pIStuts->axisMoveStepY = pInCmd->axisMoveStepY;
		MSGDRIV_send(MSGID_EXT_INPUT_AXISPOS, 0);	
	}
	else if(pIStuts->AvtPosXTv != pInCmd->AvtPosXTv || pIStuts->AvtPosYTv != pInCmd->AvtPosYTv)
	{
		pIStuts->AvtPosXTv = pInCmd->AvtPosXTv;
		pIStuts->AvtPosYTv = pInCmd->AvtPosYTv;
		MSGDRIV_send(MSGID_EXT_INPUT_AXISPOS, 0);
	}
	return ;
}


void app_ctrl_setMtdStat(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;

	if(pIStuts->MtdState[pIStuts->validChId] != pInCmd->MtdState[pIStuts->validChId])
	{
		pIStuts->MtdState[pIStuts->validChId] = pInCmd->MtdState[pIStuts->validChId];
		MSGDRIV_send(MSGID_EXT_MVDETECT, 0);
	}
	return ;
}

unsigned char app_ctrl_getMtdStat()
{
	if(msgextInCtrl==NULL)
		return 0xff;
	CMD_EXT *pIStuts = msgextInCtrl;

	return pIStuts->MtdState[pIStuts->validChId];
}


#if 1	//will be revise in next step

void app_ctrl_setMMT(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;

	if(pInCmd->MMTTempStat != pIStuts->MMTTempStat)
		pIStuts->MMTTempStat = pInCmd->MMTTempStat;

	if (pIStuts->ImgMtdStat[0] != pInCmd->ImgMtdStat[0] || pIStuts->ImgMtdStat[1] != pInCmd->ImgMtdStat[1])
	{     
		pIStuts->ImgMtdStat[0] = pInCmd->ImgMtdStat[0];
		pIStuts->ImgMtdStat[1] = pInCmd->ImgMtdStat[1];
		if(pInCmd->AvtTrkStat == eTrk_mode_acq)
		{
		    MSGDRIV_send(MSGID_EXT_INPUT_ENMTD, 0);
		}
	}

	if(pIStuts->MMTTempStat==3||pIStuts->MMTTempStat==4)
	{
		MSGDRIV_send(MSGID_EXT_INPUT_MTD_SELECT, 0);
	}
   
   return ;
}


#endif
//********************************************

void app_ctrl_Sensorchange(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;
	pIStuts->unitAimX=pIStuts->unitAxisX[pIStuts->SensorStat];
	pIStuts->unitAimY=pIStuts->unitAxisY[pIStuts->SensorStat];
}

void app_ctrl_setReset(CMD_EXT * pInCmd)
{

	if(msgextInCtrl==NULL)
		return ;
       CMD_EXT *pIStuts = msgextInCtrl;
	if(pIStuts->AvtTrkStat != eTrk_mode_acq)
		{
			pIStuts->AvtTrkStat = eTrk_mode_acq;
			 MSGDRIV_send(MSGID_EXT_INPUT_TRACK, 0);
		}
	if(pIStuts->ImgMtdStat[pIStuts->SensorStat] != eImgAlg_Disable)
		{
			pIStuts->ImgMtdStat[pIStuts->SensorStat] = eImgAlg_Disable;
			  MSGDRIV_send(MSGID_EXT_INPUT_ENMTD, 0);
		}
	if(pIStuts->TvCollimation==1)
		{
			pIStuts->TvCollimation=2;
			MSGDRIV_send(MSGID_EXT_INPUT_ENBDT, 0);
		}
	if( pIStuts->SecAcqFlag==1)
		{
		 	  pIStuts->SecAcqFlag = pIStuts->SecAcqStat = 0x00;

		}
	if(pIStuts->ImgEnhStat[pIStuts->SensorStat] == 0x01)
		{
			pIStuts->ImgEnhStat[pIStuts->SensorStat] = 0x00;
			MSGDRIV_send(MSGID_EXT_INPUT_ENENHAN, 0);
		}
	if(pIStuts->ImgPicp[pIStuts->SensorStat] == 0x01)
		{
			pIStuts->ImgPicp[pIStuts->SensorStat] = 0x02;
			MSGDRIV_send(MSGID_EXT_INPUT_PICPCROP, 0);
		}
	if(pIStuts->FrCollimation==0x01)
		{
			pIStuts->FrCollimation=0x02;
			MSGDRIV_send(MSGID_EXT_INPUT_ENFREZZ, 0);
			
		}


}

void app_ctrl_freezeReset(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
	    return ;
       CMD_EXT *pIStuts = msgextInCtrl;
	pIStuts->FreezeresetCount = 25;//default force trk 1second

	if((pristatus&0x03)!=(pIStuts->unitFaultStat&0x03))
		{
			pristatus=pIStuts->unitFaultStat;
			//MSGAPI_AckSnd( AckPeroidChek);
		}
	//printf("*************************\n");


}

void app_ctrl_setSensor(CMD_EXT * pInCmd)
{
  
  
	if(msgextInCtrl==NULL)
		return ;

    CMD_EXT *pIStuts = msgextInCtrl;

    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;
    
    if (pIStuts->SensorStat != pInCmd->SensorStat)
    {
    	pIStuts->changeSensorFlag = 1;
        pIStuts->SensorStat = pInCmd->SensorStat;
        //pIStuts->AvtMoveX = pIStuts->AvtMoveY = 0; //switch sensor axisxy clean
        //for  reply
        //MSGAPI_AckSnd(AckSensor);
	 app_ctrl_Sensorchange(pInCmd);
	 OSA_printf("the sensorstat=%d  x=%d y=%d\n",pIStuts->SensorStat,pIStuts->unitAxisX[pIStuts->SensorStat ],pIStuts->unitAxisY[pIStuts->SensorStat ]);
        MSGDRIV_send(MSGID_EXT_INPUT_SENSOR, 0);
    }
    else
           ;// MSGAPI_AckSnd(AckSensor);

   return ;
}












void app_ctrl_setForceCoast(CMD_EXT * pInCmd)
{
    if(msgextInCtrl==NULL)
		return ;
     CMD_EXT *pIStuts = msgextInCtrl;


     
    /** < set avt21 config */
    if ((pIStuts->AvtTrkStat != eTrk_mode_target)
                    && (pIStuts->AvtTrkStat != eTrk_mode_mtd))
        return ;

     pIStuts->AvtTrkCoast = 1;
     pIStuts->TrkCoastCount = (1000 /40) *2;//default force trk 2 second
     
     MSGDRIV_send(MSGID_EXT_INPUT_COAST, 0);
   
   return ;
}


void app_ctrl_setTrkBomen(CMD_EXT * pInCmd)
{
         if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;

    if(pInCmd->TrkBomenCtrl != 0x01 && pInCmd->TrkBomenCtrl != 0x02)
    {
            //MSGAPI_AckSnd(AckWaveDoor);
            return ;
    }
    if(pInCmd->TrkBomenCtrl != pIStuts->TrkBomenCtrl)
        pIStuts->TrkBomenCtrl = pInCmd->TrkBomenCtrl;

    /***for **reply*****/
    //MSGAPI_AckSnd(AckWaveDoor);
   
   return ;
}






void app_ctrl_setZoom(CMD_EXT * pInCmd)
{
            if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
/*
    if(pIStuts->SensorStat != eSen_TV_Stat)
          return ;
  */  
    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;
           
    // (pIStuts->ImgZoomStat[0] != pInCmd->ImgZoomStat[0] || pIStuts->ImgZoomStat[1] != pInCmd->ImgZoomStat[1])
    {
        pIStuts->ImgZoomStat[0] = pInCmd->ImgZoomStat[0];
        pIStuts->ImgZoomStat[1] = pInCmd->ImgZoomStat[0];
        MSGDRIV_send(MSGID_EXT_INPUT_ENZOOM, 0);
    }

    /***for **reply*****/
    if(pIStuts->CmdType == CmdZoom)
    	;//MSGAPI_AckSnd(AckZoom);
    else if(pIStuts->CmdType == CmdSmallest)
	;//MSGAPI_AckSnd(AckSmallest);
		
   return ;
}



void app_ctrl_setFRColl(CMD_EXT * pInCmd)
{
    if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
    int iMoveMask = 0,iSens = 0,iShow = 0;

    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;


    if( pInCmd->unitAxisX[pIStuts->SensorStat ] != pIStuts->unitAxisX[pIStuts->SensorStat ] || pInCmd->unitAxisY[pIStuts->SensorStat ] != pIStuts->unitAxisY[pIStuts->SensorStat ])
    {
	    if((pInCmd->unitAxisX[pIStuts->SensorStat ]>0)&&(pInCmd->unitAxisX[pIStuts->SensorStat ]<_IMAGE_WIDTH_))
	        pIStuts->unitAxisX[pIStuts->SensorStat ] = pInCmd->unitAxisX[pIStuts->SensorStat ] ;
	   if((pInCmd->unitAxisY[pIStuts->SensorStat ]>0)&&(pInCmd->unitAxisY[pIStuts->SensorStat ]<_IMAGE_HEIGHT_))
	        pIStuts->unitAxisY[pIStuts->SensorStat ] = pInCmd->unitAxisY[pIStuts->SensorStat ];
	        iMoveMask++;
    }

    if(pInCmd->FrCollimation != pIStuts->FrCollimation)
    {
        pIStuts->FrCollimation = pInCmd->FrCollimation;
        iMoveMask++;
    }

    iSens = pIStuts->SensorStat;
   

    if( pInCmd->CollPosXFir != pIStuts->CollPosXFir || pInCmd->CollPosYFir != pIStuts->CollPosYFir)
    {
        pIStuts->CollPosXFir = pInCmd->CollPosXFir ;
        pIStuts->CollPosYFir = pInCmd->CollPosYFir;
        iMoveMask++;
	OSA_printf("the  fir coll AvtPixelX=%d AvtPixelY=%d\n",pIStuts->CollPosXFir,pIStuts->CollPosYFir);
    }

    if(pInCmd->FrCollimation != pIStuts->FrCollimation)
    {
        pIStuts->FrCollimation = pInCmd->FrCollimation;
        iMoveMask++;
    }

    iSens = pIStuts->SensorStat;


    /***for **reply*****/
    //MSGAPI_AckSnd( AckFrColl);
   MSGDRIV_send(MSGID_EXT_INPUT_ENFREZZ, 0);

   return ;
}

/*******************************************************************************
  Function:    void app_ctrl_setTvColl()
  Description: this function is used to
  Calls:       null
  Called By:
  Input:
  Output:      null
  Return:      null
  Others:
*******************************************************************************/
void app_ctrl_setTvColl(CMD_EXT * pInCmd)
{
    if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;

    if(pInCmd->TvCollimation != pIStuts->TvCollimation)
    {
        pIStuts->TvCollimation = pInCmd->TvCollimation ;
        /***for **reply*****/
     //   MSGAPI_AckSnd( AckTvColl);
        
        MSGDRIV_send(MSGID_EXT_INPUT_ENBDT, 0);

    }
   // else
          //MSGAPI_AckSnd( AckTvColl);
   return ;
}



void app_ctrl_setAimSize(CMD_EXT * pInCmd)
{
	if(msgextInCtrl==NULL)
		return ;
	CMD_EXT *pIStuts = msgextInCtrl;
	
	if (pIStuts->AvtTrkAimSize != pInCmd->AvtTrkAimSize)
	{
		pIStuts->AvtTrkAimSize = pInCmd->AvtTrkAimSize;
		MSGDRIV_send(MSGID_EXT_INPUT_AIMSIZE, 0);
	}
	return ;
}


void app_ctrl_setTargetPal(CMD_EXT * pInCmd)
{
    if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

    if(pInCmd->CmdType != pIStuts->CmdType)
		pIStuts->CmdType = pInCmd->CmdType;
    
    if(pInCmd->TargetPal != 0x01 &&
                 pInCmd->TargetPal != 0x02)
    {
        //MSGAPI_AckSnd( AckTarPal);
        return ;
    }
    
   if(pInCmd->TargetPal != pIStuts->TargetPal)
   	    pIStuts->TargetPal = pInCmd->TargetPal;
   
   /***for **reply*****/
   //MSGAPI_AckSnd( AckTarPal);
   
   return ;
}



void app_ctrl_setFovCtrl(CMD_EXT * pInCmd)
{
      if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
    float fDiffMask = 0.0;

    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;

    if(pIStuts->FovCtrl != pInCmd->FovCtrl)
    {
		pIStuts->FovCtrl = pInCmd->FovCtrl;
		MSGDRIV_send(MSGID_EXT_INPUT_FOVCMD, 0);
    }
    //MSGDRIV_send(MSGID_EXT_INPUT_FOVCMD, 0);

    /***for **reply*****/
    //MSGAPI_AckSnd( AckFov);
   return ;
}


void app_ctrl_setSerTrk(CMD_EXT * pInCmd )
{
	 if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

	

	if((pInCmd->ImgPixelX[pIStuts->SensorStat] != pIStuts->ImgPixelX[pIStuts->SensorStat])
		|| (pInCmd->ImgPixelY[pIStuts->SensorStat] != pIStuts->ImgPixelY[pIStuts->SensorStat]))
	{
		pIStuts->ImgPixelX[pIStuts->SensorStat] = pInCmd->ImgPixelX[pIStuts->SensorStat];
		pIStuts->ImgPixelY[pIStuts->SensorStat] = pInCmd->ImgPixelY[pIStuts->SensorStat];
	}

	if(pInCmd->SecAcqStat != pIStuts->SecAcqStat)
		pIStuts->SecAcqStat = pInCmd->SecAcqStat;
	
	if(pInCmd->SecAcqFlag != pIStuts->SecAcqFlag)
		pIStuts->SecAcqFlag = pInCmd->SecAcqFlag;

    	if(pInCmd->CmdType != pIStuts->CmdType)
		pIStuts->CmdType = pInCmd->CmdType;

	if(pIStuts->CmdType == CmdSerTrk)
	    ;//MSGAPI_AckSnd( AckSerTrk);

	return ;	
}



void app_ctrl_poweron(CMD_EXT * pInCmd )
{
	 if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

	return ;	
}



void app_ctrl_setFovselect(CMD_EXT * pInCmd)
{
	 if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
        if(pInCmd->FovStat != pIStuts->FovStat)
            pIStuts->FovStat = pInCmd->FovStat;
	MSGDRIV_send(MSGID_EXT_INPUT_FOVSELECT, 0);

        return ;
}



void app_ctrl_setSaveCfg(CMD_EXT * pInCmd)
{
     if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;


    if(pInCmd->CmdType != pIStuts->CmdType)
        pIStuts->CmdType = pInCmd->CmdType;
	
    //if (pIStuts->AvtCfgSave != pInCmd->AvtCfgSave)
    {
        pIStuts->AvtCfgSave = pInCmd->AvtCfgSave;
	//OSA_printf("AvtCfgSave = %d,FovStat = %d\n",pIStuts->AvtCfgSave,pIStuts->FovStat);
        MSGDRIV_send(MSGID_EXT_INPUT_CFGSAVE, 0);
    }

    /***for **reply*****/
    //MSGAPI_AckSnd( ACK);

  return ;
}


void app_ctrl_SaveCollXY()
{ 
       if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

    if(pIStuts->TvCollimation == 0x01)
    {
        pIStuts->AvtMoveX = pIStuts->unitTvCollX;
        pIStuts->AvtMoveY = pIStuts->unitTvCollY;

        pIStuts->AvtCfgSave = eSave_Enable;

        //printf("save cfg\n");
        MSGDRIV_send(MSGID_EXT_INPUT_AXISPOS, 0);
        MSGDRIV_send(MSGID_EXT_INPUT_CFGSAVE, 0);
        
    }
    else if(pIStuts->FrCollimation == 0x01)
    {
#if (!FrColl_Change)
        pIStuts->AvtMoveX = pIStuts->AvtPixelX;
        pIStuts->AvtMoveY = pIStuts->AvtPixelY;
#endif      
        //OSA_printf("x:%d ****y:%d****\n",pIStuts->AvtMoveX,pIStuts->AvtMoveY);
        pIStuts->AvtCfgSave = eSave_Enable;

       // MSGDRIV_send(MSGID_EXT_INPUT_AXISPOS, 0);
        MSGDRIV_send(MSGID_EXT_INPUT_CFGSAVE, 0);
    }

   return ;
}


void app_ctrl_setDispGrade(CMD_EXT * pInCmd)
{
         if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

    if(pInCmd->CmdType != pIStuts->CmdType)
		pIStuts->CmdType = pInCmd->CmdType;
	  

    if (pIStuts->DispGrp[0] != pInCmd->DispGrp[0] 
                        || pIStuts->DispGrp[1] != pInCmd->DispGrp[1])
    {
        pIStuts->DispGrp[0] = pInCmd->DispGrp[0];
        pIStuts->DispGrp[1] = pInCmd->DispGrp[1];
        CFGID_CONFIG_SET(CFGID_OC25_TV_OSD_LEVEL,   pIStuts->DispGrp[0] );
        CFGID_CONFIG_SET(CFGID_OC25_FR_OSD_LEVEL,   pIStuts->DispGrp[1]);
    
      //  MSGDRIV_send(MSGID_EXT_INPUT_DISPGRADE, 0);
    }
	
       /***for **reply*****/
   	//MSGAPI_AckSnd( ACK);
   return ;
}


void app_ctrl_setDispColor(CMD_EXT * pInCmd )
{
  if(msgextInCtrl==NULL)
	return ;
	CMD_EXT *pIStuts = msgextInCtrl;

	if(pInCmd->CmdType != pIStuts->CmdType)
		pIStuts->CmdType = pInCmd->CmdType;
	  
if(pInCmd->DispColor[0] !=0x07)
{
	if (pIStuts->DispColor[0] != pInCmd->DispColor[0] 
                    || pIStuts->DispColor[1] != pInCmd->DispColor[1])
	{
	    pIStuts->DispColor[0] = pInCmd->DispColor[0];
	    pIStuts->DispColor[1] = pInCmd->DispColor[1]; 
	    CFGID_CONFIG_SET(CFGID_RTS_TV_SEN_COLOR,   pIStuts->DispColor[0]);

        CFGID_CONFIG_SET(CFGID_RTS_FR_SEN_COLOR,   pIStuts->DispColor[1]);
	  //  if(pIStuts->DispColor[0] < 7)
	    //MSGDRIV_send(MSGID_EXT_INPUT_DISPCOLOR, 0);
	}
}
     /***for **reply*****/
     //MSGAPI_AckSnd( ACK);
   return ;
}


void app_ctrl_setTvdispaly(CMD_EXT * pInCmd)
{
	 if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;

	//fov change need  
	/*
		TV:2400~960=Big,960~330=Mid,330~260=Sml,110=superSml,55=Zoom
		FR:4000~1000=Big,1000~330=Mid,330~120=Sml,120~90=SuperSml,60=Zoom
	*/
	if(pIStuts->SensorStat==0)
	{

		//OSA_printf("TVfov*************the unitFovAngle=%f  \n",pIStuts->unitFovAngle[pInCmd->SensorStat]);
		if(pIStuts->unitFovAngle[pInCmd->SensorStat]>960&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=2400)
		{
			//940 big fov position <- y positon
			int tvunit=(2400-960)/30;
			int tvpos=940-(2400-pIStuts->unitFovAngle[pInCmd->SensorStat])/tvunit;
			if(tvpos>940)
				tvpos=940;
			
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
				 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), tvpos);

		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>330&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=960)
		{
			//910 big fov position<- yposition
			int tvunit=(960-330)/30;
			int tvpos=910-(960-pIStuts->unitFovAngle[pInCmd->SensorStat])/tvunit;
			if(tvpos>910)
				tvpos=910;
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), tvpos);


		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>260&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=330)
		{
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 880);
		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]==110){
				 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
				 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 850);
		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]==55){
				 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
				 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 820);
		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>2400)
		{

			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 940);
		}		
	}
	else
	{
		//OSA_printf("FRfov*************the unitFovAngle=%f  \n",pIStuts->unitFovAngle[pInCmd->SensorStat]);
		if(pIStuts->unitFovAngle[pInCmd->SensorStat]>1000&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=4000)
		{
			//940 big fov position <- y positon
			int frunit=(4000-1000)/30;
			int frpos=940-(4000-pIStuts->unitFovAngle[pInCmd->SensorStat])/frunit;
			if(frpos>940)
				frpos=940;
			
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
				 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), frpos);

		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>330&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=1000)
		{
			//910 big fov position<- yposition
			int frunit=(1000-330)/30;
			int frpos=910-(1000-pIStuts->unitFovAngle[pInCmd->SensorStat])/frunit;
			if(frpos>910)
				frpos=910;
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), frpos);


		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>120&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=330)
		{
			//880 big fov position<- yposition
			int frunit=(330-120)/30;
			int frpos=880-(330-pIStuts->unitFovAngle[pInCmd->SensorStat])/frunit;
			if(frpos>880)
				frpos=880;
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), frpos);


		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>90&&pIStuts->unitFovAngle[pInCmd->SensorStat]<=120)
		{
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 850);


		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]==60){
			CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 820);

		}
		else if(pIStuts->unitFovAngle[pInCmd->SensorStat]>4000)
		{

			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_LEFT_X(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1), 150); 
			 CFGID_CONFIG_SET(CFGID_OSD_TEXT_2_TOP_Y(BLOCK_BASE_OSD_TEXT_FOVCHOOSE_1 ), 940);
		}
	}

}


void app_ctrl_setTvFovCtrl(CMD_EXT * pInCmd)
{
    if(msgextInCtrl==NULL)
		return ;
     	CMD_EXT *pIStuts = msgextInCtrl;
    float fDiffMask = 0.0;

    fDiffMask = pIStuts->unitFovAngle[pInCmd->SensorStat] - pInCmd->unitFovAngle[pInCmd->SensorStat];
   //if (fDiffMask > 0.0 || fDiffMask < 0.0)
    {
	
       pIStuts->unitFovAngle[pInCmd->SensorStat] = pInCmd->unitFovAngle[pInCmd->SensorStat];
       app_ctrl_setTvdispaly(pInCmd);
       MSGDRIV_send(MSGID_EXT_INPUT_FOVSTAT, 0);
	//printf("the unitFovAngle=%f\n",pIStuts->unitFovAngle[pInCmd->SensorStat]);
     }
//printf("the unitFovAngle=%f  fDiffMask=%f\n",pIStuts->unitFovAngle[pInCmd->SensorStat],fDiffMask);
   return ;
}



void app_ctrl_setPicp(CMD_EXT * pInCmd)
{
  if(msgextInCtrl==NULL)
		return ;
  CMD_EXT *pIStuts = msgextInCtrl;

  if(pInCmd->CmdType != pIStuts->CmdType)
		pIStuts->CmdType = pInCmd->CmdType;
	
     if((pInCmd->ImgPicp[pInCmd->SensorStat] != pIStuts->ImgPicp[pInCmd->SensorStat])&&pInCmd->FrCollimation!=1)
     {
	 	pIStuts->ImgPicp[pInCmd->SensorStat] = pInCmd->ImgPicp[pInCmd->SensorStat];
		
		MSGDRIV_send(MSGID_EXT_INPUT_PICPCROP, 0);
		//MSGAPI_AckSnd(AckPicp);
		OSA_printf("[%d]  %s  ",__LINE__,__func__);
     }
     else if(pInCmd->ImgPicp[pInCmd->SensorStat]==3)
	 	{
			MSGDRIV_send(MSGID_EXT_INPUT_PICPCROP, 0);
			//MSGAPI_AckSnd(AckPicp);
			OSA_printf("[%d]  %s  ",__LINE__,__func__);
	 	}
	else
		{
			//MSGAPI_AckSnd(AckPicp);
		}
   return ;
}


void app_ctrl_setverti(CMD_EXT * pInCmd)
{
  if(msgextInCtrl==NULL)
		return ;
  CMD_EXT *pIStuts = msgextInCtrl;

  if(pInCmd->TrkPanev != pIStuts->TrkPanev)
		pIStuts->TrkPanev = pInCmd->TrkPanev;
   if(pInCmd->TrkTitlev != pIStuts->TrkTitlev)
		pIStuts->TrkTitlev = pInCmd->TrkTitlev;  
   return ;
}


void app_ctrl_detectvideo()
{
	
	 MSGDRIV_send(MSGID_EXT_INPUT_VIDEOEN, 0);
	return ;

}

void app_ctrl_ack()
{
	//MSGAPI_AckSnd( AckFrColl);
}

void app_err_feedbak()
{
	//MSGAPI_AckSnd( AckTrkErr);
}
