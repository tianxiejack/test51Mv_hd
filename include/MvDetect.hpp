#ifndef MVDETECT_HPP_
#define MVDETECT_HPP_

//inNumber 代表创建 检测OBJ的实例个数  有效值从1到8
extern void createDetect(unsigned char inNumber,int inwidth,int inheight);
extern void exitDetect();

//index 代表第几个 检测OBJ 执行，boundRect 输出 目标的矩形框参数
extern void mvDetect(unsigned char index,unsigned char* inframe,int width,int height,cv::Rect *boundRect);

#endif
