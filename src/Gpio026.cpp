#if 0
#include "Gpio026.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "gpio_rdwr.h"
#include "osa.h"

#define GPIO_TX2 1

int hard_flag=0;


void Hard_Init()
{
	GPIO_init();
	//Hard_capturereset();
	Hard_displayreset();
	Hard_displayreset1();//for fpga tx2 gpio426 xu han
	Hard_Gpiocreate();
	hard_flag=1;
	
}


void Hard_displayreset()
{
	unsigned int disp_gpio;//tx1 152
	if(GPIO_TX2)
	{
		disp_gpio = 426;
	}
	else
	{
		disp_gpio = 152;
	}

	//gpio t02

	GPIO_create(disp_gpio,GPIO_DIRECTION_OUT);
	
	GPIO_set(disp_gpio,0);
	OSA_waitMsecs(100);
	GPIO_set(disp_gpio,1);

	GPIO_close(disp_gpio);		

}

void Hard_displayreset1()
{
	unsigned int disp_gpio1;//tx1 152
	if(GPIO_TX2)
	{
		disp_gpio1 = 277;
	}
	else
	{
		disp_gpio1 = 152;
	}

	//gpio t02

	GPIO_create(disp_gpio1,GPIO_DIRECTION_OUT);
	
	GPIO_set(disp_gpio1,0);
	OSA_waitMsecs(100);
	GPIO_set(disp_gpio1,1);

	GPIO_close(disp_gpio1);		

}

void Hard_capturereset()
{
	unsigned int cap_gpio;//tx1 150

	if(GPIO_TX2)
	{
		cap_gpio = 478;
	}
	else
	{
		cap_gpio = 150;
	}

	//gpio s06

	GPIO_create(cap_gpio,GPIO_DIRECTION_OUT);
	GPIO_set(cap_gpio,0);
	OSA_waitMsecs(100);
	GPIO_set(cap_gpio,1);
	OSA_waitMsecs(100);
	GPIO_set(cap_gpio,0);
	OSA_waitMsecs(1200);
	GPIO_set(cap_gpio,1);

	GPIO_close(cap_gpio);

}
//wj
void Hard_Gpiocreate()
{
	unsigned int getccd_gpio;//tx1 84
	unsigned int getfir_gpio;//tx1 86

	if(GPIO_TX2)
	{
		getccd_gpio = 391;
		getfir_gpio = 390;
	}
	else
	{
		getccd_gpio = 84;
		getfir_gpio = 86;
	}

	//ccd detect
	GPIO_create(getccd_gpio,GPIO_DIRECTION_IN);
	//fir detect
	GPIO_create(getfir_gpio,GPIO_DIRECTION_IN);

}

int Hard_getccdstatus()
{
	unsigned int getccd_gpio;//tx1 84

	if(GPIO_TX2)
	{
		getccd_gpio = 391;
	}
	else
	{
		getccd_gpio = 84;
	}
	//ccd
	if(hard_flag==0)
		return 1;
	
	return GPIO_get(getccd_gpio);

}

int Hard_getfirstatus()
{
	unsigned int getfir_gpio;//tx1 86

	if(GPIO_TX2)
	{
		getfir_gpio = 390;
	}
	else
	{
		getfir_gpio = 86;
	}
	//fir
	if(hard_flag==0)
		return 1;


	return GPIO_get(getfir_gpio);
	
}

#endif
