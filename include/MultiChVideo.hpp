
#ifndef MULTICHVIDEO_HPP_
#define MULTICHVIDEO_HPP_

#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osa_sem.h"
#include "v4l2camera.hpp"

typedef int (* MultiCh_CB)(void *handle, int chId, Mat frame);

class MultiChVideo;
typedef struct _Thread_Context{
	MultiChVideo  *pUser;
	int chId;
}THD_CXT;


class MultiChVideo 
{
public:
	MultiChVideo();
	~MultiChVideo();
	int creat();
	int destroy();
	int run();
	int stop();
	int run(int chId);
	int stop(int chId);
	int query();

public:
	MultiCh_CB m_usrFunc;
	void *m_user;
	int width[MAX_CHAN];
	int height[MAX_CHAN];
private:
	v4l2_camera *VCap[MAX_CHAN];
	OSA_ThrHndl m_thrCap[MAX_CHAN];
	THD_CXT m_thrCxt[MAX_CHAN];
	void process(void);
	void process(int chId);
	static void *capThreadFunc(void *context)
	{
		THD_CXT *muv = (THD_CXT *)context;
		for(;;){
			muv->pUser->process(muv->chId);
		}
		return NULL;
	}

};


#endif /* MULTICHVIDEO_HPP_ */

