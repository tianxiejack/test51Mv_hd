
#include "app_hardctrl.h"
#include "Gpio026.h"
#include"app_ctrl.h"
#include "osa.h"
//#include "msgDriv.h"

#define APP_BIT_SET(val,bit) (val|(1<<bit))
#define APP_BIT_CLR(val,bit) (val&(~(1<<bit)))

int videoenable=0;

int ccd_pristatus=0;
unsigned int ccd_count=0;
int fir_pristatus=0;
unsigned int fir_count=0;
unsigned int firpowercount=0;
void APP_getvideostatus()
{
	
	int status=0;
	if(msgextInCtrl==NULL)
		return ;
	int poweron=msgextInCtrl->Firpoweron;
	if(poweron==2)
		firpowercount=30;
	else
		{
		if(firpowercount>0)
			firpowercount--;
		}
	
	status=Hard_getccdstatus();
	//printf("ccd =%d \n",status^1);
	if(status==0||status==1)
		{

			//msgextInCtrl->unitFaultStat=APP_BIT_SET(msgextInCtrl->unitFaultStat,0,status^1);
			if(ccd_pristatus!=status)
			{
				ccd_pristatus=status;
				ccd_count=0;
			}
			if(status==0)
				{
					ccd_count++;
					if(ccd_count>=5)
					msgextInCtrl->unitFaultStat=APP_BIT_SET(msgextInCtrl->unitFaultStat,0);
				}
			else
					msgextInCtrl->unitFaultStat=APP_BIT_CLR(msgextInCtrl->unitFaultStat,0);
			videoenable++;
			//printf("unitFaultStat =%d \n",msgextInCtrl->unitFaultStat);
		}
	
	status=Hard_getfirstatus();
	//printf("fir =%d \n",status^1);
	if(status==0||status==1)
		{

			//msgextInCtrl->unitFaultStat=APP_BIT_SET(msgextInCtrl->unitFaultStat,1,status^1);
				if(fir_pristatus!=status)
				{
					fir_pristatus=status;
					fir_count=0;
				}
			if(status==0)
				{
				fir_count++;
					
					if(poweron!=2&&firpowercount==0)
						{
							if(fir_count>=10)
								{
									msgextInCtrl->unitFaultStat=APP_BIT_SET(msgextInCtrl->unitFaultStat,1);
								}
						}
					else
						{
							fir_count=0;
							fir_pristatus=1;
							msgextInCtrl->unitFaultStat=APP_BIT_CLR(msgextInCtrl->unitFaultStat,1);
						}
						
				}
			else
					msgextInCtrl->unitFaultStat=APP_BIT_CLR(msgextInCtrl->unitFaultStat,1);
			videoenable++;
			//printf("unitFaultStat =%d \n",msgextInCtrl->unitFaultStat);
		}

	
	if(videoenable>0)
	{
		//app_ctrl_detectvideo();
		videoenable=0;
	}
	// MSGDRIV_send(MSGID_EXT_INPUT_VIDEOEN, 0);

}
