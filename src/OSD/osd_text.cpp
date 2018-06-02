/*
 * osd_text.cpp
 *
 *  Created on: 2018年5月23日
 *      Author: alex
 */

#include <stdio.h>
#include <stdlib.h>
//#include <GL/glut.h>
#include "freetype.hpp"

FreeTypeFont*	_font;

void OSDCreatText()
{
	_font   =   new FreeTypeFont();
	_font->create("simsun.ttc",40,512,512);
}

void OSDdrawText(int x,int y,wchar_t* text,int win_width,int win_height)
{
	_font->begin(win_width,win_height);
	_font->drawText(x,y,0,Rgba(255,255,255,255),text,0,0,0);
	_font->end();
	return ;
}
