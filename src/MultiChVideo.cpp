
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
static int frameflag = 0;

int MultiChVideo::creat()
{

	VCap[0] = new v4l2_camera(0);
	VCap[0]->creat();
	
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

void MultiChVideo::process(int chId)
{
	fd_set fds;
	struct timeval tv;
	int ret;
	Mat frame;
	enum v4l2_buf_type type;

	FD_ZERO(&fds);

	FD_SET(VCap[chId]->m_devFd, &fds);

	tv.tv_sec = 1;
	tv.tv_usec = 0;//250000

	ret = select(VCap[chId]->m_devFd+1, &fds, NULL, NULL, &tv);

	if(-1 == ret)
	{
		if (EINTR == errno)
			return;
	}else if(0 == ret)
	{

	}

	if(VCap[chId]->m_devFd != -1 && FD_ISSET(VCap[chId]->m_devFd, &fds))
	{
		struct v4l2_buffer buf;
		memset(&buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;//V4L2_MEMORY_MMAP

		if (-1 == v4l2_camera::xioctl(VCap[chId]->m_devFd, VIDIOC_DQBUF, &buf))
		{
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
			}
		}
		
	}
}



