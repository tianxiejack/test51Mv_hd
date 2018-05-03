
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.hpp>
#include <glut.h>
#include "process021.hpp"

using namespace std;
using namespace cv;

int main(int argc, char **argv)
{

	CProcess021 proc;
	proc.creat();
	proc.init();
	proc.run();
	glutMainLoop();
	proc.destroy();
    return 0;
}


