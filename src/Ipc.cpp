#include "Ipcctl.h"
#include <string.h>
#include "osa_thr.h"
#include "osa_buf.h"
#include "app_status.h"
#include"app_ctrl.h"

#define DATAIN_TSK_PRI              (2)
#define DATAIN_TSK_STACK_SIZE       (0)
#define SDK_MEM_MALLOC(size)                                            OSA_memAlloc(size)

int ipc_loop = 1;

extern void inputtmp(unsigned char cmdid);

OSA_BufCreate msgSendBufCreate;
OSA_BufHndl msgSendBuf;

OSA_ThrHndl thrHandleDataIn_recv;
OSA_ThrHndl thrHandleDataIn_send;

void initmessage()
{
    int status;
    int i=0;
    msgSendBufCreate.numBuf = OSA_BUF_NUM_MAX;
    for (i = 0; i < msgSendBufCreate.numBuf; i++)
    {
        msgSendBufCreate.bufVirtAddr[i] = SDK_MEM_MALLOC(64);
        OSA_assert(msgSendBufCreate.bufVirtAddr[i] != NULL);
        memset(msgSendBufCreate.bufVirtAddr[i], 0, 64);
    }
    OSA_bufCreate(&msgSendBuf, &msgSendBufCreate);
 

}

void MSGAPI_msgsend(int cmdID)
{
   int bufId = 0;
   unsigned char *quebuf;

   OSA_bufGetEmpty(&(msgSendBuf), &bufId, OSA_TIMEOUT_NONE);
   quebuf=(unsigned char *)msgSendBuf.bufInfo[bufId].virtAddr;

   //msgSendBuf.bufInfo[bufId].size = 7;
   quebuf[0]=cmdID & 0xFF;	

   OSA_bufPutFull(&(msgSendBuf), bufId);
}

int  send_msgpth(SENDST * RS422)
{
	memset(RS422,0,sizeof(char)*(PARAMLEN+1));
	int bufId=0;
	int sendLen=0;
      OSA_bufGetFull(&msgSendBuf, &bufId, OSA_TIMEOUT_FOREVER);
      memcpy(RS422, msgSendBuf.bufInfo[bufId].virtAddr,sizeof(SENDST));
      OSA_bufPutEmpty(&msgSendBuf, bufId);
      send_msg(RS422);
	return 0;
}

void* recv_msg(SENDST *RS422)
{
	unsigned char cmdID = 0;
	unsigned char imgID1 = 0;
	unsigned char imgID2 = 0;
	unsigned char imgID3 = 0;
	unsigned char imgID4 = 0;
	unsigned char imgID5 = 0;	

	CMD_SENSOR Rsensor;
	CMD_PinP Rpinp;
	CMD_TRK Rtrk;
	CMD_SECTRK Rsectrk;
	CMD_ENH Renh;
	CMD_MTD Rmtd;
	CMD_MMTSELECT Rmmtselect;
	CMD_TRKDOOR Rtrkdoor;
	CMD_SYNC422 Rsync422;
	CMD_POSMOVE Rposmove;
	CMD_POSMOVE Raxismove;
	CMD_ZOOM Rzoom;

	if(RS422==NULL)
	{
		return NULL ;
	}

	cmdID	=  RS422->cmd_ID;
	imgID1	=  RS422->param[0];
	imgID2	=  RS422->param[1];
	imgID3	=  RS422->param[2];
	imgID4	=  RS422->param[3];
	imgID5 	=  RS422->param[4];

	CMD_EXT inCtrl, *pMsg = NULL;
	pMsg = &inCtrl;
	memset(pMsg,0,sizeof(CMD_EXT));
	app_ctrl_getSysData(pMsg);


	
	//printf("recv++++++++ cmdID = %02x,imgID(%02x,%02x,%02x,%02x,%02x)\n",cmdID,imgID1,imgID2,imgID3,imgID4,imgID5);
	switch(cmdID)
	{	
		case trk:	
			//inputtmp('c');
			//MSGAPI_msgsend(trk);
			//break;
			
			memcpy(&Rtrk,RS422->param,sizeof(Rtrk));
			imgID1 = Rtrk.AvtTrkStat;
		
			if(imgID1 == 0x01)
			    pMsg->TrkCmd = 0x01;
			else
			    pMsg->TrkCmd = 0x02;

			//if(pMsg->ImgMtdStat[pMsg->SensorStat] == eImgAlg_Disable)
			{
				if(pMsg->AvtTrkStat != eTrk_mode_mtd)
				{
					if(imgID1 == 0x1)
						pMsg->AvtTrkStat =eTrk_mode_target;
					else if(imgID1 == 0x2)		             
						pMsg->AvtTrkStat =eTrk_mode_mtd;
					else if(imgID1 == 0x3)
						pMsg->AvtTrkStat =eTrk_mode_sectrk;
				}
			}

			app_ctrl_setTrkStat(pMsg); 
			MSGAPI_msgsend(trk);
			
			break;
			
			memcpy(&Rtrk,RS422->param,sizeof(Rtrk));
			imgID1 = Rtrk.AvtTrkStat;	
			
			if(imgID1 == 0x01)
			    pMsg->TrkCmd = 0x01;
			else
			    pMsg->TrkCmd = 0x02;
	             
			if(pMsg->ImgMtdStat[pMsg->SensorStat] == eImgAlg_Disable)
			{
				if(pMsg->AvtTrkStat != eTrk_mode_mtd)
				{
				    if(imgID1 == 0x1)
				        pMsg->AvtTrkStat=eTrk_mode_acq;
				    else if(imgID1 == 0x2)		             
				   		 pMsg->AvtTrkStat=eTrk_mode_target;
				}
				else if(pMsg->AvtTrkStat == eTrk_mode_mtd)
				{
				    if(imgID1 == 0x01)
				        pMsg->AvtTrkStat = eTrk_mode_acq;
				}
			}
			else if(pMsg->ImgMtdStat[pMsg->SensorStat] == eImgAlg_Enable)
			{
				if((pMsg->AvtTrkStat == eTrk_mode_acq) || (pMsg->AvtTrkStat == eTrk_mode_target) || (pMsg->AvtTrkStat == eTrk_mode_mtd))
				{
				    if(imgID1 == 0x01)
				        pMsg->AvtTrkStat = eTrk_mode_acq;         			 
				    else if(imgID1 == 0x02)
				    {
				       if((pMsg->AvtTrkStat == eTrk_mode_acq) || (pMsg->AvtTrkStat == eTrk_mode_target))
				            pMsg->AvtTrkStat = eTrk_mode_target;	       
				    }
				    else if(imgID1 == 0x03)
				    {
				        if(pMsg->AvtTrkStat == eTrk_mode_acq)
				                    pMsg->AvtTrkStat = eTrk_mode_mtd;
				    }
				}
			}
						
			if(imgID1 == 0x04 && pMsg->SecAcqStat)
			{
				pMsg->AvtTrkStat = eTrk_mode_search;
			}
			app_ctrl_setTrkStat(pMsg);  
			MSGAPI_msgsend(trk);
			break;	
			
		case mmt:
			memcpy(&Rmtd,RS422->param,sizeof(Rmtd));
			imgID1 = Rmtd.ImgMtdStat;				
			if(imgID1 ==0x1)
			{
				pMsg->MMTTempStat = 0x01;//two channel mtd open all
				pMsg->ImgMtdStat[pMsg->SensorStat] = eImgAlg_Enable;
				pMsg->ImgMtdStat[(pMsg->SensorStat + 1) % 2] = eImgAlg_Enable;
			}
			else if(imgID1 ==0x2)
			{
				pMsg->MMTTempStat = 0x02;
				pMsg->ImgMtdStat[pMsg->SensorStat] = eImgAlg_Disable;
				pMsg->ImgMtdStat[(pMsg->SensorStat + 1) % 2] = eImgAlg_Disable;
			}
			else if(imgID1 ==0x3)
			{
				pMsg->MMTTempStat= 0x03;
				if (pMsg->ImgMtdStat[pMsg->SensorStat] ==  eImgAlg_Enable)
				{
					pMsg->ImgMtdSelect[pMsg->SensorStat]  =  eMMT_Next;  //mtd num to next
					app_ctrl_setMmtSelect(pMsg);
					//MSGAPI_AckSnd( AckMtdStat);
					break;
				}
				else
					pMsg->MMTTempStat = 0x02;
			}
			else if(imgID1 ==0x4)
			{
				pMsg->MMTTempStat= 0x04;
				if (pMsg->ImgMtdStat[pMsg->SensorStat] ==  eImgAlg_Enable)
				{
					pMsg->ImgMtdSelect[pMsg->SensorStat]  =  eMMT_Prev;  //mtd num to next
					app_ctrl_setMmtSelect(pMsg);
					//MSGAPI_AckSnd(  AckMtdStat);
					break;
				}
				else
					pMsg->MMTTempStat = 0x02;
			}
			else
			{
				if(pMsg->ImgMtdStat[pMsg->SensorStat] ==  eImgAlg_Enable)
					pMsg->MMTTempStat = 0x01;
				else
					pMsg->MMTTempStat = 0x02;  
			}
			app_ctrl_setMMT(pMsg);
			MSGAPI_msgsend(mmt);
			break;
		case enh:
			memcpy(&Renh,RS422->param,sizeof(Renh));
			imgID1 = Renh.ImgEnhStat;
			
			if(imgID1 == 1)
			{
				pMsg->ImgEnhStat[0] = ipc_eImgAlg_Enable;

			}
			else if(imgID1 == 0)
			{
				pMsg->ImgEnhStat[0] = ipc_eImgAlg_Disable;
			}
			
			app_ctrl_setEnhance(pMsg);
			MSGAPI_msgsend(enh);
			break;		
		case trkdoor:		
			memcpy(&Rtrkdoor,RS422->param,sizeof(Rtrkdoor));
			imgID1 = Rtrkdoor.TrkBomenCtrl;
			imgID2 = Rtrkdoor.AvtTrkAimSize;
			printf("jet +++++AvtTrkAimSize = %d+++++\n",imgID2);
			
			pMsg->TrkBomenCtrl = imgID1;
			pMsg->AvtTrkAimSize = imgID2;
			app_ctrl_setTrkBomen(pMsg); // bomen show or hide
			app_ctrl_setAimSize(pMsg);
			MSGAPI_msgsend(trkdoor);
			break;		
		case posmove:	
			memcpy(&Rposmove,RS422->param,sizeof(Rposmove));
			imgID1 = Rposmove.AvtMoveX;
			imgID2 = Rposmove.AvtMoveY;
		
			OSA_printf("+++++AvtMoveXY(%d,%d)\n",imgID1,imgID2);
			pMsg->AvtMoveX = imgID1;
			pMsg->AvtMoveY = imgID2;
			app_ctrl_setAimPos(pMsg);
			MSGAPI_msgsend(posmove);
			break;					
		case exit_img:
			MSGAPI_msgsend(exit_img);
			ipc_loop = 0;			
			break;
		default:
			break;
		}

}

int send_msg(SENDST *RS422)
{
	unsigned char cmdID = 0;
	unsigned char imgID1 = 0;
	unsigned char imgID2 = 0;
	unsigned char imgID3 = 0;
	unsigned char imgID4 = 0;
	unsigned char imgID5 = 0;	
	CMD_EXT pIStuts;
	
	memcpy(&pIStuts,msgextInCtrl,sizeof(CMD_EXT));

	if(RS422==NULL)
	{
		return  -1 ;
	}
	cmdID = RS422->cmd_ID;
	switch (cmdID)
	{
		case trk:
			RS422->param[0] = pIStuts.AvtTrkStat;
			printf("send ++++++++++ AvtTrkStat %02x  ++++++++++\n",RS422->param[0]);			
			break;				
		case mmt:
			RS422->param[0] = pIStuts.ImgMtdStat[ipc_eSen_TV];	
			printf("send ++++++++++ ImgMtdStat[tv] %02x  ++++++++++\n",RS422->param[0]);					
			break;
		case enh:
			RS422->param[0] = pIStuts.ImgEnhStat[ipc_eSen_TV];			
			printf("send ++++++++++ ImgEnhStat[tv] %02x  ++++++++++\n",RS422->param[0]);					
			break;
		case trkdoor:
			RS422->param[1] = pIStuts.AvtTrkAimSize;			
			printf("send ++++++++++ TrkBomenCtrl %02x  AvtTrkAimSize %02x ++++++++++\n",RS422->param[0],RS422->param[1]);					
			break;		
		case posmove:
			switch(pIStuts.AvtMoveX)
			{
				case 0:
					RS422->param[0] = 0;
					break;
				case 1:
					RS422->param[0] = 2;
					break;
				case -1:
					RS422->param[0] = 1;
					break;
				default:
					break;		
			}
			switch(pIStuts.AvtMoveY)
			{
				case 0:
					RS422->param[1] = 0;
					break;
				case 1:
					RS422->param[1] = 2;
					break;
				case -1:
					RS422->param[1] = 1;
					break;
				default:
					break;		
			}
			printf("send ++++++++++ AvtMoveXY = (%02x,%02x)  ++++++++++\n",RS422->param[0],RS422->param[1]);				
			break;
		case exit_img:					
			break;
		default:
			break;
	}
	return 0;
}

static void * ipc_dataRecv(Void * prm)
{
	SENDST test;
	while(ipc_loop)
	{
		ipc_recvmsg(&test,IPC_TOIMG_MSG);
		recv_msg(&test);			
	}
}


static void * ipc_dataSend(Void * prm)
{
	SENDST test;
	while(ipc_loop)
	{	
		send_msgpth(&test);
		ipc_sendmsg(&test,IPC_FRIMG_MSG);
	}
}

void Ipc_pthread_start(void)
{
	Ipc_init();
	Ipc_create();

	initmessage();
	OSA_thrCreate(&thrHandleDataIn_recv,
                      ipc_dataRecv,
                      DATAIN_TSK_PRI,
                      DATAIN_TSK_STACK_SIZE,
                      NULL);
       OSA_thrCreate(&thrHandleDataIn_send,
                      ipc_dataSend,
                      DATAIN_TSK_PRI,
                      DATAIN_TSK_STACK_SIZE,
                      NULL);
}

void Ipc_pthread_stop(void)
{
	OSA_thrDelete(&thrHandleDataIn_recv);
	OSA_thrDelete(&thrHandleDataIn_send);
}
