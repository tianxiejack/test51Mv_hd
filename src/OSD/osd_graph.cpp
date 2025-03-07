
#include "osa.h"
#include "osa_thr.h"
#include "osa_buf.h"
#include "osd_graph.h"
#include "app_osdgrp.h"
#include "app_status.h"
#include"dx.h"
#include"app_ctrl.h"
#include "msgDriv.h"
#include "grpFont.h"

Multich_graphic grpxChWinPrms;



#define CVDRAW 0

#define RED 0xFFFF0000
#define GREEN 0xFF00FF00
#define BLUE 0xFF0000FF
#define YELLO 0xFFFFFF00
#define BLACKCOLOR	0xFF000000
#define WHITECOLOR	0xFFFFFFFF
#define BGCOLOR 0x00000000

OSA_MutexHndl osd_mutex;

 Scalar  GetColour(int colour)
{

	switch(colour)
		{
		case BGCOLOR:
			return Scalar(0,0,0,0);
			break;
		case BLACKCOLOR:
			return Scalar(0,0,0,255);
			break;
		case WHITECOLOR:
			return Scalar(255,255,255,255);
			break;
		case RED:
			return Scalar(0,0,255,255);
			break;
		case YELLO:
			return Scalar(0,255,255,255);
			break;
		case BLUE:
			return Scalar(255,0,0,255);
			break;
		case GREEN:
			return Scalar(0,255,0,255);
			break;
		default:
			return Scalar(255,255,255,255);
			break;
		}


}

void DrawLine(Mat frame, int startx, int starty, int endx, int endy, int width, UInt32 colorRGBA)
{
	char R, G, B, A;
	uchar *p;

	R = 0;
	G = 0;
	B = 0;
	A = 255;

	return ;
}
//RED
void DrawHLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA)
{

if (CVDRAW)
{

	Scalar color=GetColour(colorRGBA);
	line(frame, cvPoint(startx,starty),cvPoint(startx+len,starty),  color, width, 8, 0 ); 

}
else
{
	int endx, endy;
	char R, G, B, A;
	uchar *p;

	GETRGBA(R, G, B, A, colorRGBA);
	endx = startx+len;
	endy = starty+width;

	for(int i=starty;i<endy;i++)
	{
		p=frame.ptr<uchar>(i);
		for(int j=startx;j<endx;j++)
		{
			*(p+j*4)=R;
			*(p+j*4+1)=G;
			*(p+j*4+2)=B;
			*(p+j*4+3)=A;
		}
	}

}

	return ;
}

void DrawVLine(Mat frame, int startx, int starty, int width, int len, UInt32 colorRGBA)
{


if (CVDRAW)
{

	Scalar color=GetColour(colorRGBA);
	line(frame, cvPoint(startx,starty),cvPoint(startx,starty+len),  color, width, 8, 0 ); 

}
else
{
	int endx, endy;
	char R, G, B, A;
	uchar *p;

	GETRGBA(R, G, B, A, colorRGBA);
	endx = startx+width;
	endy = starty+len;

	for(int i=starty;i<endy;i++)
	{
		p=frame.ptr<uchar>(i);
		for(int j=startx;j<endx;j++)
		{
			*(p+j*4)=R;
			*(p+j*4+1)=G;
			*(p+j*4+2)=B;
			*(p+j*4+3)=A;
		}
	}

}

	return ;
}

void drawArrow(cv::Mat& img, cv::Point pStart, cv::Point pEnd, int len, int alpha, cv::Scalar& color, int thickness, int lineType)
{
	const double PI = 3.1415926;  
	Point arrow; 
	double angle = atan2((double)(pStart.y - pEnd.y), (double)(pStart.x - pEnd.x)); 
	line(img, pStart, pEnd, color, thickness, lineType); 
	arrow.x = pEnd.x + len * cos(angle + PI * alpha / 180);
	arrow.y = pEnd.y + len * sin(angle + PI * alpha / 180);
	line(img, pEnd, arrow, color, thickness, lineType);
	arrow.x = pEnd.x + len * cos(angle - PI * alpha / 180); 
	arrow.y = pEnd.y + len * sin(angle - PI * alpha / 180); 
	line(img, pEnd, arrow, color, thickness, lineType);
}



void drawwujiao(cv::Mat& img, cv::Point pStart,int len, cv::Scalar& color)
{
	const double PI = 3.1415926;
	double angle=2*PI/10;

	
	int x=pStart.x;
	int y=pStart.y;
	Point root_points[1][5];
	Point tri_points[6][5];
	int ylen=len*cos(angle);
	int xlen=len*sin(angle);
	int xlen1=len*sin(2*angle);
	int ylen1=len*cos(2*angle);
	
	tri_points[0][0] = Point(x+xlen,y-ylen);
       tri_points[0][1] = Point(x-xlen,y-ylen);
       tri_points[0][2] = Point(x-xlen1,y+ylen1);
       tri_points[0][3] = Point(x,y+len);
	tri_points[0][4] = Point(x+xlen1,y+ylen1);
	
	double angle1 =PI * 72 / 180;
	int tlen=xlen/cos(angle1);
	int tylen=tlen*sin(angle1);

	angle1=PI * 54/ 180;
	int txlen1=tlen*sin(angle1);
	int tylen1=tlen*cos(angle1);
	
	
	
	
		tri_points[1][0] = tri_points[0][0];
       tri_points[1][1] = tri_points[0][1];
       tri_points[1][2] = Point(x,y-ylen1-tylen-2);

       tri_points[2][0] = tri_points[0][1];
       tri_points[2][1] =tri_points[0][2];
       tri_points[2][2] = Point(x-xlen-tlen,y-ylen);

       tri_points[3][0] = tri_points[0][2];
       tri_points[3][1] =tri_points[0][3];
       tri_points[3][2] = Point(x-txlen1,y+tylen1+5);

	tri_points[4][0] = tri_points[0][3];
       tri_points[4][1] =tri_points[0][4];
       tri_points[4][2] = Point(x+txlen1,y+tylen1+5);

	

	tri_points[5][0] = tri_points[0][4];
       tri_points[5][1] =  tri_points[0][0];
       tri_points[5][2] =  Point(x+xlen+tlen,y-ylen);


#if 0
	int npt[6] = {5,3,3,3,3,3};
	const Point* ppt[6] = {tri_points[0],tri_points[1],tri_points[2],tri_points[3],tri_points[4],tri_points[5]};
	fillPoly(img, ppt, npt, 6, Scalar(0,0,255,255),8);


#else

	int npt[6] = {5,3,3,3,3,3};
	const Point* ppt[6] = {tri_points[0],tri_points[1],tri_points[2],tri_points[3],tri_points[4],tri_points[5]};
	fillPoly(img, ppt, npt, 6, color,8);

#endif
	   

	
	
	
	
}

void DrawChar(Mat frame, int startx, int starty, char *pChar, UInt32 frcolor, UInt32 bgcolor)
{
	

}



void DrawStringcv(Mat & frame, int startx, int starty, char *pChar, UInt32 frcolor, UInt32 bgcolor)
{

	int numchar = strlen(pChar);
	Scalar color=GetColour(frcolor);
	if(pChar[0]=='&')
		{
			drawArrow(frame,Point(startx+20,starty+15),Point(startx,starty+15),10,30,color,2,8);
		
			//
		}
	else if(pChar[0]=='#')
		{

			drawArrow(frame,Point(startx,starty+15),Point(startx,starty-15),10,30,color,2,8);
		}
	else if(pChar[0]=='!')
		{

			drawArrow(frame,Point(startx,starty-15),Point(startx,starty+15),10,30,color,2,8);
		}
	else if(pChar[0]=='\'')
		{

			drawArrow(frame,Point(startx-20,starty+15),Point(startx,starty+15),10,30,color,2,8);
		}
	else if(pChar[0]=='|')
		{

			//drawArrow(frame,Point(startx,starty+15),Point(startx+20,starty+15),10,30,color,2,8);
			putText(frame,"l",cvPoint(startx-10,starty+24),CV_FONT_HERSHEY_SIMPLEX,0.8,color,2,8);
			line(frame, cvPoint(startx-10,starty), cvPoint(startx,starty+24), color, 2, 8, 0 ); 
			
			
		}
	else if(pChar[0]=='~')
		{
			drawwujiao(frame,Point(startx+10,starty),4,color);
			

		}
	else
	putText(frame,pChar,cvPoint(startx-10,starty+24),CV_FONT_HERSHEY_SIMPLEX,0.8,color,2,8);

}

void DrawString(Mat frame, int startx, int starty, char *pString, UInt32 frcolor, UInt32 bgcolor)
{

if (CVDRAW)
{
	DrawStringcv(frame, startx, starty, pString, frcolor, bgcolor);
}
else
{
#if 0
	uchar *q,*pin,c;
	q=FONT_LIBRARY_0814;
	int numchar=strlen(pString);
	int i,j,k,pixcolor,data,index;
	for(i=starty;i<starty+OSDUTIL_FONT_FLR_DEFAULT_HEIGHT_0814;i++)
	{
		pin=frame.ptr<uchar>(i);
		for(k=0;k<numchar;k++)
		{
			c=pString[k];
			index=c-' ';
			data=q[i-starty+index*OSDUTIL_FONT_FLR_DEFAULT_HEIGHT_0814];
			for(j=startx+k*OSDUTIL_FONT_FLR_DEFAULT_WIDTH_0814;j<startx+k*OSDUTIL_FONT_FLR_DEFAULT_WIDTH_0814+OSDUTIL_FONT_FLR_DEFAULT_WIDTH_0814;j++)
			{
				pixcolor=(data&0x80)?frcolor:bgcolor;
				*(pin+j*4)=pixcolor & 0xFF;
				*(pin+j*4+1)=(pixcolor >> 8) & 0xFF;
				*(pin+j*4+2)=(pixcolor >> 16) & 0xFF;
				*(pin+j*4+3)=(pixcolor >> 24) & 0xFF;
				data<<=1;
			}
		}
	}
#endif


	uchar *pin, *pChar, *fontData;
	int numchar = strlen(pString);
	UInt32 i,j,k,pixcolor,index, offset;
	int fontWidth, fontHeight;
	unsigned char data;
	bool FontFind = 0;
	int add=0;

	fontWidth 	= 	OSDUTIL_FONT_FLR_DEFAULT_WIDTH_0814;
	fontHeight 	= 	OSDUTIL_FONT_FLR_DEFAULT_HEIGHT_0814;
	fontData      =	FONT_LIBRARY_0814;
	if(fontWidth%8!=0)
		{
			add=1;
		}
	else
		{
			add=0;
		}
	for(i=0; i<fontHeight; i++)
	{
		pin = frame.ptr<uchar>(starty+i);
		for(k=0; k<numchar; k++)
		{
			index = (UInt32)pString[k];
			index=index-' ';
			pChar = &fontData[i*(fontWidth/8+add)+index*(fontWidth/8+add)*fontHeight];
			//printf("draw str:%c [%d]\n", pString[k], index);
			for(j=startx+k*fontWidth; j<startx+k*fontWidth+fontWidth; j++)
			{
				offset 	= j-startx-k*fontWidth;
				data 	= *(pChar + offset/8);
				data 	<<= (offset%8);

				pixcolor		= (data&0x80)?frcolor:bgcolor;
				*(pin+j*4)		= pixcolor & 0xFF;
				*(pin+j*4+1)	= (pixcolor >> 8) & 0xFF;
				*(pin+j*4+2)	= (pixcolor >> 16) & 0xFF;
				*(pin+j*4+3)	= (pixcolor >> 24) & 0xFF;
			}
		}
	}


}

	
}

void osd_draw_cross(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;

	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

	if(1)
	{
		// only cross
		len = pObj->width;
		iX = pObj->x - (pObj->width>>1);
		iY = pObj->y;
		DrawHLine(frame,iX,iY,width,len,iColor);

		height = pObj->height;
		len = height;
		iX = pObj->x;
		iY = pObj->y - (len>>1);
		DrawVLine(frame,iX,iY,width,len,iColor);
	}
	else
	{
		// with center point
		//left horizonal line
		len = (pObj->width>>1)-(pObj->lineGapWidth>>1);
		iX = pObj->x - (pObj->width>>1);
		iY = pObj->y;
		DrawHLine(frame,iX,iY,width,len,iColor);
		//middle horizonal line
		len = pObj->linePixels;
		iX = pObj->x;
		iY = pObj->y;
		DrawHLine(frame,iX,iY,width,len,iColor);
		//right horizonal line
		len = (pObj->width>>1)-(pObj->lineGapWidth>>1);
		iX = pObj->x+(pObj->lineGapWidth>>1)+2;
		iY = pObj->y;
		DrawHLine(frame,iX,iY,width,len,iColor);
		//top vertical line
		height = pObj->height;
		len = (height>>1)-(pObj->lineGapHeight>>1);
		iX = pObj->x;
		iY = pObj->y - (height>>1);
		DrawVLine(frame,iX,iY,width,len,iColor);
		//bottom vertical line
		height = pObj->height;
		len = (height>>1)-(pObj->lineGapHeight>>1);
		iX = pObj->x;
		iY = pObj->y+(pObj->lineGapHeight>>1)+2;
		DrawVLine(frame,iX,iY,width,len,iColor);
	}

	return ;
}

void osd_draw_rect(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;

	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

	//up Hline
	iX = pObj->x-(pObj->width>>1);
	iY = pObj->y-(pObj->height>>1);
	len = pObj->width;
	DrawHLine(frame,iX,iY,width,len,iColor);

	//right Vline
	iX += pObj->width-pObj->linePixels;
	len = pObj->height;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//left Vline
	iX -= pObj->width-pObj->linePixels;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//bottom Hline
	iY += pObj->height-width;
	len = pObj->width;
	DrawHLine(frame,iX,iY,width,len,iColor);

	return ;
}

void osd_draw_rect_gap(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;

	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

	//left top line
	iX = pObj->x-pObj->width/2;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x-pObj->width/2;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//right top line
	iX = pObj->x+pObj->width/2-pObj->lineGapWidth;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x+pObj->width/2-width;
	iY = pObj->y-pObj->height/2;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//right bottom line
	iX = pObj->x+pObj->width/2-pObj->lineGapWidth;
	iY = pObj->y+pObj->height/2-width;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x+pObj->width/2-width;
	iY = pObj->y+pObj->height/2-pObj->lineGapHeight;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	//left bottom line
	iX = pObj->x-pObj->width/2;
	iY = pObj->y+pObj->height/2-width;
	len = pObj->lineGapWidth;
	DrawHLine(frame,iX,iY,width,len,iColor);

	iX = pObj->x-pObj->width/2;
	iY = pObj->y+pObj->height/2-pObj->lineGapHeight;
	len = pObj->lineGapHeight;
	DrawVLine(frame,iX,iY,width,len,iColor);

	return ;
}

void osd_draw_text(Mat frame, void *prm)
{
	Text_Param_fb * pObj = (Text_Param_fb *)prm;
	UInt32 fontfrColor,fontbgColor;

	if(pObj == NULL)
		return ;

	int i;
	int x[3];
	int y[3];
	int vaild[3];
	char textStr[3][128];
	int textLenPrev[3];

	for (i = 0; i < 3; i++)
	{
		x[i] = pObj->text_x[i];
		y[i] = pObj->text_y[i];
	}
	vaild[0] = pObj->text_valid&0x0001;
	vaild[1] = (pObj->text_valid >> 1)&0x0001;
	vaild[2] = (pObj->text_valid >> 2)&0x0001;
	textLenPrev[0] = 0;
	textLenPrev[1] = pObj->textLen[0];
	textLenPrev[2] = pObj->textLen[0] + pObj->textLen[1];
	OSA_assert( textLenPrev[0] < 128 && textLenPrev[1] < 128 && textLenPrev[2] < 128 );

	memcpy(textStr[0], pObj->textAddr, pObj->textLen[0]);
	textStr[0][pObj->textLen[0]] = '\0';
	memcpy(textStr[1], pObj->textAddr + textLenPrev[1], pObj->textLen[1]);
	textStr[1][pObj->textLen[1]] = '\0';
	memcpy(textStr[2], pObj->textAddr + textLenPrev[2], pObj->textLen[2]);
	textStr[2][pObj->textLen[2]] = '\0';
	for (i = 0; i < 3; i++)
	{
		if(vaild[i] != 0)
		{
			fontfrColor = pObj->frcolor;
			fontbgColor = pObj->bgcolor;
		}
		else
		{
			fontfrColor = pObj->bgcolor;
			fontbgColor = pObj->bgcolor;
		}
	
		DrawString(frame, x[i], y[i], textStr[i], fontfrColor, fontbgColor);
		//printf("x[%d]=%d,y[%d]=%d,testStr[%d]=%s\n",i,x[i],i,y[i],i,textStr[i]);
		//DrawString(frame, x[i], y[i], textStr[i], iColor);
	}

	return ;
}

void osd_draw_cross_black_white(Mat frame, void *prm)
{
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;

	if(pObj == NULL)
		return ;

	width = pObj->linePixels;
	iColor = pObj->frcolor;

        //left horizonal line
        len = (pObj->width>>1)-(pObj->lineGapWidth>>1);
        iX = pObj->x - (pObj->width>>1);
        iY = pObj->y;
        DrawHLine(frame,iX,iY,width,len,BLACKCOLOR);
	 DrawHLine(frame,iX,iY+width,width,len,WHITECOLOR);
        //middle horizonal line
        len = pObj->linePixels;
        iX = pObj->x;
        iY = pObj->y;
        DrawHLine(frame,iX,iY,width,len,WHITECOLOR);
	 DrawHLine(frame,iX,iY+width,width,len,WHITECOLOR);
        //right horizonal line
        len = (pObj->width>>1)-(pObj->lineGapWidth>>1);
        iX = pObj->x+(pObj->lineGapWidth>>1)+width;		// ?
        iY = pObj->y;
        DrawHLine(frame,iX,iY,width,len,BLACKCOLOR);
        DrawHLine(frame,iX,iY+width,width,len,WHITECOLOR);
        //top vertical line
        height = pObj->height;
        len = (height>>1)-(pObj->lineGapHeight>>1);
        iX = pObj->x;
        iY = pObj->y - (height>>1);
        DrawVLine(frame,iX,iY,width,len,WHITECOLOR);
        DrawVLine(frame,iX+width,iY,width,len,BLACKCOLOR);
        //bottom vertical line
        height = pObj->height;
        len = (height>>1)-(pObj->lineGapHeight>>1);
        iX = pObj->x;
        iY = pObj->y+(pObj->lineGapHeight>>1)+width;	// ?
        DrawVLine(frame,iX,iY,width,len,WHITECOLOR);
        DrawVLine(frame,iX+width,iY,width,len,BLACKCOLOR);

	return ;
}






int osd_draw_Hori_Menu(Mat frame, void *prm)
{
	int color = 0;
	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
	//pObj->linePixels=pObj->linePixels*2;//TEST BLINK;
	//pObj->height=pObj->height+20;
	//pObj->y=65*2;//TEST BLINK;
	//pObj->x=pObj->x+9;
	//pObj->y=pObj->y-2;

	iColor=pObj->frcolor;
	iX=pObj->x;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->lineGapWidth;
	//printf("the x=%d y=%d \n",iX,iY);
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y+pObj->height;
	width= pObj->linePixels;
	len=pObj->lineGapWidth;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	// return 0;

	
	 iX=pObj->x;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x+pObj->width;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x+pObj->width*2;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x+pObj->width*3;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);
	 
	 iX=pObj->x+pObj->width*4;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);

 	iX=pObj->x+pObj->width*5-2;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	




    return 0;
}

int osd_draw_Vert_Menu(Mat frame, void *prm)
{
	int color = 0;

	Line_Param_fb * pObj = (Line_Param_fb *)prm;
	UInt32 iX, iY, iColor;
	UInt32 width, len, height;
	//pParamFb->color = 0x00EEEEEE;

	//printf(" %s the x =%d   the y=%d  the width=%d  the len=%d\n",__func__, pObj->x,pObj->y,pObj->linePixels,pObj->width);

/*
-----|
-----|
-----|
-----|
-----|
*/

	iColor=pObj->frcolor;
	iX=pObj->x-pObj->width;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	
	
	iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);


	 iX=pObj->x;
	iY = pObj->y+pObj->height;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	

	 iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height*2;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y+pObj->height*2;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	

	 iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height*3;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

	 iX=pObj->x;
	iY = pObj->y+pObj->height*3;
	width= pObj->linePixels;
	len=pObj->height;
	 DrawVLine(frame,iX,iY,width,len,iColor);	

	 iX=pObj->x-pObj->width;
	iY = pObj->y+pObj->height*4-2;
	width= pObj->linePixels;
	len=pObj->width;
	 DrawHLine(frame,iX,iY,width,len,iColor);

    return 0;
}





int process_draw_line(Mat frame,int startx,int starty,int endx,int linewidth,char R,char G,char B,char A)
{
	uchar *p;
	for(int i=starty;i<starty+linewidth;i++)
	{
		p=frame.ptr<uchar>(i);
		for(int j=startx;j<endx;j++)
		{
			*(p+j*4)=R;
			*(p+j*4+1)=G;
			*(p+j*4+2)=B;
			*(p+j*4+3)=A;
		}
	}
	return 0;
}

int process_draw_text(Mat frame,int startx,int starty,char *pstring,int frcolor,int bgcolor)
{

	uchar *q,*pin,c;
	q=FONT_LIBRARY_0814;
	int numchar=strlen(pstring);
	int i,j,k,pixcolor,data,index;
	for(i=starty;i<starty+14;i++)
	{
		pin=frame.ptr<uchar>(i);
		for(k=0;k<numchar;k++)
		{
			c=pstring[k];
			index=c-' ';
			data=q[i-starty+index*14];
			for(j=startx+k*8;j<startx+k*8+8;j++)
			{
				pixcolor=(data&0x80)?frcolor:bgcolor;
				*(pin+j*4)=pixcolor & 0xFF;
				*(pin+j*4+1)=(pixcolor >> 8) & 0xFF;
				*(pin+j*4+2)=(pixcolor >> 16) & 0xFF;
				*(pin+j*4+3)=(pixcolor >> 24) & 0xFF;
				data<<=1;
			}
		}
	}




	return 0;
}

static void* MultichGrpx_task(void *pPrm)
{
    int status = 0;
    int chId = 0;
    osdprocess_CB fun=(osdprocess_CB )pPrm;
    OSA_printf(" %s task start!!! \r\n", __func__);
    Multich_graphic *pMultGraphicObj = &grpxChWinPrms;
    
    while (pMultGraphicObj->tskGraphicLoop == TRUE)
    {
        status = OSA_semWait(&pMultGraphicObj->tskGraphicSem, OSA_TIMEOUT_FOREVER);

        if (pMultGraphicObj->tskGraphicLoop == FALSE)
            break;
       OSA_mutexLock(&pMultGraphicObj->muxLock);
	//OSA_printf(" %s task start!!! \r\n", __func__);
	fun(NULL);
        OSA_mutexUnlock(&pMultGraphicObj->muxLock);
    }
    pMultGraphicObj->tskGraphicStopDone = TRUE;
    OSA_printf(" %s task exit!!! \r\n", __func__);
    return NULL;
}


void  osdgraph_init(osdprocess_CB fun)
{
	int status=0;
	int i, devId, winId;
	Text_Param_fb * textParam = NULL;
	Line_Param_fb * lineParam = NULL;
	memset(&grpxChWinPrms, 0, sizeof(Multich_graphic));

	grpxChWinPrms.devFb_num = 1;
	for ( devId = 0; devId < grpxChWinPrms.devFb_num; devId++)
	{
		grpxChWinPrms.chParam[devId].chId = devId;
		grpxChWinPrms.chParam[devId].chEnable = TRUE;
		grpxChWinPrms.chParam[devId].numWindows = WINID_MAX/2;

		for(i = WINID_TV_CROSSAXIS; i < WINID_MAX; i++)
		{
			winId = i/2;
			textParam = &grpxChWinPrms.chParam[devId].winPrms[winId];
			lineParam = (Line_Param_fb *)&grpxChWinPrms.chParam[devId].winPrms[winId];

			if(winId == WINID_TV_CROSSAXIS)
			{
				lineParam->enableWin 		= 1;
				lineParam->objType 		= grpx_ObjId_Cross;
				//lineParam->objType = grpx_ObjId_Cross_Black_White;
				lineParam->frcolor 		= WHITECOLOR;
				lineParam->x 				= vdisWH[devId][0]/2;
				lineParam->y 				= vdisWH[devId][1]/2;
				lineParam->width 			= 60;
				lineParam->height 		= 40;
				lineParam->linePixels 		= 2;
				lineParam->lineGapWidth 	= lineParam->width/4;
				lineParam->lineGapHeight 	= lineParam->height/4;
			}

			if(winId == WINID_TV_AIMAREA)
			{
				lineParam->enableWin 		= 0;
				lineParam->objType 		= grpx_ObjId_Rect;
				lineParam->frcolor 		= WHITECOLOR;
				lineParam->x 				= vdisWH[devId][0]/2;
				lineParam->y 				= vdisWH[devId][1]/2;
				lineParam->width 			= 60;
				lineParam->height 		= 40;
				lineParam->linePixels 		= 2;
				lineParam->lineGapWidth 	= lineParam->width;
				lineParam->lineGapHeight 	= lineParam->height;
			}
		}
	}

	grpxChWinPrms.tskGraphicLoop 			= TRUE;
	grpxChWinPrms.tskGraphicStopDone 		= FALSE;

	status = OSA_semCreate(&grpxChWinPrms.tskGraphicSem, 1, 0);
	OSA_assert(status == OSA_SOK);

	status = OSA_thrCreate(
	             &grpxChWinPrms.tskGraphicHndl,
	             MultichGrpx_task,
	             OSA_THR_PRI_DEFAULT,
	             0,
	            (void *)fun
	         );

	OSA_assert(status == OSA_SOK);


	OSA_mutexCreate(&osd_mutex);

	grpxChWinPrms.bGraphicInit = TRUE;
	
	OSA_printf(" %s done.\n", __func__);

}


int timesta=0;
static int timecount=0;
void  set_graph()
{
	timesta=1;
}
void MultichGrpx_update_sem_post(void)
{

    //int timecount=5;
    if(timesta==0)
    	{
		return ;
    	}
    Multich_graphic *pMultGraphicObj = &grpxChWinPrms;

    if (!pMultGraphicObj->bGraphicInit)
        return ;

	if(timecount==0)
   			 OSA_semSignal(&pMultGraphicObj->tskGraphicSem);
	else
			timecount++;
	if(timecount>=1)
		timecount=0;

}

static int APP_get_Rgb_Color(int colorId)
{
	int iRtnColor;

	if(colorId == ecolor_White)
		iRtnColor = 0xFEFEFE;
	else if(colorId == ecolor_Black)
		iRtnColor = 0x000000;
	else if(colorId==ecolor_Red)
		iRtnColor = 0xFE0000;
	else if(colorId==ecolor_Yellow)
		iRtnColor = 0xFEFE00;
	else if(colorId==ecolor_Blue)
		iRtnColor = 0x0000FE;
	else if(colorId==ecolor_Green)
		iRtnColor = 0x00FE00;
	else
		iRtnColor = 0xFEFEFE;	// ecolor_Default is white
	return iRtnColor;
}

#if 1
Int32 APP_set_graphic_parms_fb( Int32 blkId, Int32 fieldId,Multich_graphic *pMultGrap,int winid)
{
    int configId   = CFGID_BUILD(blkId, 2);
    int graph_id   = GET_GRAPHIC_ID(gDXD_info.sysConfig[configId]);
    int windows_id = GET_WINDOWS_ID(gDXD_info.sysConfig[configId]);
    int sensors_id = GET_SENSORS_ID(gDXD_info.sysConfig[configId]);

    int user_draw_id = 0;
	
    Text_Param_fb * textLinkParam = NULL;

    int disMask[eSen_Max];
    disMask[0] = gDXD_info.sysConfig[CFGID_RTS_TV_SEN_GRAPH];
    disMask[1] = gDXD_info.sysConfig[CFGID_RTS_FR_SEN_GRAPH];

    int osdlevel[eSen_Max],i = 0;
    osdlevel[0] = gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]  % 5;
    osdlevel[1] = gDXD_info.sysConfig[CFGID_OC25_FR_OSD_LEVEL]  % 5;

    OSA_assert( sensors_id == 0 || sensors_id == 1);
    //OSA_assert( windows_id < WINID_MAX / 2);
    OSA_assert( graph_id   >= grpx_ObjId_Cross && graph_id <= grpx_ObjId_Text);

  
   // OSA_printf(" %s blkId %03d sensors_id 0x%04x windows_id 0x%02x graph_id 0x%02x, osd_id 0x%06x",
    //        __func__,blkId,sensors_id,windows_id,graph_id,gDXD_info.sysConfig[configId]);

   windows_id = (blkId - 94)/2;
    OSA_assert( windows_id < ALG_LINK_GRPX_MAX_WINDOWS);

	if(sensors_id)
		return OSA_SOK;
   
     
     textLinkParam = &pMultGrap->chParam[0].winPrms[windows_id];
     OSA_assert(textLinkParam != NULL);
     user_draw_id = (blkId-94) % 2;
	// graph_id=grpx_ObjId_Text;
	//printf("**********************************the graphid=%d\n",graph_id); 
    switch (graph_id)
    {
    	case grpx_ObjId_Text:
        {
		//textLinkParam->enableWin=1;
		textLinkParam->bgcolor=BGCOLOR;
		textLinkParam->frcolor=APP_get_colour(sensors_id);
		//textLinkParam->text_valid=0x7;
		textLinkParam->objType = grpx_ObjId_Text;
			
		//printf("*********************************************  valid=%x\n",textLinkParam->text_valid);
	    OSD_PROPERTY_GET(textLinkParam->enableWin,     blkId, 1);
          //  OSD_PROPERTY_GET(textLinkParam->frcolor,      blkId, 3);
            OSD_PROPERTY_GET(textLinkParam->text_valid, blkId, 6);
            OSD_PROPERTY_GET(textLinkParam->text_x[0],  blkId, 7);
            OSD_PROPERTY_GET(textLinkParam->text_y[0],  blkId, 8);
            OSD_PROPERTY_GET(textLinkParam->textLen[0], blkId, 9);
            OSD_PROPERTY_GET(textLinkParam->text_x[1],  blkId, 10);
            OSD_PROPERTY_GET(textLinkParam->text_y[1],  blkId, 11);
            OSD_PROPERTY_GET(textLinkParam->textLen[1], blkId, 12);
            OSD_PROPERTY_GET(textLinkParam->text_x[2],  blkId, 13);
            OSD_PROPERTY_GET(textLinkParam->text_y[2],  blkId, 14);
            OSD_PROPERTY_GET(textLinkParam->textLen[2], blkId, 15);


            APP_text_genContext(sensors_id, windows_id,
                                (char*)textLinkParam->textAddr, SHMEM_LEN,
                                textLinkParam->textLen);

		//printf("textLen[0]=%x textLen[1]=%x textLen[2]=%x\n", textLinkParam->textLen[0],textLinkParam->textLen[1],textLinkParam->textLen[2]);
            CFG_FIELD_FILL(textLinkParam->text_valid, blkId,  6);
            CFG_FIELD_FILL(textLinkParam->textLen[0], blkId,  9);
            CFG_FIELD_FILL(textLinkParam->textLen[1], blkId, 12);
            CFG_FIELD_FILL(textLinkParam->textLen[2], blkId, 15);
#if 0
		if (windows_id%(WINID_GRAPHIC_MAX/2)*2==WINID_TV_AXIS)
{
		textLinkParam->text_valid=7;
		textLinkParam->text_x[0]=30;
		textLinkParam->text_y[0]=50;
	       OSA_printf("blkId %d ,x[0]: %d,y[0] :%d len[0]=%d  value=%d\n",blkId, textLinkParam->text_x[0],textLinkParam->text_y[0],textLinkParam->textLen[0],textLinkParam->text_valid);
		OSA_printf("x[1]: %d,y[1] : %d len[1]=%d value=%d\n", textLinkParam->text_x[1],textLinkParam->text_y[1],textLinkParam->textLen[1],textLinkParam->text_valid);
		OSA_printf("x[2]: %d,y[2] : %d len[2]=%d value=%d\n", textLinkParam->text_x[2],textLinkParam->text_y[2],textLinkParam->textLen[2],textLinkParam->text_valid);
}

	#endif		
			

         //   textLinkParam->frcolor  = APP_get_Rgb_Color(textLinkParam->frcolor);
	 //    textLinkParam->enableWin = (disMask[sensors_id] & eDisp_show_text) ? textLinkParam->enableWin : 0;
     #if 1
	     if(osdlevel[sensors_id] > 0x02)
	     {
		textLinkParam->text_valid= 0;
	     }
	     else if(osdlevel[sensors_id] == 0x02)
	     {
		   //if((blkId-94) == (WINID_TV_UPDATE_MENU_0 + sensors_id) ||
		   	//    (blkId-94) == (WINID_TV_UPDATE_MENU_1+ sensors_id))
		   if((blkId-94) == (WINID_TV_VCP_SYS + sensors_id))	  
			textLinkParam->text_valid = 0;
	     }	
	#endif
        }
        break;

    default:
        return OSA_EFAIL;
    }
    return OSA_SOK;
}

Int32 APP_get_colour( int sensorid)
{
	// int configId   = CFGID_BUILD(blkId, fieldId);
	// int sensors_id = GET_SENSORS_ID(gDXD_info.sysConfig[configId]);
	int configId ;
	if(sensorid==0)
		configId=CFGID_RTS_TV_SEN_COLOR;
	else
		configId=CFGID_RTS_FR_SEN_COLOR;
	
	//if((gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]==3)||(gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]==4))
	//	{
	//		return BGCOLOR;

	//	}
	switch(gDXD_info.sysConfig[configId])
		{

		case 1:
			return BLACKCOLOR;
			break;
		case 2:
			return WHITECOLOR;
			break;
		case 3:
			return RED;
			break;
		case 4:
			return YELLO;
			break;
		case 5:
			return BLUE;
			break;
		case 6:
			return GREEN;
			break;
		default:
			return WHITECOLOR;
			break;




		}



}




Int32 APP_set_graphic_parms_line_fb( Int32 blkId, Int32 fieldId,Multich_graphic *pMultGrap)
{
    int configId   = CFGID_BUILD(blkId, 2);
    int graph_id   = GET_GRAPHIC_ID(gDXD_info.sysConfig[configId]);
    int windows_id = GET_WINDOWS_ID(gDXD_info.sysConfig[configId]);
    int sensors_id = GET_SENSORS_ID(gDXD_info.sysConfig[configId]);
	
    int user_draw_id = 0;
	
    Line_Param_fb * lineLinkParam = NULL;

    int disMask[eSen_Max];
    disMask[0] = gDXD_info.sysConfig[CFGID_RTS_TV_SEN_GRAPH];
    disMask[1] = gDXD_info.sysConfig[CFGID_RTS_FR_SEN_GRAPH];
	
    int osdlevel[eSen_Max],i = 0;
    osdlevel[0] = gDXD_info.sysConfig[CFGID_OC25_TV_OSD_LEVEL]  % 5;
    osdlevel[1] = gDXD_info.sysConfig[CFGID_OC25_FR_OSD_LEVEL]  % 5;

    OSA_assert( sensors_id == 0 || sensors_id == 1);
    OSA_assert( windows_id < WINID_MAX / 2);
    OSA_assert( graph_id   >= grpx_ObjId_Cross && graph_id <= grpx_objId_Vert_Menu);

	if(blkId == 84)
	{
		windows_id = WINID_TEXT_MAX/2 + 0;
	}
	else if(blkId == 86)
	{
		windows_id = WINID_TEXT_MAX/2 + 1;
	}
	else
		return OSA_SOK;
     
    /*OSA_printf(" %s blkId %03d sensors_id 0x%04x windows_id 0x%02x graph_id 0x%02x, osd_id 0x%06x",
            __func__,blkId,sensors_id,windows_id,graph_id,gDXD_info.sysConfig[configId]);*/
     lineLinkParam = (Line_Param_fb*)&pMultGrap->chParam[0].winPrms[windows_id];//lineParamFb[blkId-84];
     OSA_assert(lineLinkParam != NULL);
     user_draw_id = (blkId-84) % 2 ;
	 
    switch (graph_id)
    {
        case grpx_objId_Hori_Menu:
		//printf("***********************************************************grpx_objId_Hori_Menu\n");
		lineLinkParam->bgcolor=BGCOLOR;
		lineLinkParam->frcolor=APP_get_colour(sensors_id);
		//textLinkParam->text_valid=0x7;
		lineLinkParam->objType = grpx_objId_Hori_Menu;
		
            OSD_PROPERTY_GET(lineLinkParam->enableWin,       blkId, 1);
          //  OSD_PROPERTY_GET(lineLinkParam->color,        blkId, 3);
            OSD_PROPERTY_GET(lineLinkParam->x,            blkId, 6);
            OSD_PROPERTY_GET(lineLinkParam->y,            blkId, 7);
            OSD_PROPERTY_GET(lineLinkParam->width,        blkId, 8);
            OSD_PROPERTY_GET(lineLinkParam->height,       blkId, 9);
            OSD_PROPERTY_GET(lineLinkParam->linePixels,   blkId, 11);
            OSD_PROPERTY_GET(lineLinkParam->lineGapWidth, blkId, 12);
            OSD_PROPERTY_GET(lineLinkParam->lineGapHeight,blkId, 13);
	         if(osdlevel[sensors_id] > 0x02)
	   {
	       lineLinkParam->frcolor=BGCOLOR;// lineLinkParam->bgcolor;
	   }
	   else if(osdlevel[sensors_id] == 0x02)  
	   {
    	     //if((blkId-84) == (WINID_TV_HORI_MENU +sensors_id))
    	     //if(((blkId-84)==0))
    	        //lineLinkParam->frcolor= lineLinkParam->bgcolor;
	   }	



			
			break;
        case grpx_objId_Vert_Menu:	
        {


		lineLinkParam->bgcolor=BGCOLOR;
		lineLinkParam->frcolor=APP_get_colour(sensors_id);
		//textLinkParam->text_valid=0x7;
		lineLinkParam->objType = grpx_objId_Vert_Menu;
		
            OSD_PROPERTY_GET(lineLinkParam->enableWin,       blkId, 1);
          //  OSD_PROPERTY_GET(lineLinkParam->color,        blkId, 3);
            OSD_PROPERTY_GET(lineLinkParam->x,            blkId, 6);
            OSD_PROPERTY_GET(lineLinkParam->y,            blkId, 7);
            OSD_PROPERTY_GET(lineLinkParam->width,        blkId, 8);
            OSD_PROPERTY_GET(lineLinkParam->height,       blkId, 9);
            OSD_PROPERTY_GET(lineLinkParam->linePixels,   blkId, 11);
            OSD_PROPERTY_GET(lineLinkParam->lineGapWidth, blkId, 12);
            OSD_PROPERTY_GET(lineLinkParam->lineGapHeight,blkId, 13);
			//break;
        	//}

#if 1
           // lineLinkParam->color  = APP_get_Rgb_Color(lineLinkParam->color);
           // lineLinkParam->enableWin = (disMask[sensors_id] &eDisp_show_rect)?lineLinkParam->enableWin:0;

	    if(osdlevel[sensors_id] > 0x02)
	   {
	       lineLinkParam->frcolor=BGCOLOR;// lineLinkParam->bgcolor;
	   }
	   else if(osdlevel[sensors_id] == 0x02)  
	   {
    	     //if((blkId-84) == (WINID_TV_HORI_MENU +sensors_id))
    	     //if(((blkId-84)==0))
    	        //lineLinkParam->frcolor= lineLinkParam->bgcolor;
	   }	
	   #endif
       }
        break;

    default:
        return OSA_EFAIL;
    }
    return OSA_SOK;
}
/***********************************END**************************************/

#endif



void Draw_graph_osd(Mat frame, void *tParam,void *lParam)
{
	Text_Param_fb * textParam = (Text_Param_fb *)tParam;
	Line_Param_fb * lineParam = (Line_Param_fb *)lParam;

	switch(textParam->objType)
	{
		case grpx_ObjId_Cross:
			osd_draw_cross(frame, lineParam);
			break;
		case grpx_ObjId_Rect:
			osd_draw_rect(frame, lineParam);
			break;
		case grpx_ObjId_Rect_gap:
			osd_draw_rect_gap(frame, lineParam);
			break;
		case grpx_ObjId_Compass:
			break;
		case grpx_ObjId_Text:
			osd_draw_text(frame, textParam);
			break;

		// for project
		case grpx_ObjId_Cross_Black_White:
			osd_draw_cross_black_white(frame, lineParam);
			break;
		case grpx_ObjId_Rect_Black_White:
			break;
		case grpx_objId_Hori_Menu:
			osd_draw_Hori_Menu(frame, lineParam);
			break;
		case grpx_objId_Vert_Menu:
			osd_draw_Vert_Menu(frame, lineParam);
			break;

		default:
			break;
	}

}

void EraseDraw_graph_osd(Mat frame, void *Param,void *Parampri)
{

	Text_Param_fb * textParam = (Text_Param_fb *)Param;
	Line_Param_fb * lineParam = (Line_Param_fb *)Param;
	Text_Param_fb * textParampri = (Text_Param_fb *)Parampri;
	Line_Param_fb * lineParampri = (Line_Param_fb *)Parampri;
	int i=0;
	int eraseflag[3];
	memset(eraseflag,0,3*sizeof(int));
	int eraseflag1=0;
	if(textParam->objType==grpx_ObjId_Text)
	{
		for(i=0;i<3;i++)
		{
			if(textParam->text_x[i]!=textParampri->text_x[i])
			{
				eraseflag[i]++;
			}

			if(textParam->text_y[i]!=textParampri->text_y[i])
			{
				eraseflag[i]++;
			}
		//BIT_CLRj(textParampri->text_valid,i);
		//if(eraseflag[i]!=0)
			textParampri->text_valid=0;
		}
	}
	else
	{
#if 1
		if(lineParam->x!=lineParampri->x)
		{
			eraseflag1++;
			OSA_printf("%s   line=%d  x=%d\n",__func__,__LINE__,lineParam->x);
		}
		if(lineParam->y!=lineParampri->y)
		{
			eraseflag1++;
			OSA_printf("%s   line=%d  y=%d\n",__func__,__LINE__,lineParam->y);
		}
	
		if(lineParam->width!=lineParampri->width)
		{
			eraseflag1++;
			OSA_printf("%s   line=%d\n",__func__,__LINE__);
		}
		if(lineParam->height!=lineParampri->height)
		{
			eraseflag1++;
			OSA_printf("%s   line=%d\n",__func__,__LINE__);
		}
		if(eraseflag1==0)
		{
			//printf("%s   line=%d\n",__func__,__LINE__);
			return ;
		}
#endif
		lineParampri->frcolor=lineParam->bgcolor;
		//printf("%s   line=%d\n",__func__,__LINE__);
	}
	Draw_graph_osd(frame,textParampri,lineParampri);
}
void APP_graphic_timer_alarm()
{

    int i;
    int blkId;

    if(msgextInCtrl==NULL)
		return ;
     CMD_EXT *pIStuts = msgextInCtrl;

	    if(pIStuts->TrkCoastCount)
    {
	pIStuts->TrkCoastCount--;
	if(pIStuts->TrkCoastCount == 0)
	{
		pIStuts->AvtTrkCoast = 0;
		MSGDRIV_send(MSGID_EXT_INPUT_COAST, 0);
	}
    }
    if(pIStuts->FreezeresetCount)
    	{
		pIStuts->FreezeresetCount--;
		if((pIStuts->FreezeresetCount==0)&&(pIStuts->FrCollimation==0x01))
			{

				pIStuts->FrCollimation=0x02;
				
				//pIStuts->PicpSensorStatpri=0x02;
				MSGDRIV_send(MSGID_EXT_INPUT_ENFREZZ, 0);
				app_ctrl_ack();
			}

    	}

	if((pIStuts->unitFaultStatpri&0x03)!=(pIStuts->unitFaultStat&0x03))
		{
			pIStuts->unitFaultStatpri=pIStuts->unitFaultStat;
			MSGDRIV_send(MSGID_EXT_INPUT_VIDEOEN, 0);
			//MSGAPI_AckSnd( AckCheck);
			OSA_printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^MSGID_EXT_INPUT_VIDEOEN**********\n");
		}

	
	//printf("!!!!!!!!!!!!!!!!!!!!!!!!unitFaultStatpri=%d  unitFaultStat=%d\n",pIStuts->unitFaultStatpri,pIStuts->unitFaultStat);
	
#if 0
    for(i = WINID_TV_MMT_0; i <= WINID_FR_MMT_9; i++)
    {
        blkId = CFGID_blkId(CFGID_OSD_GRAPH_OBJ_ID(i));

        APP_set_graphic_parms(blkId, 2);
    }
#endif


    OSA_mutexLock(&osd_mutex);
    for(i = WINID_TV_SOFTVER; i < WINID_TEXT_MAX; i++)
    {

        blkId = CFGID_blkId(CFGID_OSD_TEXT_OBJ_ID(i));
	APP_set_graphic_parms_fb(blkId, 2,&grpxChWinPrms,i);
    }


    for(i = WINID_TV_HORI_MENU; i <= WINID_FR_VERT_MENU; i++)
    {
        blkId = CFGID_blkId(CFGID_OSD_GRAPH_OBJ_ID(i));
	 APP_set_graphic_parms_line_fb(blkId, 2,&grpxChWinPrms);
    }
   OSA_mutexUnlock(&osd_mutex);
	
 //   MultichGrpx_update_sem_post();


}



void  APP_tracker_timer_alarm()
{

	 if(msgextInCtrl==NULL)
		return ;
        CMD_EXT *pIStuts = msgextInCtrl;
	 if(pIStuts->TrkCoastCount>0)
	 	{
			pIStuts->TrkCoastCount--;
	 	}
	 else
	 	{
			pIStuts->TrkCoastCount=0;
			pIStuts->AvtTrkCoast=0;

	 	}


}
void  APP_Err_feedback_timer_alarm()
{

	 if(msgextInCtrl==NULL)
		return ;
        CMD_EXT *pIStuts = msgextInCtrl;

	if(pIStuts->TrkErrFeedback)
		app_err_feedbak();
	


}


#include "grpFont.h"

