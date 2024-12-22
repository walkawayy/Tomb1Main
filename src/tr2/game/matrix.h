#pragma once

#include "global/types.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int32_t _00;
    int32_t _01;
    int32_t _02;
    int32_t _03;
    int32_t _10;
    int32_t _11;
    int32_t _12;
    int32_t _13;
    int32_t _20;
    int32_t _21;
    int32_t _22;
    int32_t _23;
} MATRIX;

extern MATRIX *g_MatrixPtr;
extern MATRIX *g_IMMatrixPtr;
extern MATRIX g_W2VMatrix;

void Matrix_ResetStack(void);
void __cdecl Matrix_GenerateW2V(const PHD_3DPOS *viewpos);

void __cdecl Matrix_Push(void);
void __cdecl Matrix_PushUnit(void);
void __cdecl Matrix_Pop(void);
void __cdecl Matrix_LookAt(
    int32_t xsrc, int32_t ysrc, int32_t zsrc, int32_t xtar, int32_t ytar,
    int32_t ztar, int16_t roll);
void __cdecl Matrix_RotX(int16_t rx);
void __cdecl Matrix_RotY(int16_t ry);
void __cdecl Matrix_RotZ(int16_t rz);
void __cdecl Matrix_RotYXZ(int16_t ry, int16_t rx, int16_t rz);
void __cdecl Matrix_RotYXZpack(uint32_t rpack);
void __cdecl Matrix_RotYXZsuperpack(const int16_t **pprot, int32_t index);
bool __cdecl Matrix_TranslateRel(int32_t x, int32_t y, int32_t z);
void __cdecl Matrix_TranslateAbs(int32_t x, int32_t y, int32_t z);
void Matrix_TranslateSet(int32_t x, int32_t y, int32_t z);

void __cdecl Matrix_InitInterpolate(int32_t frac, int32_t rate);
void __cdecl Matrix_Interpolate(void);
void __cdecl Matrix_InterpolateArm(void);
void __cdecl Matrix_Push_I(void);
void __cdecl Matrix_Pop_I(void);
void __cdecl Matrix_RotX_I(int16_t ang);
void __cdecl Matrix_RotY_I(int16_t ang);
void __cdecl Matrix_RotZ_I(int16_t ang);
void __cdecl Matrix_RotYXZ_I(int16_t y, int16_t x, int16_t z);
void __cdecl Matrix_RotYXZsuperpack_I(
    const int16_t **pprot1, const int16_t **pprot2, int32_t index);
void __cdecl Matrix_TranslateRel_I(int32_t x, int32_t y, int32_t z);
void __cdecl Matrix_TranslateRel_ID(
    int32_t x, int32_t y, int32_t z, int32_t x2, int32_t y2, int32_t z2);
