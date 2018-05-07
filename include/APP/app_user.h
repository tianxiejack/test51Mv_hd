
#ifndef __USER_APP_H__
#define __USER_APP_H__
#define MAX_INPUT_STR_SIZE 		( 128 )

#define MCFW_VCAP_VDIS_CASE 	        ( 0 )
#define MCFW_VCAP_VENC_CASE 	        ( 1 )
#define MCFW_IMAGE_PROCESS_BOARD_CASE   ( 2 )


#define SYS_INFOR_SHOW_TIMER			(0x00)
#define SYS_INFOR_SHOW_TIMER_TICKS	(1000)

#define OSD_SHOW_TIMER			    (0x01)
#define OSD_SHOW_TIMER_TICKS		    (40)//ms


//#define ALG_MTD_TIMER			        (0x02)
//#define ALG_MTD_TIMER_TICKS		    (40)//ms

//#define ALG_TRACK_TIMER			    (0x03)
//#define ALG_TRACK_TIMER_TICKS			(40)//ms
#define GPIO_INSPECT_TIMER                    (0x06)
#define GPIO_INSPECT_TICKS		    (300) //ms


#define GRPX_SHOW_TIMER                    (0x04)
#define GRPX_SHOW_TIME_TICKS		    (80) //ms

#define ALG_TRACK_TIMER                    (0x05)
#define ALG_TRACK_TIMER_TICKS		    (40) //ms

#define ALG_TRACK_TIMER_ERRFEEDBACK                   (0x07)
#define ALG_TRACK_TIMER_ERRFEEDBACK_TICKS		    (15) //ms

char APP_getChar( );
int cltrl_dis_user(int value,int id);
unsigned char APP_getint( );
void initgdxd_info();
void APP_setPrms( int nType );
#endif

