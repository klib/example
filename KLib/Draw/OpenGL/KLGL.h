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

#ifndef ___KLIB_GL_H___
#define ___KLIB_GL_H___
	
// I N C L U D E
//===================================================================

	#include "c_extern.h"
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
	#include "KLShader.h"
	#include "KLibDefine.h"
	#include "KLMatrix.h"

// D E F I N E
//========================================================================

	/// 未定義時のテクスチャID
	#define KLGL_UNBINDED_TEXID (0)

	/// 軸要素数2D
	#define KL_AXIS_SIZE_XY (2)

	/// 軸要素数3D
	#define KL_AXIS_SIZE_XYZ (3)

	/// RGB要素数
	#define KL_COLOR_SIZE_RGB (3)

	/// RGBA要素数
	#define KL_COLOR_SIZE_RGBA (4)


// E N U M
//========================================================================

		/// 描画モード
		enum
		{
			KL_DRAWMODE_DEFAULT,
			KL_DRAWMODE_MASK,
			KL_DRAWMODE_MAX,
		};

		/// 軸インデクス
		enum{
			X = 0,
			Y,
			Z
		};

		/// 行列モード
		typedef enum{
			
			KLMAT_PROJECTION,
			KLMAT_MODEL,
			
		}KLMatMode;



	// S T R U C T U R E
	//========================================================================

		//========================================================================
		/**
		 *	@brief	glEnableされた情報フラグ
		 *	@todo	ES1の頃のフラグが残っているので要整理
		 */
		//========================================================================	
		typedef struct
		{
			unsigned	color_array:1;
			unsigned	normal_array:1;
			unsigned	point_size_array:1;
			unsigned	uv_array:1;
			unsigned	vertex_array:1;
			
			unsigned alpha_test:1;
			unsigned blend:1;
			unsigned color_logic_op:1;
			unsigned clip_plane0:1;
			unsigned clip_plane1:1;
			unsigned clip_plane2:1;
			unsigned clip_plane3:1;
			unsigned clip_plane4:1;
			unsigned clip_plane5:1;
			unsigned color_material:1;
			unsigned cull_face:1;
			unsigned depth_test:1;
			unsigned dither:1;
			unsigned fog:1;
			unsigned light0:1;
			unsigned light1:1;
			unsigned light2:1;
			unsigned light3:1;
			unsigned light4:1;
			unsigned light5:1;
			unsigned light6:1;
			unsigned light7:1;
			unsigned lighting:1;
			unsigned line_smooth:1;
			unsigned multi_sample:1;
			unsigned normalize:1;
			unsigned point_smooth:1;
			unsigned point_sprite:1;
			unsigned polygon_offset_factor:1;
			unsigned polygon_offset_fill:1;
			unsigned polygon_offset_units:1;
			unsigned rescale_normal:1;
			unsigned sample_a2cov:1;
			unsigned sample_a2one:1;
			unsigned sample_cov:1;
			unsigned scissor_test:1;
			unsigned stencil_test:1;
			unsigned texture2d:1;
			unsigned use_colorary:1;
			unsigned use_vertexary:1;
			
		}KLGLFlags;
		
		//========================================================================
		/**
		 *	@brief	GLの状態
		 */
		//========================================================================	
		typedef struct GLState{

			void*		p_color;
			void*		p_normal;
			void*		p_point_size;
			void*		p_uv;
			void*		p_vertex;
			
			KLGLFlags enabled;
			
		}GLState;


		//========================================================================
		/**
		 *	@brief	GLマネージャ
		 */
		//========================================================================	
		typedef struct GLManager{
			
			GLuint		program;					///< 共通使用のプログラム(これのUnity等を変更に使う)
			GLint		locproj;					///< プロジェクションマトリクスのロケーションID
			KLMat4		matproj;					///< プロジェクションマトリクス
			KLMat4		matprojold;					///< プロジェクションマトリクス(変更比較用)

			GLState		state;						///< 状態系フラグ
			void*		p_vertices;					///< 現在の頂点情報
			void*		p_colors;					///< 現在の色情報
			GLenum		depthFunc;
			size_t		szUsageTex;					///< テクスチャ使用量
			GLfloat		clipping_area[4][4];
			GLuint		fbid;						///< 現在バインドされているフレームバッファID
			GLuint		rbid;						///< 現在バインドされているレンダリングバッファID
			GLuint		dbid;						///< 現在バインドされているデプスバッファID
			GLuint		tid;						///< 現在バインドされているテクスチャID
			GLint		maxTextureSize;				///< 最大テクスチャサイズ
			GLint		viewportX, viewportY;		///< ViewportXY
			GLsizei		viewportW, viewportH;		///< ViewportWH
			GLint		scissorX, scissorY;			///< ScissorXY
			GLsizei		scissorW, scissorH;			///< ScissorWH
			u16			mode_mat;					///< 現在のマトリクスモード
			u16			mode_blend[4];				///< 現在のブレンドモード [0]…src [1]…dest [2]…src_alpha [3]…dest_alpha
			u16			mode_blend_old[4];			///< 現在のブレンドモード [0]…src [1]…dest [2]…src_alpha [3]…dest_alpha
			u16			mode_shade;					///< 現在のシェードモデル
			KLMatMode	mode_matrix;				///< 現在のマトリクスモード
			u16			filter_type;				///< フィルタタイプ
			u8			drawmode;
			GLfloat		clearR;						///< クリアカラーR
			GLfloat		clearG;						///< クリアカラーG
			GLfloat		clearB;						///< クリアカラーB
			GLfloat		clearA;						///< クリアカラーA
			unsigned	usingClip:1;				///< クリッピング使用中か
			unsigned	rMask:1;
			unsigned	gMask:1;
			unsigned	bMask:1;
			unsigned	aMask:1;
			unsigned	isdepthmask:1;
		
		}GLManager;
		
		
	// F U N C T I O N
	//===================================================================

		void		KLGL_Init( GLuint programid );
		KLGLFlags*	KLGL_GetFlags();
		bl			glsBindFramebuffer( GLuint fbid );
		bl			glsBindFramebufferf( GLuint fbid );
		bl			glsBindRenderbuffer( GLuint rbid );
		bl			glsBindRenderbufferf( GLuint rbid );
		bl			glsBindDepthbuffer( GLuint dbid );
		bl			glsBindDepthbufferf( GLuint dbid );
		bl			glsBindTexture( GLuint tid );
		bl			glsBindTexturef( GLuint tid );
		bl			glsBlend( bl isTrue );
		bl			glsBlendFunc( u16 src, u16 dest );
		bl			glsBlendFuncf( u16 src, u16 dest );
		bl			glsBlendFuncSeparate( u16 src, u16 dest, u16 src_a, u16 dest_a );
		bl			glsBlendFuncSeparatef( u16 src, u16 dest, u16 src_a, u16 dest_a );
		bl			glsCheckFramebuffer();
		void		glsClear();
		void		glsClearc();
		void		glsCleard();
		bl			glsClearColor( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
		bl			glsClearColorf( GLfloat r, GLfloat g, GLfloat b, GLfloat a );
		bl			glsCheckFramebuffer();
		bl			glsColorMask(bl rMask, bl gMask, bl bMask, bl aMask);
		bl			glsColorMaskf(bl rMask, bl gMask, bl bMask, bl aMask);
		bl			glsCullFace( bl isTrue );
		bl			glsDepthFunc( GLenum funcType );
		bl			glsDepthTest( bl isTrue );
		bl			glsDepthMask( GLboolean ismask );
		bl			glsDepthMaskf( GLboolean ismask );
		bl			glsDisable( u16 state );
		bl			glsDisablef( u16 state );
		bl			glsDither( bl isTrue );
		bl			glsEnable( u16 state );
		bl			glsEnablef( u16 state );
		bl			glsFilter( GLenum filter_type );
		bl			glsFilterf( GLenum filter_type );
		void		glsLinkProjectionMatrix();
		bl			glsMatrixMode( KLMatMode matrixMode );
		bl			glsMatrixModef( KLMatMode matrixMode );
		bl			glsOrthMatrix( float l, float r, float top, float bottom, float near, float far, float xoffset, float yoffset );
		bl			glsOrthMatrixf( float l, float r, float top, float bottom, float near, float far, float xoffset, float yoffset  );
		bl			glsPolygonOffsetFactor( bl isTrue );
		bl			glsPolygonOffsetFill( bl isTrue );
		bl			glsPolygonOffsetUnits( bl isTrue );
		bl			glsSampleAlphaToCoverage( bl isTrue );
		bl			glsSampleCoverage( bl isTrue );
		bl			glsScissorTest( bl isTrue );
		bl			glsStencilTest( bl isTrue );
		bl			glsTexParameteri( GLenum filter_type );
		bl			glsTexParameterif( GLenum filter_type );
		bl			glsTexture2D( bl isTrue );

		bl			glsBlendf( bl isTrue );
		bl			glsCullFacef( bl isTrue );
		bl			glsDepthTestf( bl isTrue );
		bl			glsDitherf( bl isTrue );
		bl			glsPolygonOffsetFactorf( bl isTrue );
		bl			glsPolygonOffsetFillf( bl isTrue );
		bl			glsPolygonOffsetUnitsf( bl isTrue );
		bl			glsSampleAlphaToCoveragef( bl isTrue );
		bl			glsSampleCoveragef( bl isTrue );
		bl			glsScissorTestf( bl isTrue );
		bl			glsStencilTestf( bl isTrue );
		bl			glsTexture2Df( bl isTrue );
		bl			glsViewport( GLint x, GLint y, GLsizei w, GLsizei h );
		bl			glsScissor( GLint x, GLint y, GLsizei w, GLsizei h );
		bl			glsVertexAttribPointer(KLShaderHandler* pHandler, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr);
		bl			glsEnableVertexAttribArray( KLShaderHandler* pHandler );
		bl			glsDisableVertexAttribArray( KLShaderHandler* pHandler );
		bl			glsUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
		bl			glsUniformMatrix4fvf( GLint location, GLsizei count, GLboolean transpose, const GLfloat* value );
		u32			KLGL_ValidateTextureSize( u32 n );
		u32			KLGL_GetMaxTextureSize();
		void		KLGL_AddTextureUsage( size_t wxh, GLuint colorType, GLuint byteType );
		void		KLGL_SubTextureUsage( size_t wxh, GLuint colorType, GLuint byteType );


#include "c_extern_close.h"
#endif
