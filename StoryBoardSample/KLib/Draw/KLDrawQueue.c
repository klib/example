//
//  KLDrawQueue.c
//  KLib
//
// Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//
// KLib MIT Lisence
//#####################################################################
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
// 以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を取得する
// すべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
// これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、
// および/または販売する権利、およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
// 上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
// ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
// ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、
// それに限定されるものではありません。
// 作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
// あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について
// 何らの責任も負わないものとします。
//
//#####################################################################

#include "KLDrawQueue.h"
#include "KLSprite.h"
#include "KLGL.h"
#include "KLib.h"

static KLDrawQueue	kl_drawqueue;
static KLSprite		kl_share_sprite;

//================================================================
/**
 *	@brief	描画キューの解放処理
 */
//================================================================
static void KLDrawQueue_Free(){

	KLDrawQueue* p = &kl_drawqueue;
	
	if( p->iboSquare )
	{
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboSquare);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &p->iboSquare);
		p->iboSquare = 0;
	}

	if( p->iboCircle )
	{
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboCircle);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &p->iboCircle);
		p->iboCircle = 0;
	}
	
	if( p->iboLine )
	{
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboLine);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &p->iboLine);
		p->iboLine = 0;
	}
	
	glsDisableVertexAttribArray(&klib.shader.pos);
	glsDisableVertexAttribArray(&klib.shader.color);
	glsDisableVertexAttribArray(&klib.shader.colormul);
	glsDisableVertexAttribArray(&klib.shader.uv);

#ifdef KLGL_SHORTTYPEUV
	glsVertexAttribPointer( &klib.shader.uv,	KL_AXIS_SIZE_XY,	GL_UNSIGNED_SHORT,			GL_FALSE, 0, 0 );
#else
	glsVertexAttribPointer( &klib.shader.uv,	KL_AXIS_SIZE_XY,	GL_FLOAT,			GL_FALSE, 0, 0 );
#endif
	glsVertexAttribPointer( &klib.shader.color, KL_COLOR_SIZE_RGBA,	GL_UNSIGNED_BYTE,	GL_FALSE, 0, 0 );
	glsVertexAttribPointer( &klib.shader.colormul, KL_COLOR_SIZE_RGBA,	GL_UNSIGNED_BYTE,	GL_FALSE, 0, 0 );
	glsVertexAttribPointer( &klib.shader.pos, KL_AXIS_SIZE_XYZ,	GL_FLOAT,			GL_FALSE, 0, 0 );
	
	sfree(kl_drawqueue.pVertex);
	sfree(kl_drawqueue.pIndexes);
	sfree(kl_drawqueue.pIndexesCircle);
	sfree(kl_drawqueue.pIndexesLine);
	sfree(kl_drawqueue.pColor);
	sfree(kl_drawqueue.pColorRate);
	sfree(kl_drawqueue.pUv);
	sfree(kl_drawqueue.pv);
	
	KLSprite_UnloadWithTexture(&p->minimumsp);
	p->loaded = FALSE;

}

//================================================================
/**
 *	@brief	描画キューの終了処理
 */
//================================================================
void KLDrawQueue_Unload(){

	KLDrawQueue_Free();
	KLLog ("[KLDrawQueue] unloaded.\n");
	
}

//================================================================
/**
 *	@brief	テクスチャなしスプライト用のテクスチャIDを返す
 *
 *	@return	KLDrawQueue が初期化済みならテクスチャID / それ以外は未定義ID
 */
//================================================================
GLuint KLDrawQueue_GetMinimumTexId() {
	
	KLDrawQueue* p = &kl_drawqueue;
	
	return p->loaded ? p->minimumsp.tid : KLGL_UNBINDED_TEXID;
	
}

//================================================================
/**
 *	@brief	描画キューの初期化
 *
 *	@param	nMax	最大キュー数 / 0でKLibデフォルト値を設定
 *
 *	@return	正常終了時にTRUE / 失敗時にFALSE
 */
//================================================================
bl KLDrawQueue_Init( u32 nMax ){

	KLDrawQueue* p = &kl_drawqueue;
	
	klmemclear( p, KLDrawQueue );
	
	if( sizeof(KLVertex)%4 )
	{
		// 4byteアラインしていないと警告
		KLLog("[KLdrawQueue] Vertex align failed:%ld\n", sizeof(KLVertex) );
	}
	
	if( p->nMax==0 )
	{
		p->nMax = 5120;
	}
	
	p->loaded		= FALSE;
	KLSprite_Init( &p->minimumsp );
	p->nVertexPrim	= KLDRAWQUEUE_PRIM_VERTEXNUM;
	p->nIndexPrim	= KLDRAWQUEUE_PRIM_INDEXNUM;
	
	p->pVertex 			= (GLfloat*) Calloc( sizeof(GLfloat) * p->nVertexPrim* p->nMax * KL_AXIS_SIZE_XYZ );
	p->pIndexes			= (GLushort*)Calloc( sizeof(GLushort)* p->nIndexPrim * p->nMax );
	p->pIndexesCircle	= (GLushort*)Calloc( sizeof(GLushort)* 3 * p->nMax * 360 );
	p->pIndexesLine		= (GLushort*)Calloc( sizeof(GLushort)* 2 * p->nMax );
	p->pColor			= (GLubyte*) Calloc( sizeof(GLubyte) * p->nVertexPrim* p->nMax * KL_COLOR_SIZE_RGBA  );
	p->pColorRate		= (GLubyte*) Calloc( sizeof(GLubyte) * p->nVertexPrim* p->nMax * KL_COLOR_SIZE_RGBA  );
#ifdef KLGL_SHORTTYPEUV
	p->pUv				= (GLushort*)Calloc( sizeof(GLushort) * p->nVertexPrim* p->nMax * KL_AXIS_SIZE_XYZ  );
#else
	p->pUv				= (GLfloat*)Calloc( sizeof(GLfloat) * p->nVertexPrim* p->nMax * KL_AXIS_SIZE_XYZ  );
#endif
	p->pv				= (KLVertex*)Calloc( sizeof(KLVertex) * p->nMax );
	
	if( !p->pv || !p->pVertex || !p->pIndexes || !p->pIndexesCircle || !p->pIndexesLine || !p->pColor || !p->pUv )
	{
		// mem error
		KLDrawQueue_Free();
		return 0;
	}
	
	if( klib.shader.program.handler==0 ){
		
		KLLog("[ERR][KLDrawQueue] handler is 0.\n");
		
	}
	
	// Vertex Attrib setup
#ifdef KLGL_SHORTTYPEUV
	glsVertexAttribPointer( &klib.shader.uv,		KL_AXIS_SIZE_XY,	GL_UNSIGNED_SHORT,	GL_FALSE, 0, p->pUv);
#else
	glsVertexAttribPointer( &klib.shader.uv,		KL_AXIS_SIZE_XY,	GL_FLOAT,	GL_FALSE, 0, p->pUv);
#endif
	glsVertexAttribPointer( &klib.shader.color,		KL_COLOR_SIZE_RGBA,	GL_UNSIGNED_BYTE,	GL_FALSE, 0, p->pColor);
	glsVertexAttribPointer( &klib.shader.colormul,	KL_COLOR_SIZE_RGBA,	GL_UNSIGNED_BYTE,	GL_FALSE, 0, p->pColorRate);
	glsVertexAttribPointer( &klib.shader.pos,		KL_AXIS_SIZE_XYZ,	GL_FLOAT,	GL_FALSE, 0, p->pVertex);
	
	// Index buffer object setup
	for( int o=0; o<p->nMax; ++o ){
		
		int c = o*4;
		int i = o*p->nIndexPrim;
		int i2= o*3;
		
		// Rect & Sprite
		p->pIndexes[i]		= 0+c;
		p->pIndexes[i+1]	= 1+c;
		p->pIndexes[i+2]	= 2+c;
		p->pIndexes[i+3]	= 3+c;
		p->pIndexes[i+4]	= 2+c;
		p->pIndexes[i+5]	= 1+c;
		
		// Line
		p->pIndexesLine[o*2+0] = o*2+0;
		p->pIndexesLine[o*2+1] = o*2+1;
		
		// Circle
		p->pIndexesCircle[i2]	= 0+i2;
		p->pIndexesCircle[i2+1]	= 1+i2;
		p->pIndexesCircle[i2+2]	= 2+i2;

	}
	
	// Bind IBO
	glGenBuffers(1, &p->iboSquare);
	glGenBuffers(1, &p->iboCircle);
	glGenBuffers(1, &p->iboLine);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboSquare);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)* p->nMax * p->nIndexPrim, p->pIndexes, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboCircle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)* 3 * p->nMax * 360, p->pIndexesCircle, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboLine);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)* p->nMax * 2, p->pIndexesLine, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	
	
	KLSprite_LoadFile( &p->minimumsp, "kl_texture.png");
	
	p->loaded = TRUE;
	
	return 1;
}

//================================================================
/**
 *	@brief	次から描画するオブジェクトに対してのブレンドを設定
 *
 *	@param	blendSrc	ブレンドの設定1
 *	@param	blendDest	ブレンドの設定2
 */
//================================================================
void KLDrawQueue_SetBlend( GLuint blendSrc, GLuint blendDest ){
	
	KLDrawQueue* p = &kl_drawqueue;
	
	p->isForceUpdate= (p->blend[0]!=blendSrc) || (p->blend[1]!=blendDest);
	
	p->blend[0]		= blendSrc;
	p->blend[1]		= blendDest;
	
}


//================================================================
/**
 *	@brief	ブレンド設定をデフォルトに戻す
 */
//================================================================
void KLDrawQueue_ResetBlend(){
	
	KLDrawQueue* p = &kl_drawqueue;
	
	p->blend[0]		= GL_SRC_ALPHA;
	p->blend[1]		= GL_ONE_MINUS_SRC_ALPHA;
	
}


//================================================================
/**
 *	@brief	描画キューをクリアする
 */
//================================================================
void KLDrawQueue_Clear(){
	
	KLDrawQueue* p = &kl_drawqueue;
	
	p->offset	= 0;
	p->tid		= KLGL_UNBINDED_TEXID;
	p->type		= KLPRIM_UNDEFINED;
	p->wLine	= 1.0f;
	p->isForceUpdate = 0;
	//p->clipz	= 0;
	
}


//================================================================
/**
 *	@brief	現在キューに溜まっているオブジェクトを実際に描画する
 */
//================================================================
void KLDrawQueue_Flush(){
	
	KLDrawQueue* p = &kl_drawqueue;
	KLGLFlags* pFlags = KLGL_GetFlags();
	
	pFlags->forceReflesh = 0;
	
	if( p->offset==0 )
	{
		return;
	}
	
	switch( p->type )
	{
		case KLPRIM_LINE:
		{
			glsBindTexture(KLGL_UNBINDED_TEXID);
			
			glLineWidth(p->wLine);
			
			glsEnableVertexAttribArray(&klib.shader.pos);
			glsEnableVertexAttribArray(&klib.shader.color);
			glsEnableVertexAttribArray(&klib.shader.colormul);
			glsDisableVertexAttribArray(&klib.shader.uv);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboLine);
			glDrawElements( GL_LINE_STRIP, p->offset, GL_UNSIGNED_SHORT, 0 );
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		break;
			
		case KLPRIM_SQUARE:
		{
			
			glsBindTexture( p->tid );
			KLGL_CheckError();
			
			if(p->tid!=0)
			{
				
				glsEnableVertexAttribArray(&klib.shader.uv);
				KLGL_CheckError();
				
#ifdef KLGL_SHORTTYPEUV
				glsVertexAttribPointer( &klib.shader.uv, KL_AXIS_SIZE_XY, GL_UNSIGNED_SHORT, GL_FALSE, 0, p->pUv);
#else
				glsVertexAttribPointer( &klib.shader.uv, KL_AXIS_SIZE_XY, GL_FLOAT, GL_FALSE, 0, p->pUv);
#endif
				KLGL_CheckError();
				
			}
			else
			{
				glsDisableVertexAttribArray(&klib.shader.uv);
				KLGL_CheckError();
				
				glsDisable(GL_TEXTURE_2D);
				KLGL_CheckError();
			}
			
			glsEnableVertexAttribArray(&klib.shader.pos);
			KLGL_CheckError();
			
			glsEnableVertexAttribArray(&klib.shader.color);
			KLGL_CheckError();
			
			glsEnableVertexAttribArray(&klib.shader.colormul);
			KLGL_CheckError();
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboSquare);
			KLGL_CheckError();
			
			glDrawElements( GL_TRIANGLES, p->offset*6, GL_UNSIGNED_SHORT, 0 );
			KLGL_CheckError();
			/*
			if(p->tid!=0)
			{
				
				glsDisableVertexAttribArray(&klib.shader.uv);
				KLGL_CheckError();
			
			}
			
			
			glsDisableVertexAttribArray(&klib.shader.pos);
			KLGL_CheckError();
			
			glsDisableVertexAttribArray(&klib.shader.color);
			KLGL_CheckError();
			
			glsDisableVertexAttribArray(&klib.shader.colormul);
			KLGL_CheckError();
			*/
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			KLGL_CheckError();
			
		}
		break;
			
		case KLPRIM_CIRCLE:
		{
			if(p->tid!=0)
			{
				glsBindTexture( p->tid );
				glsEnableVertexAttribArray(&klib.shader.uv);
			}
			else
			{
				glsDisableVertexAttribArray(&klib.shader.uv);
			}
			
			glsEnableVertexAttribArray(&klib.shader.pos);
			glsEnableVertexAttribArray(&klib.shader.color);
			glsEnableVertexAttribArray(&klib.shader.colormul);
			
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p->iboCircle);
			glDrawElements( GL_TRIANGLES, p->offset*3, GL_UNSIGNED_SHORT, 0 );
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		break;
			
		default:
		{
		
		}
		break;
			
	}
	
}


//================================================================
/**
 *	@brief	描画キューに指定スプライトを追加
 *
 *	@param	pSprite	追加するスプライトのポインタ
 */
//================================================================
static void KLDrawQueue_AddSpriteNoIdx( KLSprite* pSprite ){
	
	KLDrawQueue*	pQueue	= &kl_drawqueue;
	u32				i				= pQueue->offset==1 ?
										4 * KL_AXIS_SIZE_XYZ :
										4 * KL_AXIS_SIZE_XYZ + pQueue->offset * 6 * KL_AXIS_SIZE_XYZ;
	u32				c				= 0;
	GLfloat			l = 0, r = 0, t = 0, b = 0;
	
	if( pQueue->offset>=pQueue->nMax )
	{
		return;
	}
	
	l = -pSprite->cx;
	r = l + pSprite->w;
	t = -pSprite->cy;
	b = t + pSprite->h;
	
	pSprite->z = pQueue->clipz;
	
	//(変換後のx) = (変換前のx)×cosθ - (変換前のy)×sinθ + tx
	//(変換後のy) = (変換前のx)×sinθ + (変換前のy)×cosθ + ty
	u16		rot	= pSprite->rz;
	float	cosv= KL_Cos(rot);
	float 	sinv= KL_Sin(rot);
	
	// 2つ目以降は前ストリップ終端と今回の始点を二重化して縮退させる
	if(pQueue->offset)
	{
		pQueue->pVertex[i] = pQueue->pVertex[i-3];
		++i;
		pQueue->pVertex[i] = pQueue->pVertex[i-3];
		++i;
		pQueue->pVertex[i] = pQueue->pVertex[i-3];
		++i;
	}
	
	pQueue->pVertex[i++] = l*cosv - t*sinv + pSprite->x;
	pQueue->pVertex[i++] = l*sinv + t*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	if(pQueue->offset)
	{
		pQueue->pVertex[i] = pQueue->pVertex[i-3];
		++i;
		pQueue->pVertex[i] = pQueue->pVertex[i-3];
		++i;
		pQueue->pVertex[i] = pQueue->pVertex[i-3];
		++i;
	}
	
	pQueue->pVertex[i++] = l*cosv - b*sinv + pSprite->x;
	pQueue->pVertex[i++] = l*sinv + b*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	pQueue->pVertex[i++] = r*cosv - t*sinv + pSprite->x;
	pQueue->pVertex[i++] = r*sinv + t*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	pQueue->pVertex[i++] = r*cosv - b*sinv + pSprite->x;
	pQueue->pVertex[i++] = r*sinv + b*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	/*
	i = pQueue->offset*6;
	c = pQueue->offset*4;
	
	// Index
	pQueue->pIndexes[i++] = 0+c;
	pQueue->pIndexes[i++] = 1+c;
	pQueue->pIndexes[i++] = 2+c;
	pQueue->pIndexes[i++] = 3+c;
	pQueue->pIndexes[i++] = 2+c;
	pQueue->pIndexes[i++] = 1+c;
	*/
	
	i = pQueue->offset==1? 4*4 : 4*4+pQueue->offset*6*4;
	
	// Color(rate)
	
	if(pQueue->offset)
	{
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
	}
	
	// LT
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].a;
	
	
	if(pQueue->offset)
	{
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
		pQueue->pColorRate[i] = pQueue->pColorRate[i-4];
		++i;
	}

	
	// LB
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].a;
	
	// RT
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].a;
	
	// RB
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].a;
	
	i = pQueue->offset==1? 4*4 : 4*4+pQueue->offset*6*4;

	// Color(add)
		
	if(pQueue->offset)
	{
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
	}

	// LT
	pQueue->pColor[i++] = pSprite->rgba[0].r;
	pQueue->pColor[i++] = pSprite->rgba[0].g;
	pQueue->pColor[i++] = pSprite->rgba[0].b;
	pQueue->pColor[i++] = pSprite->rgba[0].a;
	
	if(pQueue->offset)
	{
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
		pQueue->pColor[i] = pQueue->pColor[i-4];
		++i;
	}
	
	// LB
	pQueue->pColor[i++] = pSprite->rgba[1].r;
	pQueue->pColor[i++] = pSprite->rgba[1].g;
	pQueue->pColor[i++] = pSprite->rgba[1].b;
	pQueue->pColor[i++] = pSprite->rgba[1].a;
	
	// RT
	pQueue->pColor[i++] = pSprite->rgba[2].r;
	pQueue->pColor[i++] = pSprite->rgba[2].g;
	pQueue->pColor[i++] = pSprite->rgba[2].b;
	pQueue->pColor[i++] = pSprite->rgba[2].a;
	
	// RB
	pQueue->pColor[i++] = pSprite->rgba[3].r;
	pQueue->pColor[i++] = pSprite->rgba[3].g;
	pQueue->pColor[i++] = pSprite->rgba[3].b;
	pQueue->pColor[i++] = pSprite->rgba[3].a;
	
	
	i = pQueue->offset==1? 2*4 : 2*4+pQueue->offset*2*6;
	c = 0;
	
	// UV
#ifdef KLGL_SHORTTYPEUV
	if(pQueue->offset)
	{
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
	}
	
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];

	if(pQueue->offset)
	{
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
	}

	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
#else
	if(pQueue->offset)
	{
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
	}
	
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	
	if(pQueue->offset)
	{
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
		pQueue->pUv[i] = pQueue->pUv[i-2];
		++i;
	}
	
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
#endif
	
	pQueue->offset++;
	
}


static void KLDrawQueue_AddSprite( KLSprite* pSprite ){
	
	KLDrawQueue*	pQueue	= &kl_drawqueue;
	u32				i				= pQueue->offset * 4 * KL_AXIS_SIZE_XYZ;
	u32				c				= 0;
	GLfloat			l = 0, r = 0, t = 0, b = 0;
	
	if( pQueue->offset>=pQueue->nMax )
	{
		return;
	}
	
	l = -pSprite->cx;
	r = l + pSprite->w;
	t = -pSprite->cy;
	b = t + pSprite->h;
	
	pSprite->z = pQueue->clipz;
	
	//(変換後のx) = (変換前のx)×cosθ - (変換前のy)×sinθ + tx
	//(変換後のy) = (変換前のx)×sinθ + (変換前のy)×cosθ + ty
	u16		rot	= pSprite->rz;
	float	cosv= KL_Cos(rot);
	float 	sinv= KL_Sin(rot);
	
	pQueue->pVertex[i++] = l*cosv - t*sinv + pSprite->x;
	pQueue->pVertex[i++] = l*sinv + t*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	pQueue->pVertex[i++] = l*cosv - b*sinv + pSprite->x;
	pQueue->pVertex[i++] = l*sinv + b*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	pQueue->pVertex[i++] = r*cosv - t*sinv + pSprite->x;
	pQueue->pVertex[i++] = r*sinv + t*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;
	
	pQueue->pVertex[i++] = r*cosv - b*sinv + pSprite->x;
	pQueue->pVertex[i++] = r*sinv + b*cosv + pSprite->y;
	pQueue->pVertex[i++] = pSprite->z;

	// 奇数テクスチャUV幅の際のピクセルブレを回避するため、無回転時の小数座標を補正する
	
	// X
	if ( pQueue->pVertex[0]==pQueue->pVertex[3] && pQueue->pVertex[6]==pQueue->pVertex[9] ) {
		
		// 小数部の取得
		double dummy	= 0;
		double d		= modf( pQueue->pVertex[0], &dummy );
		
		// 左辺の補正
		pQueue->pVertex[0] -= d;
		pQueue->pVertex[3] -= d;
		
		// 右辺の補正
		pQueue->pVertex[6] -= d;
		pQueue->pVertex[9] -= d;
		pQueue->pVertex[6] = (s64)(pQueue->pVertex[6] + 0.5f);
		pQueue->pVertex[9] = (s64)(pQueue->pVertex[9] + 0.5f);
		
	}
	
	// Y
	if ( pQueue->pVertex[1]==pQueue->pVertex[4] && pQueue->pVertex[7]==pQueue->pVertex[10] ) {
		
		// 小数部の取得
		double dummy	= 0;
		double d		= modf( pQueue->pVertex[1], &dummy );
		
		// 左辺の補正
		pQueue->pVertex[1] -= d;
		pQueue->pVertex[4] -= d;
		
		// 右辺の補正
		pQueue->pVertex[7] -= d;
		pQueue->pVertex[10] -= d;
		pQueue->pVertex[7] = (s64)(pQueue->pVertex[7] + 0.5f);
		pQueue->pVertex[10] = (s64)(pQueue->pVertex[10] + 0.5f);
		
	}
	
	
	i = pQueue->offset*6;
	c = pQueue->offset*4;
	
	// Index
	pQueue->pIndexes[i++] = 0+c;
	pQueue->pIndexes[i++] = 1+c;
	pQueue->pIndexes[i++] = 2+c;
	pQueue->pIndexes[i++] = 3+c;
	pQueue->pIndexes[i++] = 2+c;
	pQueue->pIndexes[i++] = 1+c;
	i = pQueue->offset*4*4;
	
	// Color(rate)

	// LT
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[0].a;
	
	// LB
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[1].a;
	
	// RT
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[2].a;
	
	// RB
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].r;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].g;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].b;
	pQueue->pColorRate[i++] = pSprite->rgbaRate[3].a;
	i = pQueue->offset*4*4;
	
	// Color(add)
	
	// LT
	pQueue->pColor[i++] = pSprite->rgba[0].r;
	pQueue->pColor[i++] = pSprite->rgba[0].g;
	pQueue->pColor[i++] = pSprite->rgba[0].b;
	pQueue->pColor[i++] = pSprite->rgba[0].a;
	
	// LB
	pQueue->pColor[i++] = pSprite->rgba[1].r;
	pQueue->pColor[i++] = pSprite->rgba[1].g;
	pQueue->pColor[i++] = pSprite->rgba[1].b;
	pQueue->pColor[i++] = pSprite->rgba[1].a;
	
	// RT
	pQueue->pColor[i++] = pSprite->rgba[2].r;
	pQueue->pColor[i++] = pSprite->rgba[2].g;
	pQueue->pColor[i++] = pSprite->rgba[2].b;
	pQueue->pColor[i++] = pSprite->rgba[2].a;
	
	// RB
	pQueue->pColor[i++] = pSprite->rgba[3].r;
	pQueue->pColor[i++] = pSprite->rgba[3].g;
	pQueue->pColor[i++] = pSprite->rgba[3].b;
	pQueue->pColor[i++] = pSprite->rgba[3].a;

	
	i = pQueue->offset*4*2;
	c = 0;
	
	// UV
#ifdef KLGL_SHORTTYPEUV
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
	pQueue->pUv[i++] = 0xFFFF * pSprite->uv[c++];
#else
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
	pQueue->pUv[i++] = pSprite->uv[c++];
#endif
	pQueue->offset++;
	
}


//================================================================
/**
 *	@brief	対象をキューに追加する場合に<br/>
 *			描画キューの更新が必要かチェックし<br/>
 *			必要なら保持しているキューを反映してからリセットする
 *
 *	@param	type	プリミティブタイプ
 *	@param	pSprite	スプライトタイプならそのポインタ / それ以外はNULL
 *	@param	line_w	ラインタイプならその太さ / それ以外は0
 *
 *	@return	キューを消化したらTRUE / 消化不要で継続できるなたFALSE
 */
//================================================================
bl KLDrawQueue_Check( KLPrimitiveType type, KLSprite* pSprite, float line_w ){
	
	KLDrawQueue*	p		= &kl_drawqueue;
	KLGLFlags*		pFlags	= KLGL_GetFlags();
	
	// ブレンドかタイプかテクスチャIDが変わったら現在溜まってるキューを反映
	if( pFlags->forceReflesh ||
	    ( p->offset && p->isForceUpdate ) ||
		( pSprite && pSprite->tid != p->tid ) ||
		( p->type!=type ) ||
		( p->type == KLPRIM_LINE && line_w != p->wLine ) )
	{
		
		KLDrawQueue_Flush();
		KLDrawQueue_Clear();
		
		// TODO: ループ最後のクリア後はブレンドもクリアする？
		p->isForceUpdate = FALSE;
		
		return TRUE;
		
	}
	
	return FALSE;
}


//================================================================
/**
 *	@brief	スプライトを描画キューに追加する
 *
 *	@param	pSprite	スプライトのポインタ
 */
//================================================================
void KLDrawQueue_DrawSprite( KLSprite* pSprite ){
	
	KLDrawQueue* p = &kl_drawqueue;
	
	if(pSprite)
	{
		// ブレンドかプリミティブかテクスチャIDが変わったら現在溜まってるキューを反映
		KLDrawQueue_Check( KLPRIM_SQUARE, pSprite, 0 );
		KLDrawQueue_AddSprite( pSprite );
		p->tid				= pSprite->tid;
		p->type				= KLPRIM_SQUARE;
		p->isForceUpdate	= FALSE;
	}
	
}

void KL_DrawGradation( float x, float y, float w, float h, Rgba topL, Rgba bottomL, Rgba topR, Rgba bottomR ){
	
	kl_share_sprite.z = kl_drawqueue.clipz;
	KLSprite_Init(		&kl_share_sprite );
	KLSprite_SetWH(		&kl_share_sprite, w, h );
	KLSprite_SetXY(		&kl_share_sprite, x, y );
	KLSprite_SetTexId(	&kl_share_sprite, 0 );
	KLSprite_SetGrad(	&kl_share_sprite, topL, bottomL, topR, bottomR );
	KLSprite_Draw(		&kl_share_sprite );
	
}

void KL_DrawGradationX( float x, float y, float w, float h, Rgba rgba1, Rgba rgba2 ){
	
	kl_share_sprite.z = kl_drawqueue.clipz;
	KLSprite_Init(		&kl_share_sprite );
	KLSprite_SetWH(		&kl_share_sprite, w, h );
	KLSprite_SetXY(		&kl_share_sprite, x, y );
	KLSprite_SetTexId(	&kl_share_sprite, 0 );
	KLSprite_SetGradX(	&kl_share_sprite, rgba1, rgba2 );
	KLSprite_Draw(		&kl_share_sprite );
	
}

void KL_DrawGradationY( float x, float y, float w, float h, Rgba rgba1, Rgba rgba2 ){
	
	kl_share_sprite.z = kl_drawqueue.clipz;
	KLSprite_Init(		&kl_share_sprite );
	KLSprite_SetWH(		&kl_share_sprite, w, h );
	KLSprite_SetXY(		&kl_share_sprite, x, y );
	KLSprite_SetTexId(	&kl_share_sprite, 0 );
	KLSprite_SetGradY(	&kl_share_sprite, rgba1, rgba2 );
	KLSprite_Draw(		&kl_share_sprite );
	
}

void KL_DrawLine( KLPos3d* pPosAry, Rgba commonRgba, u32 nAry, u32 linew ){
	
	KLDrawQueue*	pQueue	= &kl_drawqueue;
	
	if( pQueue->offset+nAry>=pQueue->nMax )
	{
		return;
	}
	
	// 線以外を描画していたら一度キューを消化する
	KLDrawQueue_Check(KLPRIM_LINE, NULL, 0);
	
	for( int i=0; i<nAry; ++i )
	{
		pQueue->pVertex[i*3+0] = pPosAry[i].x;
		pQueue->pVertex[i*3+1] = pPosAry[i].y;
		pQueue->pVertex[i*3+2] = pQueue->clipz;//pPosAry[i].z;
		
		pQueue->pColor[i*4+0] = commonRgba.r;
		pQueue->pColor[i*4+1] = commonRgba.g;
		pQueue->pColor[i*4+2] = commonRgba.b;
		pQueue->pColor[i*4+3] = commonRgba.a;
		
		pQueue->pColorRate[i*4+0] = 0;
		pQueue->pColorRate[i*4+1] = 0;
		pQueue->pColorRate[i*4+2] = 0;
		pQueue->pColorRate[i*4+3] = 0;
		
		pQueue->offset++;
	}
	
	pQueue->wLine	= linew;
	pQueue->type	= KLPRIM_LINE;
	
}

void KL_DrawLineMultiColor( KLPos3d* pPosAry, Rgba* pRgbaAry, u32 nAry, u32 linew ){

	KLDrawQueue*	pQueue	= &kl_drawqueue;
	
	if( pQueue->offset+nAry>=pQueue->nMax )
	{
		return;
	}
	
	// 線以外を描画していたら一度キューを消化する
	KLDrawQueue_Check(KLPRIM_LINE, NULL, 0);
	
	for( int i=0; i<nAry; ++i )
	{
		pQueue->pVertex[i*3+0] = pPosAry[i].x;
		pQueue->pVertex[i*3+1] = pPosAry[i].y;
		pQueue->pVertex[i*3+2] = pQueue->clipz;//pPosAry[i].z;
		
		pQueue->pColor[i*4+0] = pRgbaAry[i].r;
		pQueue->pColor[i*4+1] = pRgbaAry[i].g;
		pQueue->pColor[i*4+2] = pRgbaAry[i].b;
		pQueue->pColor[i*4+3] = pRgbaAry[i].a;
		
		pQueue->pColorRate[i*4+0] = 0;
		pQueue->pColorRate[i*4+1] = 0;
		pQueue->pColorRate[i*4+2] = 0;
		pQueue->pColorRate[i*4+3] = 0;
		
		pQueue->offset++;
	}
	
	pQueue->wLine = linew;
	pQueue->type	= KLPRIM_LINE;
	
}

void KL_DrawRect( float x, float y, float w, float h, Rgba rgba ){
	
	KLDrawQueue*	p	= &kl_drawqueue;
	
	kl_share_sprite.z = kl_drawqueue.clipz;
	KLSprite_Init(		&kl_share_sprite );
	KLSprite_SetWH(		&kl_share_sprite, w, h );
	KLSprite_SetXY( 	&kl_share_sprite, x, y );
	KLSprite_SetTexId(	&kl_share_sprite, p->minimumsp.tid );
	KLSprite_SetRgba(	&kl_share_sprite, 0,0,0,0);//rgba.a );
	KLSprite_SetRgbaAdd(&kl_share_sprite, rgba.r, rgba.g, rgba.b, rgba.a );
	KLSprite_Draw(		&kl_share_sprite );
	
}

void KL_DrawRectRot( float x, float y, float w, float h, Rgba rgba, float cx, float cy, wdeg rot ){
	
	KLDrawQueue*	p	= &kl_drawqueue;
	
	kl_share_sprite.z = kl_drawqueue.clipz;
	KLSprite_Init(		&kl_share_sprite );
	KLSprite_SetWH(		&kl_share_sprite, w, h );
	KLSprite_SetXY( 	&kl_share_sprite, x, y );
	KLSprite_SetTexId(	&kl_share_sprite, p->minimumsp.tid );
	KLSprite_SetRgba(	&kl_share_sprite, 0,0,0,0 );
	KLSprite_SetRgbaAdd(&kl_share_sprite, rgba.r, rgba.g, rgba.b, rgba.a );
	KLSprite_SetCenter( &kl_share_sprite, cx, cy );
	KLSprite_SetRot(	&kl_share_sprite, rot );
	KLSprite_Draw(		&kl_share_sprite );
	
}

void KL_DrawRectScreen( Rgba rgba ){
	
	KL_DrawRect( 0, 0, klib.view.w, klib.view.h, rgba );
	
}


void KL_DrawPolygon( float cx, float cy, float r, u16 nTerm, u16 rot, Rgba rgba ){
	
	KLDrawQueue*	pQueue	= &kl_drawqueue;
	int				c		= 0;
	int				v		= 0;
	u16				fixrot	= rot;
	
	r *= 0.5f;
	
	KLDrawQueue_Check(KLPRIM_CIRCLE, &kl_share_sprite, 0);
	pQueue->type	= KLPRIM_CIRCLE;
	
	c		= pQueue->offset*12;
	v		= pQueue->offset*9;
	
	if(nTerm<3)
	{
		nTerm = KL_CIRCLE_QUALITY_HIGH;
	}
	
	if( pQueue->offset+nTerm*3 >= pQueue->nMax )
	{
		return;
	}
	
	for( int i=nTerm; i>=0; --i )
	{
	
		// Center
		pQueue->pVertex[v++] 	= cx;
		pQueue->pVertex[v++] 	= cy;
		pQueue->pVertex[v++] 	= kl_drawqueue.clipz;
		pQueue->pColorRate[c+0]	= 0;
		pQueue->pColorRate[c+1]	= 0;
		pQueue->pColorRate[c+2]	= 0;
		pQueue->pColorRate[c+3]	= 0;
		pQueue->pColor[c+0]		= rgba.r;
		pQueue->pColor[c+1]		= rgba.g;
		pQueue->pColor[c+2]		= rgba.b;
		pQueue->pColor[c+3]		= rgba.a;
		
		pQueue->pVertex[v++] 	= cx + r * KL_Cos((KL_WDEG_CYCLE*i/nTerm+fixrot)%(u32)KL_WDEG_CYCLE);
		pQueue->pVertex[v++] 	= cy + r * KL_Sin((KL_WDEG_CYCLE*i/nTerm+fixrot)%(u32)KL_WDEG_CYCLE);
		pQueue->pVertex[v++] 	= kl_drawqueue.clipz;
		pQueue->pColorRate[c+4]	= 0;
		pQueue->pColorRate[c+5]	= 0;
		pQueue->pColorRate[c+6]	= 0;
		pQueue->pColorRate[c+7]	= 0;
		pQueue->pColor[c+4]		= rgba.r;
		pQueue->pColor[c+5]		= rgba.g;
		pQueue->pColor[c+6]		= rgba.b;
		pQueue->pColor[c+7]		= rgba.a;
		
		if(i)
		{
			pQueue->pVertex[v++] 	= cx + r * KL_Cos((KL_WDEG_CYCLE*(i-1)/nTerm+fixrot)%(u32)KL_WDEG_CYCLE);
			pQueue->pVertex[v++] 	= cy + r * KL_Sin((KL_WDEG_CYCLE*(i-1)/nTerm+fixrot)%(u32)KL_WDEG_CYCLE);
		}
		else
		{
			pQueue->pVertex[v++] 	= cx + r * KL_Cos((KL_WDEG_CYCLE+fixrot)%(u32)KL_WDEG_CYCLE);
			pQueue->pVertex[v++] 	= cy + r * KL_Sin((KL_WDEG_CYCLE+fixrot)%(u32)KL_WDEG_CYCLE);
		}
		pQueue->pVertex[v++] 	= kl_drawqueue.clipz;
		pQueue->pColorRate[c+8]	= 0;
		pQueue->pColorRate[c+9]	= 0;
		pQueue->pColorRate[c+10]= 0;
		pQueue->pColorRate[c+11]= 0;
		pQueue->pColor[c+8]		= rgba.r;
		pQueue->pColor[c+9]		= rgba.g;
		pQueue->pColor[c+10]	= rgba.b;
		pQueue->pColor[c+11]	= rgba.a;

		c += 12;
		
	}
	
	pQueue->offset += nTerm;
	
}

//===========================================================
/**
 *
 *	@brief	深度バッファを使用してクリッピング描画を開始する<br/>
 *			このコール後、KL_StartClippingDraw をコールするまで透明な描画扱いでクリッピング情報がバッファに書き込まれる。<br/>
 *			描画した箇所を描画するエリアにするか、その箇所以外を描画するエリアにするかは KL_StartClippingDraw で指定する。
 *
 *	@param	maskid 1〜255 でマスクに設定するID これと後に比較してどこをマスクするかを決める
 */
//===========================================================
void KL_PrepareClipping (u8 cid) {

	KLDrawQueue_Flush();
	KLDrawQueue_Clear();
	
#ifdef KL_DEPTH_CLIPPING

	kl_drawqueue.clipz = cid;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc( GL_ALWAYS );
	glDepthMask(1);

#else

	//glClearStencil(~0);
	//glClear(GL_STENCIL_BUFFER_BIT);
	
	glsEnable(GL_STENCIL_TEST);
	glStencilFunc( GL_ALWAYS, cid, ~0);
	glStencilOp( GL_KEEP, GL_REPLACE, GL_REPLACE );
	
	glStencilMask(0xFF);
	glsColorMask(0,0,0,1);
	glsDepthMask(0);
	
#endif
	
}

//===========================================================
/**
 *
 *	@brief	以降、KL_EndClippingDraw がコールされるまで深度バッファを使用してマスキング描画となる
 *
 *	@param	cid	クリッピング描画終了後に式評価をするID
 *	@param	optype	クリッピングエリア内のみ描画なら GL_EQUAL、エリア外に描画なら GL_NOTEQUAL
 */
//===========================================================
void KL_StartClippingDraw ( u8 cid, GLenum optype ) {

	KLDrawQueue_Flush();
	KLDrawQueue_Clear();

#ifdef KL_DEPTH_CLIPPING
		
	glDepthFunc( optype );
	
#else

	glsColorMask(1,1,1,1);
	glsDepthMask(0);
	
	glStencilMask(0xff);
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
	glStencilFunc( optype, cid, ~0);

#endif
}

void KL_EndClippingDraw () {
	
	KLDrawQueue_Flush();
	KLDrawQueue_Clear();

#ifdef KL_DEPTH_CLIPPING
	
	glDepthMask(0);
	glDisable(GL_DEPTH_TEST);
	kl_drawqueue.clipz = 0;
	
#else
	
	glsDisable(GL_STENCIL_TEST);
	
#endif
}
