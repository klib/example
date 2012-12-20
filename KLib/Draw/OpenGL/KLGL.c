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

#include <string.h>
#include "KLGL.h"
#include "KLibDefine.h"
#include "KLMath.h"
#include "KLib.h"

/// OpenGLマネージャ
static GLManager glm;

//=================================================================
/**
 *	@brief	カラーバッファのクリア
 */
//=================================================================
inline void glsClearc(){ glsDepthMask(GL_TRUE); glClear(GL_COLOR_BUFFER_BIT); };

//=================================================================
/**
 *	@brief	デプスバッファのクリア
 */
//=================================================================
inline void glsCleard(){ glClear(GL_DEPTH_BUFFER_BIT); };

//=================================================================
/**
 *	@brief	バッファのクリア
 */
//=================================================================
inline void glsClear(){ glsDepthMask(GL_TRUE); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); };

//=================================================================
/**
 *	@brief	DepthMask設定(前回と同じなら無視)
 */
//=================================================================
bl glsDepthMask( GLboolean ismask ){
	
	if( ismask != glm.isdepthmask ){
		
		return glsDepthMaskf(ismask);
		
	}
	
	return FALSE;
	
}



//=================================================================
/**
 *	@brief	DepthMaskを強制設定(前回と同じなら無視)
 */
//=================================================================
bl glsDepthMaskf( GLboolean ismask ){
	
	glm.isdepthmask = ismask;
	glDepthMask(ismask);
	return TRUE;
	
}


//=================================================================
/**
 *	@brief	クリアカラーの設定(前回と同じなら無視)
 */
//=================================================================
bl glsClearColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a ){
	
	if( r!=glm.clearR || g!=glm.clearG || b!=glm.clearB || a!=glm.clearA ){
		
		return glsClearColorf(r,g,b,a);
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	クリアカラーを強制的に設定
 */
//=================================================================
bl glsClearColorf( GLfloat r, GLfloat g, GLfloat b, GLfloat a ){
	
	glm.clearR = r;
	glm.clearG = g;
	glm.clearB = b;
	glm.clearA = a;
	glClearColor(r, g, b, a);
	return TRUE;
	
}
//=================================================================
/**
 *	@brief	フレームバッファが利用可能かを返す
 */
//=================================================================
inline bl glsCheckFramebuffer(){
	return (glCheckFramebufferStatus((GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)); };

//=================================================================
/**
 *	@brief	GLEnabledフラグのポインタを取得
 */
//=================================================================
inline KLGLFlags* KLGL_GetFlags(){ return &glm.state.enabled; }

//=================================================================
/**
 *	@brief	最大テクスチャサイズを取得
 */
//=================================================================
inline u32 KLGL_GetMaxTextureSize(){ return glm.maxTextureSize; }

//=================================================================
/**
 *	@brief	GLマネージャの初期化
 */
//=================================================================
void KLGL_Init( GLuint programid ){
	
	KLGLFlags*	pEnabled	= KLGL_GetFlags();
	u16 state[] = {
			GL_BLEND
		,	GL_CULL_FACE
		,	GL_DEPTH_TEST
		,	GL_DITHER
		,	GL_POLYGON_OFFSET_FILL
		,	GL_SAMPLE_ALPHA_TO_COVERAGE
		,	GL_SAMPLE_COVERAGE
		,	GL_SCISSOR_TEST
		,	GL_STENCIL_TEST
	};
	size_t	len = sizeof(state)/sizeof(u16);
	u32		i;
	
	memset( &glm, 0, sizeof(GLManager) );
	
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glm.maxTextureSize);
	
	glm.rMask = TRUE;
	glm.gMask = TRUE;
	glm.bMask = TRUE;
	glm.aMask = TRUE;
	
	glm.depthFunc = (GLenum)0xFFFF;
	
	glsClearColorf( 1,1,1,1 );
	
	for( i=0; i<len; ++i ){
		
		if( GL_TRUE==glIsEnabled(state[i]) ){
			
			glsEnablef( state[i] );

#ifdef KL_LOG_GL
			KLLog("[ GLS ] (%i) TRUE \n", i);
#endif
			
		}else{
			
			glsDisablef( state[i] );

#ifdef KL_LOG_GL
			KLLog("[ GLS ] (%i) FALSE \n", i);
#endif
		
		}
		
	}
	
	pEnabled->color_array		= FALSE;
	pEnabled->normal_array		= FALSE;
	pEnabled->point_size_array	= FALSE;
	pEnabled->uv_array			= FALSE;
	pEnabled->vertex_array		= FALSE;
	
	// 初期ブレンドの設定
	glsBlendf(TRUE);
	//glsBlendFuncSeparatef( GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR );
	glsBlendFuncf( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	
	glsCullFacef(TRUE);
	
	glClearDepthf(0);
	
	glm.program	= programid;
	glm.locproj	= glGetUniformLocation(glm.program, "matproj");
	
	glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES, GL_FASTEST);
	
	glsDepthMaskf(GL_FALSE);
	glsDisablef(GL_DITHER);
	//glsDisablef(GL_BLEND);
	glsDisablef(GL_STENCIL_TEST);
	glsDisablef(GL_TEXTURE_2D);
	glsDisablef(GL_DEPTH_TEST);
}

//=================================================================
/**
 *	@brief	ブレンドファンクションを前回と異なる時だけ設定する
 */
//=================================================================
bl glsBlendFunc( u16 src, u16 dest ){

	if( glm.mode_blend[0] != src || glm.mode_blend[1] !=dest ){
		
		return glsBlendFuncf(src,dest);
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	ブレンドファンクションを強制的に変更する
 */
//=================================================================
bl glsBlendFuncf( u16 src, u16 dest ){
	
	glm.mode_blend[0] = src;
	glm.mode_blend[1] = dest;
	glBlendFunc( src, dest );
	return TRUE;
	
}


//=================================================================
/**
 *	@brief	ブレンドファンクションを前回と異なる時だけ設定する
 */
//=================================================================
bl glsBlendFuncSeparate( u16 src, u16 dest, u16 src_a, u16 dest_a ){
	
	if(	glm.mode_blend[0] != src   || glm.mode_blend[1] !=dest ||
		glm.mode_blend[2] != src_a || glm.mode_blend[3] !=dest_a ){
		
		return glsBlendFuncSeparatef(src,dest,src_a,dest_a);
		
	}
	
	return FALSE;
	
}


//=================================================================
/**
 *	@brief	ブレンドファンクションをRGB/Aそれぞれで強制的に変更する
 */
//=================================================================
bl glsBlendFuncSeparatef( u16 src, u16 dest, u16 src_a, u16 dest_a ){
	
	glm.mode_blend[0] = src;
	glm.mode_blend[1] = dest;
	glm.mode_blend[2] = src_a;
	glm.mode_blend[3] = dest_a;
	glBlendFuncSeparate(src, dest, src_a, dest_a);
	return TRUE;
	
}


//=================================================================
/**
 *	@brief	UniformMatrix4fvを前回と異なる時だけ設定する
 */
//=================================================================
bl glsUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ){
	
	if( location != klib.shader.uniform.handler ||
	   	klib.shader.uniform.count!=count ||
	   	klib.shader.uniform.transpose!=transpose ||
	   	klib.shader.uniform.value!=value )
	{
		
		return glsUniformMatrix4fvf( location, count, transpose, value );
		
	}
	
	return FALSE;
}


//=================================================================
/**
 *	@brief	UniformMatrix4fvを強制設定する
 */
//=================================================================
bl glsUniformMatrix4fvf( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value ){
	
	klib.shader.uniform.handler 	= location;
	klib.shader.uniform.count		= count;
	klib.shader.uniform.transpose	= transpose;
	klib.shader.uniform.value		= (GLfloat*)value;
	
	glUniformMatrix4fv( location, count, transpose, value );
	
	return TRUE;

}

//=================================================================
/**
 *	@brief	プロジェクション行列をシェーダに転送<br />
 *			useProgramでprogramを指定後に行わないとGL_INVALID_OPERATIONとなるので注意
 */
//=================================================================
void glsLinkProjectionMatrix(){
	
	if( KLMat4_Cmp(&glm.matproj, &glm.matprojold) )
	{
		glm.matprojold = glm.matproj;
		glsUniformMatrix4fvf(glm.locproj, 1, GL_FALSE, glm.matproj.p);
		KLLog("Linked to the new projection.\n");
	}
	
}

//=================================================================
/**
 *	@brief	正射影プロジェクションの設定<br />
 *			描画前にglsLinkOrthMatrixを行わないと意味がない
 */
//=================================================================
bl glsOrthMatrix( float l, float r, float top, float bottom, float near, float far, float xoffset, float yoffset ){

	KLMat4 mat		= KLMat4_GetOrthoMatrix(l,r,bottom,top,near,far);
	
	if( KLMat4_Cmp(&glm.matproj, &mat) ){
	
		glm.matproj = mat;
		KLLog("Generate new ortho matrix.\n");
		return TRUE;
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	正射影プロジェクションを強制的に設定<br />
 *			描画前にglsLinkOrthMatrixを行わないと意味がない
 */
//=================================================================
bl glsOrthMatrixf( float l, float r, float top, float bottom, float near, float far, float xoffset, float yoffset  ){
	
	KLMat4 mat = KLMat4_GetOrthoMatrix(l,r,bottom,top,near,far);
	
	glm.matproj = KLMat4_Move( mat, xoffset, yoffset, 0 );
	
	return TRUE;
}


//=================================================================
/**
 *	@brief	マトリクスモードの設定
 */
//=================================================================
bl glsMatrixMode( KLMatMode matrixMode ){
	
	if( glm.mode_matrix != matrixMode ){
		
		return glsMatrixModef( matrixMode );
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	マトリクスモードの設定を強制的に実行
 */
//=================================================================
bl glsMatrixModef( KLMatMode matrixMode ){
	
	glm.mode_matrix = matrixMode;
	
	return TRUE;
	
}

//=================================================================
/**
 *	@brief	フレームバッファの割当
 */
//=================================================================
bl glsBindFramebuffer( GLuint fbid ){

	if( fbid != glm.fbid ){
		
		return glsBindFramebufferf( fbid );
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	フレームバッファの割当を強制的に実行
 */
//=================================================================
bl glsBindFramebufferf( GLuint fbid ){
		
	glm.fbid = fbid;
	glBindFramebuffer(GL_FRAMEBUFFER, fbid );
	
	return TRUE;
	
}

//=================================================================
/**
 *	@brief	レンダリングバッファの割当
 */
//=================================================================
bl glsBindRenderbuffer( GLuint rbid ){
	
	if( rbid != glm.rbid ){
		
		return glsBindRenderbufferf( rbid );
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	レンダリングバッファの割当を強制的に実行
 */
//=================================================================
bl glsBindRenderbufferf( GLuint rbid ){
	
	glm.rbid = rbid;
	glBindRenderbuffer(GL_RENDERBUFFER, rbid );
	
	return TRUE;
	
}

//=================================================================
/**
 *	@brief	レンダリングバッファの割当
 */
//=================================================================
bl glsBindDepthbuffer( GLuint dbid ){
	
	if( dbid != glm.dbid ){
		
		return glsBindDepthbufferf( dbid );
		
	}
	
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	レンダリングバッファの割当を強制的に実行
 */
//=================================================================
bl glsBindDepthbufferf( GLuint dbid ){
	
	glm.dbid = dbid;
	glBindRenderbuffer(GL_RENDERBUFFER, dbid );
	
	return TRUE;
	
}

//=================================================================
/**
 *	@brief	テクスチャの割当
 */
//=================================================================
bl glsBindTexture( GLuint tid ){

	if( tid != glm.tid ){
		
		return glsBindTexturef( tid );
		
	}
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	テクスチャの割当を強制的に実行
 */
//=================================================================
bl glsBindTexturef( GLuint tid ){
	
	glm.tid = tid;
	glBindTexture(GL_TEXTURE_2D, tid );
	
	return TRUE;
	
}

//=================================================================
/**
 *	@brief	フィルタの割当
 */
//=================================================================
bl glsFilter( GLenum filter_type ){
	
	if( filter_type != glm.filter_type ){
		
		return glsFilterf(filter_type);
		
	}
	return FALSE;
	
}

//=================================================================
/**
 *	@brief	フィルタの割当を強制的に実行
 */
//=================================================================
bl glsFilterf( GLenum filter_type ){
	
	glm.filter_type = filter_type;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_type );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_type );
	
	return TRUE;
	
}

//=================================================================
/**
 *	@brief	フィルタの割当(==glsFilter)
 */
//=================================================================
bl glsTexParameteri( GLenum filter_type ){
	
	return glsFilter(filter_type);
	
}

//=================================================================
/**
 *	@brief	フィルタの割当を強制的に実行(==glsFilterf)
 */
//=================================================================
bl glsTexParameterif( GLenum filter_type ){
	
	return glsFilterf(filter_type);
	
}

//========================================================================
/**
 *	@brief	実際のglEnable/glDisableの使用状況に関わらず直接管理フラグを変更する
 */
//========================================================================
static bl GL_SetStateForce( int state, bl isTrue ){
	
	isTrue ? glEnable(state) : glDisable(state);
	
	return TRUE;
	
}

//========================================================================
/**
 *	@brief	指定状態が現状と異なる場合のみ状態フラグを変更しglコマンドを発行する
 */
//========================================================================
static bl GL_SetState( u16 state, bl isTrue ){
	
	KLGLFlags*	pEnabled	= KLGL_GetFlags();
	bl		run			= FALSE;
	
	switch ( state )
	{

		case	GL_BLEND:							{ run = ( pEnabled->blend != isTrue );				if(run)pEnabled->blend=isTrue; }break;
		case	GL_CULL_FACE:						{ run = ( pEnabled->cull_face != isTrue );			if(run)pEnabled->cull_face=isTrue; }break;
		case GL_DEPTH_TEST:						{ run = ( pEnabled->depth_test != isTrue );		if(run)pEnabled->depth_test=isTrue; }break;
		case	GL_DITHER:							{ run = ( pEnabled->dither != isTrue );			if(run)pEnabled->dither=isTrue; }break;
		case	GL_POLYGON_OFFSET_FACTOR:		{ run = ( pEnabled->polygon_offset_factor != isTrue );	if(run)pEnabled->polygon_offset_factor=isTrue; }break;
		case	GL_POLYGON_OFFSET_FILL:			{ run = ( pEnabled->polygon_offset_fill != isTrue );	if(run)pEnabled->polygon_offset_fill=isTrue; }break;
		case	GL_POLYGON_OFFSET_UNITS:			{ run = ( pEnabled->polygon_offset_units != isTrue );	if(run)pEnabled->polygon_offset_units=isTrue; }break;
		case	GL_SAMPLE_ALPHA_TO_COVERAGE:	{ run = ( pEnabled->sample_a2cov != isTrue );		if(run)pEnabled->sample_a2cov=isTrue; }break;
		case	GL_SAMPLE_COVERAGE:				{ run = ( pEnabled->sample_cov != isTrue );		if(run)pEnabled->sample_cov=isTrue; }break;
		case	GL_SCISSOR_TEST:					{ run = ( pEnabled->scissor_test != isTrue );		if(run)pEnabled->scissor_test=isTrue; }break;
		case	GL_STENCIL_TEST:					{ run = ( pEnabled->stencil_test != isTrue );		if(run)pEnabled->stencil_test=isTrue; }break;
		case	GL_TEXTURE_2D:						{ run = ( pEnabled->texture2d != isTrue );			if(run)pEnabled->texture2d=isTrue; }break;
			
		default:									{ run = FALSE; } break; 
			
	}
	
	if(run)
	{
		
		return GL_SetStateForce(state,isTrue);
		
	}
	
	return run;
	
}

//========================================================================
/**
 *	@brief	指定状態が現状と異なる場合のみ状態フラグを変更しglコマンドを発行する
 */
//========================================================================
bl glsEnable( u16 state ){

	return GL_SetState( state, 1 );
	
}

//========================================================================
/**
 *	@brief	指定状態が現状と異なる場合のみ状態フラグを変更しglコマンドを発行する
 */
//========================================================================
bl glsDisable( u16 state ){
	
	return GL_SetState( state, 0 );
	
}

//========================================================================
/**
 *	@brief	強制的に状態フラグを変更しglコマンドを発行する
 */
//========================================================================
bl glsEnablef( u16 state ){
	
	return GL_SetStateForce( state, 1 );
	
}

//========================================================================
/**
 *	@brief	強制的に状態フラグを変更しglコマンドを発行する
 */
//========================================================================
bl glsDisablef( u16 state ){
	
	return GL_SetStateForce( state, 0 );
	
}

//========================================================================
/**
 *	@brief	指定状態が現状と異なる場合のみglColorMaskを行う
 */
//========================================================================
bl glsColorMask(bl rMask, bl gMask, bl bMask, bl aMask)
{
	if( rMask!=glm.rMask || gMask!=glm.gMask || bMask!=glm.bMask || aMask!=glm.aMask )
	{
		glm.rMask = rMask;
		glm.gMask = gMask;
		glm.bMask = bMask;
		glm.aMask = aMask;
		glColorMask(rMask, gMask, bMask, aMask);
		return TRUE;
	}
	
	return FALSE;
}

//========================================================================
/**
 *	@brief	強制的にglColorMaskを行う
 */
//========================================================================
bl glsColorMaskf(bl rMask, bl gMask, bl bMask, bl aMask)
{
	if( rMask!=glm.rMask || gMask!=glm.gMask || bMask!=glm.bMask || aMask!=glm.aMask )
	{
		glm.rMask = rMask;
		glm.gMask = gMask;
		glm.bMask = bMask;
		glm.aMask = aMask;
		glColorMask(rMask, gMask, bMask, aMask);
	}
	return TRUE;
}


//========================================================================
/**
 *	@brief	DepthFuncタイプの設定
 */
//========================================================================
inline bl glsDepthFunc( GLenum funcType ){ if( glm.depthFunc!=funcType ){ glm.depthFunc=funcType; return TRUE; } return FALSE; };
//========================================================================
/**
 *	@brief	DepthFuncタイプの設定（強制）
 */
//========================================================================
inline bl glsDepthFuncf( GLenum funcType ){ glm.depthFunc=funcType; return TRUE; };

//========================================================================
/**
 *	@brief	ブレンド有無の設定
 */
//========================================================================
inline bl glsBlend( bl isTrue ){ return GL_SetState( GL_BLEND, isTrue ); };

//========================================================================
/**
 *	@brief	ブレンド有無の設定（強制）
 */
//========================================================================
inline bl glsBlendf( bl isTrue ){ return GL_SetStateForce( GL_BLEND, isTrue ); };


//========================================================================
/**
 *	@brief	CullFaceの設定
 */
//========================================================================
inline bl glsCullFace( bl isTrue ){ return GL_SetState( GL_CULL_FACE, isTrue ); };

//========================================================================
/**
 *	@brief	CullFaceの設定（強制）
 */
//========================================================================
inline bl glsCullFacef( bl isTrue ){ return GL_SetStateForce( GL_CULL_FACE, isTrue ); };


//========================================================================
/**
 *	@brief	DepthTestの設定
 */
//========================================================================
inline bl glsDepthTest( bl isTrue ){ return GL_SetState( GL_DEPTH_TEST, isTrue ); };

//========================================================================
/**
 *	@brief	DepthTestの設定（強制）
 */
//========================================================================
inline bl glsDepthTestf( bl isTrue ){ return GL_SetStateForce( GL_DEPTH_TEST, isTrue ); };


//========================================================================
/**
 *	@brief	Ditherの設定
 */
//========================================================================
inline bl glsDither( bl isTrue ){ return GL_SetState( GL_DITHER, isTrue ); };

//========================================================================
/**
 *	@brief	Ditherの設定（強制）
 */
//========================================================================
inline bl glsDitherf( bl isTrue ){ return GL_SetStateForce( GL_DITHER, isTrue ); };


//========================================================================
/**
 *	@brief	PolygonOffsetFactorの設定
 */
//========================================================================
inline bl glsPolygonOffsetFactor( bl isTrue ){ return GL_SetState( GL_POLYGON_OFFSET_FACTOR, isTrue ); };

//========================================================================
/**
 *	@brief	PolygonOffsetFactorの設定（強制）
 */
//========================================================================
inline bl glsPolygonOffsetFactorf( bl isTrue ){ return GL_SetStateForce( GL_POLYGON_OFFSET_FACTOR, isTrue ); };


//========================================================================
/**
 *	@brief	PolygonOffsetFillの設定
 */
//========================================================================
inline bl glsPolygonOffsetFill( bl isTrue ){ return GL_SetState( GL_POLYGON_OFFSET_FILL, isTrue ); };

//========================================================================
/**
 *	@brief	PolygonOffsetFillの設定（強制）
 */
//========================================================================
inline bl glsPolygonOffsetFillf( bl isTrue ){ return GL_SetStateForce( GL_POLYGON_OFFSET_FILL, isTrue ); };


//========================================================================
/**
 *	@brief	PolygonOffsetUnitsの設定
 */
//========================================================================
inline bl glsPolygonOffsetUnits( bl isTrue ){ return GL_SetState( GL_POLYGON_OFFSET_UNITS, isTrue ); };

//========================================================================
/**
 *	@brief	PolygonOffsetUnitsの設定（強制）
 */
//========================================================================
inline bl glsPolygonOffsetUnitsf( bl isTrue ){ return GL_SetStateForce( GL_POLYGON_OFFSET_UNITS, isTrue ); };


//========================================================================
/**
 *	@brief	SampleAlphaToCoverageの設定
 */
//========================================================================
inline bl glsSampleAlphaToCoverage( bl isTrue ){ return GL_SetState( GL_SAMPLE_ALPHA_TO_COVERAGE, isTrue ); };

//========================================================================
/**
 *	@brief	SampleAlphaToCoverageの設定（強制）
 */
//========================================================================
inline bl glsSampleAlphaToCoveragef( bl isTrue ){ return GL_SetStateForce( GL_SAMPLE_ALPHA_TO_COVERAGE, isTrue ); };


//========================================================================
/**
 *	@brief	SampleCoverageの設定
 */
//========================================================================
inline bl glsSampleCoverage( bl isTrue ){ return GL_SetState( GL_SAMPLE_COVERAGE, isTrue ); };

//========================================================================
/**
 *	@brief	SampleCoverageの設定（強制）
 */
//========================================================================
inline bl glsSampleCoveragef( bl isTrue ){ return GL_SetStateForce( GL_SAMPLE_COVERAGE, isTrue ); };


//========================================================================
/**
 *	@brief	ScissorTestの設定
 */
//========================================================================
inline bl glsScissorTest( bl isTrue ){ return GL_SetState( GL_SCISSOR_TEST, isTrue ); };

//========================================================================
/**
 *	@brief	ScissorTestの設定（強制）
 */
//========================================================================
inline bl glsScissorTestf( bl isTrue ){ return GL_SetStateForce( GL_SCISSOR_TEST, isTrue ); };


//========================================================================
/**
 *	@brief	StencilTestの設定
 */
//========================================================================
inline bl glsStencilTest( bl isTrue ){ return GL_SetState( GL_STENCIL_TEST, isTrue ); };

//========================================================================
/**
 *	@brief	StencilTestの設定（強制）
 */
//========================================================================
inline bl glsStencilTestf( bl isTrue ){ return GL_SetStateForce( GL_STENCIL_TEST, isTrue ); };

//========================================================================
/**
 *	@brief	指定値が収まる2の累乗を返す
 *
 *	@param	n	チェックする数値
 *
 *	@return	その数が収まる2の累乗数 / 最大値はOpenGLESの仕様に関係なく0x80000000 / 異常時は0
 */
//========================================================================
u32 KLGL_ValidateTextureSize( u32 n ){

	// 既に2のべき乗ならそのまま返す
	if( !(n & (n-1)) )
	{
		return n;
	}
	
	// ２のべき乗ではなければ次のべき乗になる数を返す
	return 2<<((u32)log2( KLMath_GetTop1bitOnlyNumber(n)<<1 )-1);
	
}

//========================================================================
/**
 *	@brief	テクスチャ使用量を加算<br/>
 *			メモリの解放同様、減算をしないとずっと情報が残るので注意
 *
 *	@param	wxh			幅と高さ
 *	@param	colorType	GL_RGBAなどのカラータイプ
 *	@param	byteType	GL_UNSIGNED_BYTEなどの１要素辺りのサイズ
 */
//========================================================================
void KLGL_AddTextureUsage( size_t wxh, GLuint colorType, GLuint byteType ){

	// ひとまずRGBとRGBAのみ対応
	u32 c = colorType== GL_RGBA ? 4 : colorType==GL_RGB ? 3 : 3;
	u32 b = byteType == GL_UNSIGNED_BYTE|| (byteType == GL_BYTE)? 1 :
			byteType == GL_UNSIGNED_INT || (byteType == GL_INT) ? 4 : 2;
	u32 v = c*b*wxh;
	
	glm.szUsageTex += v;

	KLLog("[ KLGL ] Tex usage:+%d Total:%ld\n", v, glm.szUsageTex);
}

//========================================================================
/**
 *	@brief	テクスチャ使用量を減算<br/>
 *			メモリの解放同様、加算したものは減算をしないとずっと情報が残るので注意
 *
 *	@param	wxh			幅と高さ
 *	@param	colorType	GL_RGBAなどのカラータイプ
 *	@param	byteType	GL_UNSIGNED_BYTEなどの１要素辺りのサイズ
 */
//========================================================================
void KLGL_SubTextureUsage( size_t wxh, GLuint colorType, GLuint byteType ){
	
	// ひとまずRGBとRGBAのみ対応
	u32 c = colorType== GL_RGBA ? 4 : colorType==GL_RGB ? 3 : 3;
	u32 b = byteType == GL_UNSIGNED_BYTE|| (byteType == GL_BYTE)? 1 :
			byteType == GL_UNSIGNED_INT || (byteType == GL_INT) ? 4 : 2;
	u32 v = c*b*wxh;
	
	glm.szUsageTex -= v;
	
	KLLog("[ KLGL ] Tex usage:-%d Total:%ld\n", v, glm.szUsageTex);
	
}

//========================================================================
/**
 *	@brief	前回と状態が違う時のみviewportを設定
 *
 *	@param	x	X
 *	@param	y	Y
 *	@param	w	W
 *	@param	h	H
 *
 *	@return	変更されたらTRUE
 */
//========================================================================
bl glsViewport( GLint x, GLint y, GLsizei w, GLsizei h ){
	
	if( glm.viewportX!=x || glm.viewportY!=y || glm.viewportW!=w || glm.viewportH!=h )
	{
		glm.viewportX = x;
		glm.viewportY = y;
		glm.viewportW = w;
		glm.viewportH = h;
		
		glViewport(x,y,w,h);
		return TRUE;
		
	}
	
	return FALSE;
	
}

//========================================================================
/**
 *	@brief	前回と状態が違う時のみscissorを設定
 *
 *	@param	x	X
 *	@param	y	Y
 *	@param	w	W
 *	@param	h	H
 *
 *	@return	変更されたらTRUE
 */
//========================================================================
bl glsScissor( GLint x, GLint y, GLsizei w, GLsizei h ){
	
	if( glm.scissorX!=x || glm.scissorY!=y || glm.scissorW!=w || glm.scissorH!=h )
	{
		glm.scissorX = x;
		glm.scissorY = y;
		glm.scissorW = w;
		glm.scissorH = h;
		
		glScissor(x,y,w,h);
		return TRUE;
		
	}
	
	return FALSE;
	
}

//========================================================================
/**
 *	@brief	前回と状態が違う時のみglVertexAttribPointerを設定
 *
 *	@param	pHandler	KLShaderHandlerのポインタ
 *	@param	size		1要素のサイズ
 *	@param	type		データタイプ
 *	@param	normalized	正規化有無
 *	@param	stride		1要素あたりのポインタ進行数
 *	@param	ptr			データポインタ
 *
 *	@return	変更されたらTRUE
 */
//========================================================================
bl glsVertexAttribPointer(KLShaderHandler* pHandler, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr){
	
	if( ptr==NULL || pHandler->size!=size || pHandler->type!=type ||
	   pHandler->normalized!=normalized || pHandler->stride!=stride || pHandler->ptr!=ptr )
	{
		pHandler->size = size;
		pHandler->type = type;
		pHandler->normalized = normalized;
		pHandler->stride = stride;
		pHandler->ptr = (GLvoid*)ptr;
		glVertexAttribPointer(pHandler->handler, size, type, normalized, stride, ptr);
		return TRUE;
	}
	
	return FALSE;
	
}

//========================================================================
/**
 *	@brief	前回と状態が違う時のみglEnableVertexAttribArrayを設定
 *
 *	@param	pHandler	KLShaderHandlerのポインタ
 *
 *	@return	変更されたらTRUE
 */
//========================================================================
bl glsEnableVertexAttribArray( KLShaderHandler* pHandler ){

	if( !pHandler->enabled ){
		
		pHandler->enabled = TRUE;
		glEnableVertexAttribArray( pHandler->handler );
		return TRUE;
		
	}
	
	return FALSE;
	
}

//========================================================================
/**
 *	@brief	前回と状態が違う時のみglEnableVertexAttribArrayを設定
 *
 *	@param	pHandler	KLShaderHandlerのポインタ
 *
 *	@return	変更されたらTRUE
 */
//========================================================================
bl glsDisableVertexAttribArray( KLShaderHandler* pHandler ){
	
	if( pHandler->enabled ){
		
		pHandler->enabled = FALSE;
		glDisableVertexAttribArray( pHandler->handler );
		return TRUE;
		
	}
	
	return FALSE;
	
}
