/*
 * MultiChVideo.cpp
 *
 *  Created on:
 *      Author: sh
 */

#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "MultiChVideo.hpp"
#include "v4l2camera.hpp"
#include "gpio_rdwr.h"
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

MultiChVideo::MultiChVideo():m_usrFunc(NULL),m_user(NULL)
{
	memset(m_thrCap, 0, sizeof(m_thrCap));
	memset(VCap, 0, MAX_CHAN*sizeof(void*));
}

MultiChVideo::~MultiChVideo()
{
	destroy();
}
static void pabort(const char *s)
{
	perror(s);
	abort();
}
static volatile int front = 1;
static int fd = 0;
static uint8_t tx1[] = {0x00, 0x70, 0x11, 0x04};
static uint8_t tx2[] = {0x00, 0x70, 0x0F, 0x04};
static uint8_t tx3[] = {0x00, 0x70, 0x21, 0x04};
static uint8_t tx4[] = {0x00, 0x70, 0x0F, 0x04};
	//uint8_t rx[ARRAY_SIZE(tx)] = {0, };
static struct spi_ioc_transfer tr1 = {
	tr1.tx_buf = (unsigned long)tx1,
	tr1.rx_buf = 0,//(unsigned long)rx,
	tr1.len = 4,
	tr1.delay_usecs = 0,
	tr1.speed_hz = 40000000,
	tr1.bits_per_word = 32,
};
static struct spi_ioc_transfer tr2 = {
	tr2.tx_buf = (unsigned long)tx2,
	tr2.rx_buf = 0,//(unsigned long)rx,
	tr2.len = 4,
	tr2.delay_usecs = 0,
	tr2.speed_hz = 40000000,
	tr2.bits_per_word = 32,
};
static struct spi_ioc_transfer tr3 = {
	tr2.tx_buf = (unsigned long)tx3,
	tr2.rx_buf = 0,//(unsigned long)rx,
	tr2.len = 4,
	tr2.delay_usecs = 0,
	tr2.speed_hz = 40000000,
	tr2.bits_per_word = 32,
};
static struct spi_ioc_transfer tr4 = {
	tr2.tx_buf = (unsigned long)tx4,
	tr2.rx_buf = 0,//(unsigned long)rx,
	tr2.len = 4,
	tr2.delay_usecs = 0,
	tr2.speed_hz = 40000000,
	tr2.bits_per_word = 32,
};

int MultiChVideo::creat()
{
	/*for(int i=0; i<MAX_CHAN; i++){
		VCap[i] = new v4l2_camera(3);
		VCap[i]->creat();
	}*/
	VCap[0] = new v4l2_camera(0);
	VCap[0]->creat();
	//VCap[1] = new v4l2_camera(1);
	//VCap[1]->creat();
//	VCap[2] = new v4l2_camera(3);
//	VCap[2]->creat();
//	VCap[3] = new v4l2_camera(4);
//	VCap[3]->creat();

	//int status;
	//ccd detect
	//status = GPIO_create(456,0);
	//fir detect
	//GPIO_create(333,0);

	//printf("gpio 456  = %d\n",status);
	//int ccdGpio = GPIO_get(456);
	//printf("***************ccdgpio = %d\n",ccdGpio);
	int ret = 0;

	static uint8_t mode = 0;
	static uint8_t bits = 32;
	static uint32_t speed = 40000000;
	static uint16_t delay;
#if 0
	fd = open("/dev/spidev3.0",O_RDWR);
	if(fd < 0)
		pabort("can't open device");

	//spi mode
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if(ret == -1)
		pabort("can't set spi mode");

	//bits per word
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if(ret == -1)
		pabort("can't set bits per word");

	//max speed hz
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret == -1)
		pabort("can't set max speed hz");

	//ioctl(fd,SPI_IOC_MESSAGE(1),&tr1);
#endif	
	return 0;
}

int MultiChVideo::destroy()
{
	for(int i=0; i<MAX_CHAN; i++){
		if(VCap[i] != NULL)
		{
			stop();
			VCap[i]->stop();
			delete VCap[i];
			VCap[i] = NULL;
		}
	}
	return 0;
}

int MultiChVideo::run()
{
	int iRet = 0;

	for(int i=0; i<MAX_CHAN; i++){
		VCap[i]->run();
		m_thrCxt[i].pUser = this;
		m_thrCxt[i].chId = i;
		iRet = OSA_thrCreate(&m_thrCap[i], capThreadFunc, 0, 0, &m_thrCxt[i]);
	}

	return iRet;
}

int MultiChVideo::stop()
{
	for(int i=0; i<MAX_CHAN; i++){
		VCap[i]->stop();
		OSA_thrDelete(&m_thrCap[i]);
	}

	return 0;
}

int MultiChVideo::run(int chId)
{
	int iRet;
	if(chId<0 || chId>=MAX_CHAN)
		return -1;

	VCap[chId]->run();
	m_thrCxt[chId].pUser = this;
	m_thrCxt[chId].chId = chId;
	iRet = OSA_thrCreate(&m_thrCap[chId], capThreadFunc, 0, 0, &m_thrCxt[chId]);

	return iRet;
}

int MultiChVideo::stop(int chId)
{
	if(chId<0 || chId>=MAX_CHAN)
		return -1;

	OSA_thrDelete(&m_thrCap[chId]);
	VCap[chId]->stop();

	return 0;
}

void MultiChVideo::process()
{
	int chId;
	fd_set fds;
	struct timeval tv;
	int ret;

	Mat frame;

	FD_ZERO(&fds);

	for(chId=0; chId<MAX_CHAN; chId++){
		if(VCap[chId]->m_devFd != -1 &&VCap[chId]->bRun )
			FD_SET(VCap[chId]->m_devFd, &fds);
		//OSA_printf("MultiChVideo::process: FD_SET ch%d -- fd %d", chId, VCap[chId]->m_devFd);
	}

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	ret = select(FD_SETSIZE, &fds, NULL, NULL, &tv);

	//OSA_printf("MultiChVideo::process: ...");

	if(-1 == ret)
	{
		if (EINTR == errno)
			return;
	}else if(0 == ret)
	{
		return;
	}

	for(chId=0; chId<MAX_CHAN; chId++){
		if(VCap[chId]->m_devFd != -1 && FD_ISSET(VCap[chId]->m_devFd, &fds)){
			struct v4l2_buffer buf;
			memset(&buf, 0, sizeof(buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory =V4L2_MEMORY_USERPTR;//V4L2_MEMORY_MMAP;//

			if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_DQBUF, &buf))
			{
				fprintf(stderr, "cap ch%d DQBUF Error!\n", chId);
			}
			else
			{
				
				if(m_usrFunc != NULL){
					frame = cv::Mat(VCap[chId]->imgheight, VCap[chId]->imgwidth, VCap[chId]->imgtype,
							VCap[chId]->buffers[buf.index].start);

					m_usrFunc(m_user, chId, frame);
				}
				
				if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_QBUF, &buf)){
					fprintf(stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
		}
	}

}

static int frameflag = 0;
void MultiChVideo::process(int chId)
{
	fd_set fds;
	struct timeval tv;
	int ret;
	//int ccdGpio;
	//GPIO_create(456,0);
	Mat frame;
	enum v4l2_buf_type type;

	FD_ZERO(&fds);

	FD_SET(VCap[chId]->m_devFd, &fds);

	tv.tv_sec = 1;
	tv.tv_usec = 0;//250000

	ret = select(VCap[chId]->m_devFd+1, &fds, NULL, NULL, &tv);

	//OSA_printf("MultiChVideo::process: ...");

	if(-1 == ret)
	{
		if (EINTR == errno)
			return;
	}else if(0 == ret)
	{
		if(((1 == GPIO_get(391))&&(chId==0))||((1 == GPIO_get(390))&&(chId==1)))
		{

			//OSA_printf("11****************TV = %d,  FR = %d\n",GPIO_get(391),GPIO_get(390));
			//OSA_printf("MultiChVideo::process: ... ret = %d\n",ret);
			VCap[chId]->destroy();
			//printf("***************destroytime = %d\n",OSA_getCurTimeInMsec()-destroytime);
			//int creattime = OSA_getCurTimeInMsec();
			VCap[chId]->creat();
			//printf("***************creattime = %d\n",OSA_getCurTimeInMsec()-creattime);
			//int runtime = OSA_getCurTimeInMsec();
			VCap[chId]->run();		
			//OSA_waitMsecs(2000);
			return;
		}
	}

	//ccdGpio = GPIO_get(456);
	//printf("***************ccdgpio = %d\n",ccdGpio);
#if 0
	if((1 == ccdGpio)&&(chId == 0))
	{

		ret = ioctl(fd,SPI_IOC_MESSAGE(1),&tr2);	
		ret = ioctl(fd,SPI_IOC_MESSAGE(1),&tr1);
		
		int time = OSA_getCurTimeInMsec();
		

		
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_STREAMOFF, &type);
		VCap[chId]->start_capturing_2();
		v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_STREAMON, &type);
		printf("***************time = %d\n",OSA_getCurTimeInMsec()-time);
		
		resetvideo = 1;

	}
	//front = ccdGpio;
#endif

	if(VCap[chId]->m_devFd != -1 && FD_ISSET(VCap[chId]->m_devFd, &fds))
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;//V4L2_MEMORY_MMAP


		if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_DQBUF, &buf))
		{
			//fprintf(stderr, "cap ch%d DQBUF Error! WH = %d,%d\n", chId,VCap[chId]->imgheight, VCap[chId]->imgwidth);

			#if 0
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_STREAMOFF, &type)){
				fprintf(stderr, "CAPTURE VIDEO error %d, %s\n", errno, strerror(errno));
				exit(EXIT_FAILURE);
			}

			if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_STREAMON, &type)){
				fprintf(stderr, "CAPTURE VIDEO error %d, %s\n", errno, strerror(errno));
				exit(EXIT_FAILURE);
			}
 			#endif
			#if 1
			if(chId == 0)
			{
				//OSA_waitMsecs(500);
				if(1 == GPIO_get(391))
				{
					//OSA_printf("22****************TV = %d,  FR = %d\n",GPIO_get(391),GPIO_get(390));
					//int destroytime = OSA_getCurTimeInMsec();
					VCap[chId]->destroy();
					//printf("***************destroytime = %d\n",OSA_getCurTimeInMsec()-destroytime);
					//int creattime = OSA_getCurTimeInMsec();
					VCap[chId]->creat();
					//printf("***************creattime = %d\n",OSA_getCurTimeInMsec()-creattime);
					//int runtime = OSA_getCurTimeInMsec();
					VCap[chId]->run();
					//printf("***************runtime = %d\n",OSA_getCurTimeInMsec()-runtime);
				}
			}
			#endif
			if((chId == 1) && (1 == GPIO_get(390)))
			{
				//OSA_printf("33****************TV = %d,  FR = %d\n",GPIO_get(391),GPIO_get(390));
				VCap[chId]->destroy();
				VCap[chId]->creat();
				VCap[chId]->run();
			}
			
			
			//exit(0);
		}
		else
		{
			if(m_usrFunc != NULL){
				frame = cv::Mat(VCap[chId]->imgheight, VCap[chId]->imgwidth, VCap[chId]->imgtype,
						VCap[chId]->buffers[buf.index].start);

				//if(((frameflag++) % 100) == 0)
					//printf("***************video WH (%d,%d)\n",VCap[0]->imgheight, VCap[0]->imgwidth);
				//static unsigned int dupframe=0;
				//if((dupframe++)%2==0)
				m_usrFunc(m_user, chId, frame);
			}
			if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_QBUF, &buf)){
				fprintf(stderr, "VIDIOC_QBUF error %d, %s\n", errno, strerror(errno));
				//exit(EXIT_FAILURE);
			}
		}
		
	}
}



