/*
 * osd_text.cpp
 *
 *  Created on: 2018年5月23日
 *      Author: alex
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include "freetype.hpp"

FreeTypeFont*	_font;

void OSDCreatText()
{
	_font   =   new FreeTypeFont();
	_font->create("simsun.ttc",16,512,512);
}

void OSDdrawText()
{
	_font->begin(100,100);
	_font->drawText(0,0,0,Rgba(0,125,255,100),L"怎么搞啊",0,0,0);
	//_font->drawText(900,800,0,Rgba(255,255,255),L"一二三四五",0,0,0);
	_font->end();
	return ;
}
