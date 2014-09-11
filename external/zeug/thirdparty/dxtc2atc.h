// DXTC to ATC
// based on
// A Method for Load-Time Conversion of DXTC Assets to ATC 
// by Ray Ratelis, John Bergman - Guild Software, Inc.

#ifndef INCLUDE_DXTC2ATC_H
#define INCLUDE_DXTC2ATC_H

void dxt2atc_convert_texture(void *data, int srcfmt, int destfmt, int w, int h);

#ifdef DXTC2ATC_IMPLEMENTATION

#include <stdlib.h>
#include <math.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define DXTC2ATC_DXT1 1
#define DXTC2ATC_DXT3 3
#define DXTC2ATC_DXT5 5

unsigned convertrgbbits(unsigned bits)
{
    unsigned lut[4] = {0,3,1,2};
    unsigned a = lut[bits & 0x03];
    unsigned b = lut[(bits>>2) & 0x03];
    unsigned c = lut[(bits>>4) & 0x03];
    unsigned d = lut[(bits>>6) & 0x03];
    return a | (b<<2) | (c<<4) | (d<<6);
}

#pragma pack(1)
struct alphablock
{
    unsigned char alpha0;
    unsigned char alpha1;
    unsigned char a[6];
};

struct DXT3alphablock
{
    unsigned char b_a;
    unsigned char d_c;
    unsigned char f_e;
    unsigned char h_g;
    unsigned char j_i;
    unsigned char l_k;
    unsigned char n_m;
    unsigned char p_o;
};

struct colorblock
{
    unsigned short color0;
    unsigned short color1;
    unsigned char d_c_b_a;
    unsigned char h_g_f_e;
    unsigned char l_k_j_i;
    unsigned char p_o_n_m;
};

struct DXT3block
{
    struct DXT3alphablock alpha;
    struct colorblock color;
};

struct DXT5block
{
    struct alphablock alpha;
    struct colorblock color;
};
#pragma pack()

void convertblock(struct colorblock *block)
{
    unsigned color0b = (block->color0&0x01f);
    unsigned color0g = (block->color0&0x07C0)>>1;
    unsigned color0r = (block->color0&0xF800)>>1;
    block->color0 = color0r | color0g | color0b;
    block->d_c_b_a = convertrgbbits(block->d_c_b_a);
    block->h_g_f_e = convertrgbbits(block->h_g_f_e);
    block->l_k_j_i = convertrgbbits(block->l_k_j_i);
    block->p_o_n_m = convertrgbbits(block->p_o_n_m);
}

void dxt2atc_convert_texture(void *data, int srcfmt, int destfmt, int w, int h)
{
    if(srcfmt == DXTC2ATC_DXT1 && destfmt == GL_ATC_RGB_AMD)
    {
        int s = fmax(1, w / 4) * fmax(1, h / 4);
        struct colorblock *blocks = (struct colorblock*)data;
        for(int i=0;i<s;i++)
        {
            convertblock(blocks+i);
        }
    }
    else if(srcfmt == DXTC2ATC_DXT3 && destfmt == GL_ATC_RGBA_EXPLICIT_ALPHA_AMD)
    {
        int s = fmax(1, w / 4) * fmax(1, h / 4);
        struct DXT3block *blocks = (struct DXT3block*)data;
        for(int i=0;i<s;i++)
        {
            convertblock(&(blocks[i].color));
        }
    }
    else if(srcfmt == DXTC2ATC_DXT5 && destfmt== GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD)
    {
        int s = fmax(1, w / 4) * fmax(1, h / 4);
        struct DXT5block *blocks = (struct DXT5block*)data;
        for(int i=0;i<s;i++)
        {
            convertblock(&(blocks[i].color));
        }
    }
}
#endif // DXTC2ATC_IMPLEMENTATION

#endif // INCLUDE_DXTC2ATC_H