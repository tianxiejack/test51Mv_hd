
#ifndef APP_CTRL_H_
#define APP_CTRL_H_

#include "app_status.h"
extern CMD_EXT *msgextInCtrl;

void app_ctrl_freezeReset(CMD_EXT * pInCmd);
void app_ctrl_setReset(CMD_EXT * pInCmd);
void app_ctrl_setSensor(CMD_EXT * pInCmd);
void app_ctrl_setTrkStat(CMD_EXT * pInCmd);
void app_ctrl_setMMT(CMD_EXT * pInCmd);
void app_ctrl_setForceCoast(CMD_EXT * pInCmd);
void app_ctrl_setTrkBomen(CMD_EXT * pInCmd);
void app_ctrl_setAimPos(CMD_EXT * pInCmd);
void app_ctrl_setZoom(CMD_EXT * pInCmd);
void app_ctrl_setFRColl(CMD_EXT * pInCmd);
void app_ctrl_setTvColl(CMD_EXT * pInCmd);

void app_ctrl_setAimSize(CMD_EXT * pInCmd);
void app_ctrl_setFovCtrl(CMD_EXT * pInCmd);
void app_ctrl_setSerTrk(CMD_EXT * pInCmd );

void app_ctrl_setSysmode(CMD_EXT * pInCmd);
unsigned char app_ctrl_getSysmode();


void app_ctrl_setSaveCfg(CMD_EXT * pInCmd);
void app_ctrl_setFovselect(CMD_EXT * pInCmd);
void app_ctrl_SaveCollXY();

void app_ctrl_setDispGrade(CMD_EXT * pInCmd);

void app_ctrl_setDispColor(CMD_EXT * pInCmd );
void app_ctrl_setAxisPos(CMD_EXT * pInCmd);
void app_ctrl_setTvFovCtrl(CMD_EXT * pInCmd);

void app_ctrl_setEnhance(CMD_EXT * pInCmd);
void app_ctrl_setPicp(CMD_EXT * pInCmd);
void app_ctrl_setverti(CMD_EXT * pInCmd);
void app_ctrl_poweron(CMD_EXT * pInCmd );


void app_ctrl_detectvideo();
void app_ctrl_ack();
void app_err_feedbak();

void app_ctrl_setMtdStat(CMD_EXT * pInCmd);
unsigned char app_ctrl_getMtdStat();

void  app_ctrl_getSysData(CMD_EXT * exthandle);
void app_ctrl_setMmtSelect(CMD_EXT * pIStuts,unsigned char index);

void app_ctrl_setMtdStat(CMD_EXT * pInCmd);
void app_ctrl_setAcqRect(CMD_EXT * pInCmd);

#endif /* APP_CTRL_H_ */
