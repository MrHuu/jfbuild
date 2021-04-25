// A.ASM replacement using C
// Mainly by Ken Silverman, with things melded with my port by
// Jonathon Fowler (jf@jonof.id.au)
//
// "Build Engine & Tools" Copyright (c) 1993-1997 Ken Silverman
// Ken Silverman's official web site: "http://www.advsys.net/ken"
// See the included license file "BUILDLIC.TXT" for license info.

#include "compat.h"
#include "a.h"

#ifdef ENGINE_USING_A_C

int krecip(int num);	// from engine.c

#define BITSOFPRECISION 3
#define BITSOFPRECISIONPOW 8

extern int asm1, asm2, asm4, fpuasm, globalx3, globaly3;
extern intptr_t asm3;
extern void *reciptable;

static int bpl, transmode = 0;
static int glogx, glogy, gbxinc, gbyinc, gpinc;
static unsigned char *gbuf, *gpal, *ghlinepal, *gtrans;

	//Global variable functions
void setvlinebpl(int dabpl) { bpl = dabpl; }
void fixtransluscence(void *datransoff) { gtrans = (unsigned char *)datransoff; }
void settransnormal(void) { transmode = 0; }
void settransreverse(void) { transmode = 1; }


	//Ceiling/floor horizontal line functions
void sethlinesizes(int logx, int logy, void *bufplc)
	{ glogx = logx; glogy = logy; gbuf = (unsigned char *)bufplc; }
void setpalookupaddress(void *paladdr) { ghlinepal = (unsigned char *)paladdr; }
void setuphlineasm4(int bxinc, int byinc) { gbxinc = bxinc; gbyinc = byinc; }
void hlineasm4(int cnt, int skiploadincs, int paloffs, unsigned int by, unsigned int bx, void *p)
{
	unsigned char *palptr, *pp;

	palptr = &ghlinepal[paloffs];
	pp = (unsigned char *)p;
	if (!skiploadincs) { gbxinc = asm1; gbyinc = asm2; }
	for(;cnt>=0;cnt--)
	{
		*pp = palptr[gbuf[((bx>>(32-glogx))<<glogy)+(by>>(32-glogy))]];
		bx -= gbxinc;
		by -= gbyinc;
		pp--;
	}
}


	//Sloped ceiling/floor vertical line functions
void setupslopevlin(int logylogx, void *bufplc, int pinc)
{
	glogx = (logylogx&255); glogy = (logylogx>>8);
	gbuf = (unsigned char *)bufplc; gpinc = pinc;
}
void slopevlin(void *p, int i, void *slopaloffs, int cnt, int bx, int by)
{
	intptr_t *slopalptr;
	int bz, bzinc;
	unsigned int u, v;
	unsigned char *pp;

	bz = asm3; bzinc = (asm1>>3);
	slopalptr = (intptr_t *)slopaloffs;
	pp = (unsigned char *)p;
	for(;cnt>0;cnt--)
	{
		i = krecip(bz>>6); bz += bzinc;
		u = bx+globalx3*i;
		v = by+globaly3*i;
		*pp = *(unsigned char *)(slopalptr[0]+gbuf[((u>>(32-glogx))<<glogy)+(v>>(32-glogy))]);
		slopalptr--;
		pp += gpinc;
	}
}


	//Wall,face sprite/wall sprite vertical line functions
void setupvlineasm(int neglogy) { glogy = neglogy; }
void vlineasm1(int vinc, void *paloffs, int cnt, unsigned int vplc, void *bufplc, void *p)
{
	unsigned char *pp;

	gbuf = (unsigned char *)bufplc;
	gpal = (unsigned char *)paloffs;
	pp = (unsigned char *)p;
	for(;cnt>=0;cnt--)
	{
		*pp = gpal[gbuf[vplc>>glogy]];
		pp += bpl;
		vplc += vinc;
	}
}

void setupmvlineasm(int neglogy) { glogy = neglogy; }
void mvlineasm1(int vinc, void *paloffs, int cnt, unsigned int vplc, void *bufplc, void *p)
{
	unsigned char ch, *pp;

	gbuf = (unsigned char *)bufplc;
	gpal = (unsigned char *)paloffs;
	pp = (unsigned char *)p;
	for(;cnt>=0;cnt--)
	{
		ch = gbuf[vplc>>glogy]; if (ch != 255) *pp = gpal[ch];
		pp += bpl;
		vplc += vinc;
	}
}

void setuptvlineasm(int neglogy) { glogy = neglogy; }
void tvlineasm1(int vinc, void *paloffs, int cnt, unsigned int vplc, void *bufplc, void *p)
{
	unsigned char ch, *pp;

	gbuf = (unsigned char *)bufplc;
	gpal = (unsigned char *)paloffs;
	pp = (unsigned char *)p;
	if (transmode)
	{
		for(;cnt>=0;cnt--)
		{
			ch = gbuf[vplc>>glogy];
			if (ch != 255) *pp = gtrans[(*pp)+(gpal[ch]<<8)];
			pp += bpl;
			vplc += vinc;
		}
	}
	else
	{
		for(;cnt>=0;cnt--)
		{
			ch = gbuf[vplc>>glogy];
			if (ch != 255) *pp = gtrans[((*pp)<<8)+gpal[ch]];
			pp += bpl;
			vplc += vinc;
		}
	}
}

	//Floor sprite horizontal line functions
void msethlineshift(int logx, int logy) { glogx = logx; glogy = logy; }
void mhline(void *bufplc, unsigned int bx, int cntup16, int UNUSED(junk), unsigned int by, void *p)
{
	unsigned char ch, *pp;

	gbuf = (unsigned char *)bufplc;
	gpal = (unsigned char *)asm3;
	pp = (unsigned char *)p;
	for(cntup16>>=16;cntup16>0;cntup16--)
	{
		ch = gbuf[((bx>>(32-glogx))<<glogy)+(by>>(32-glogy))];
		if (ch != 255) *pp = gpal[ch];
		bx += asm1;
		by += asm2;
		pp++;
	}
}

void tsethlineshift(int logx, int logy) { glogx = logx; glogy = logy; }
void thline(void *bufplc, unsigned int bx, int cntup16, int UNUSED(junk), unsigned int by, void *p)
{
	unsigned char ch, *pp;

	gbuf = (unsigned char *)bufplc;
	gpal = (unsigned char *)asm3;
	pp = (unsigned char *)p;
	if (transmode)
	{
		for(cntup16>>=16;cntup16>0;cntup16--)
		{
			ch = gbuf[((bx>>(32-glogx))<<glogy)+(by>>(32-glogy))];
			if (ch != 255) *pp = gtrans[(*pp)+(gpal[ch]<<8)];
			bx += asm1;
			by += asm2;
			pp++;
		}
	}
	else
	{
		for(cntup16>>=16;cntup16>0;cntup16--)
		{
			ch = gbuf[((bx>>(32-glogx))<<glogy)+(by>>(32-glogy))];
			if (ch != 255) *pp = gtrans[((*pp)<<8)+gpal[ch]];
			bx += asm1;
			by += asm2;
			pp++;
		}
	}
}


	//Rotatesprite vertical line functions
void setupspritevline(void *paloffs, int bxinc, int byinc, int ysiz)
{
	gpal = (unsigned char *)paloffs;
	gbxinc = bxinc;
	gbyinc = byinc;
	glogy = ysiz;
}
void spritevline(int bx, int by, int cnt, void *bufplc, void *p)
{
	unsigned char *pp;

	gbuf = (unsigned char *)bufplc;
	pp = (unsigned char *)p;
	for(;cnt>1;cnt--)
	{
		*pp = gpal[gbuf[(bx>>16)*glogy+(by>>16)]];
		bx += gbxinc;
		by += gbyinc;
		pp += bpl;
	}
}

	//Rotatesprite vertical line functions
void msetupspritevline(void *paloffs, int bxinc, int byinc, int ysiz)
{
	gpal = (unsigned char *)paloffs;
	gbxinc = bxinc;
	gbyinc = byinc;
	glogy = ysiz;
}
void mspritevline(int bx, int by, int cnt, void *bufplc, void *p)
{
	unsigned char ch, *pp;

	gbuf = (unsigned char *)bufplc;
	pp = (unsigned char *)p;
	for(;cnt>1;cnt--)
	{
		ch = gbuf[(bx>>16)*glogy+(by>>16)];
		if (ch != 255) *pp = gpal[ch];
		bx += gbxinc;
		by += gbyinc;
		pp += bpl;
	}
}

void tsetupspritevline(void *paloffs, int bxinc, int byinc, int ysiz)
{
	gpal = (unsigned char *)paloffs;
	gbxinc = bxinc;
	gbyinc = byinc;
	glogy = ysiz;
}
void tspritevline(int bx, int by, int cnt, void *bufplc, void *p)
{
	unsigned char ch, *pp;

	gbuf = (unsigned char *)bufplc;
	pp = (unsigned char *)p;
	if (transmode)
	{
		for(;cnt>1;cnt--)
		{
			ch = gbuf[(bx>>16)*glogy+(by>>16)];
			if (ch != 255) *pp = gtrans[(*pp)+(gpal[ch]<<8)];
			bx += gbxinc;
			by += gbyinc;
			pp += bpl;
		}
	}
	else
	{
		for(;cnt>1;cnt--)
		{
			ch = gbuf[(bx>>16)*glogy+(by>>16)];
			if (ch != 255) *pp = gtrans[((*pp)<<8)+gpal[ch]];
			bx += gbxinc;
			by += gbyinc;
			pp += bpl;
		}
	}
}

void setupdrawslab (int dabpl, void *pal)
	{ bpl = dabpl; gpal = (unsigned char *)pal; }

void drawslab (int dx, int v, int dy, int vi, void *vptr, void *p)
{
	int x;
	unsigned char *pp, *vpptr;
	
	pp = (unsigned char *)p;
	vpptr = (unsigned char *)vptr;
	while (dy > 0)
	{
		for(x=0;x<dx;x++) *(pp+x) = gpal[(int)(*(vpptr+(v>>16)))];
		pp += bpl; v += vi; dy--;
	}
}

void stretchhline (void * UNUSED(p0), int u, int cnt, int uinc, void *rptr, void *p)
{
	unsigned char *pp, *rpptr, *np;

	rpptr = (unsigned char *)rptr;
	pp = (unsigned char *)p;
	np = (unsigned char *)((intptr_t)p-(cnt<<2));
	do
	{
		pp--;
		*pp = *(rpptr+(u>>16)); u -= uinc;
	} while (pp > np);
}


void mmxoverlay() { }

#elif defined __AMIGA__

//
// Dante/Oxyron 2003
//

extern int fixchain;

/* #pragma aux setupspritevline parm [eax][ebx][ecx][edx][esi][edi] */
static char *spal_eax;
static int smach_eax;
static int smach2_eax;
static int smach5_eax;
static int smach_ecx;
void setupspritevline(void *i1, int i2, int i3, int i4, int i5, int i6)
{
    spal_eax = i1;
    smach_eax = (i5<<16);
    smach2_eax = (i5>>16)+i2;
    smach5_eax = smach2_eax + i4;
    smach_ecx = i3;
} /* setupspritevline */

/* #pragma aux spritevline parm [eax][ebx][ecx][edx][esi][edi] */
void spritevline(int i1, int i2, int i3, int i4, void *i5, void *i6)
{
    char *source = (char *)i5;
    char *dest = (char *)i6;

dumblabel1:
    i2 += smach_eax;
    i1 = (i1&0xffffff00) | (*source&0xff);
    if ((i2 - smach_eax) > i2) source += smach2_eax + 1;
    else source += smach2_eax;

dumblabel2:
    i1 = (i1&0xffffff00) | (spal_eax[i1]&0xff);
    *dest = i1;
    dest += fixchain;

    i4 += smach_ecx;
    i4--;
    if (!((i4 - smach_ecx) > i4) && i4 != 0)
	    goto dumblabel1;
    if (i4 == 0) return;

    i2 += smach_eax;
    i1 = (i1&0xffffff00) | (*source&0xff);
    if ((i2 - smach_eax) > i2) source += smach5_eax + 1;
    else source += smach5_eax;

    goto dumblabel2;
} /* spritevline */

/* #pragma aux msetupspritevline parm [eax][ebx][ecx][edx][esi][edi] */
static void *mspal_eax;
static int msmach_eax;
static int msmach2_eax;
static int msmach5_eax;
static int msmach_ecx;
void msetupspritevline(void *i1, int i2, int i3, int i4, int i5, int i6)
{
    mspal_eax = i1;
    msmach_eax = (i5<<16);
    msmach2_eax = (i5>>16)+i2;
    msmach5_eax = smach2_eax + i4;
    msmach_ecx = i3;
} /* msetupspritevline */

/* #pragma aux mspritevline parm [eax][ebx][ecx][edx][esi][edi] */
void mspritevline(int i1, int i2, int i3, int i4, void *i5, void *i6)
{
    char *source = (char *)i5;
    char *dest = (char *)i6;

msdumblabel1:
    i2 += smach_eax;
    i1 = (i1&0xffffff00) | (*source&0xff);
    if ((i2 - smach_eax) > i2) source += smach2_eax + 1;
    else source += smach2_eax;
msdumblabel2:
    if ((i1&0xff) != 255)
    {
	    i1 = (i1&0xffffff00) | (((char *)spal_eax)[i1]&0xff);
	    //*dest = i1;
	    *dest = 0x7f;
    }
    dest += fixchain;

    i4 += smach_ecx;
    i4--;
    if (!((i4 - smach_ecx) > i4) && i4 != 0)
	    goto msdumblabel1;
    if (i4 == 0) return;
    i2 += smach_eax;
    i1 = (i1&0xffffff00) | (*source&0xff);
    if ((i2 - smach_eax) > i2) source += smach5_eax + 1;
    else source += smach5_eax;
    goto msdumblabel2;
} /* mspritevline */

#endif
/*
 * vim:ts=4:
 */

