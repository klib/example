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

#ifndef	___KL_LABEL_H___
#define	___KL_LABEL_H___

#include "c_extern.h"
	
	// I N C L U D E
	//=============================================================
	#include "KLibDefine.h"
	#include "KLib.h"
	#include "KLSprite.h"
	#include "KLFont.h"
	#include <CoreGraphics/CGContext.h>
	#include <CoreFoundation/CFString.h>
	#include <stdlib.h>

	// E N U M
	//=============================================================
	typedef enum
	{
		
		KLLABEL_ALIGN_L,
		KLLABEL_ALIGN_CENTER,
		KLLABEL_ALIGN_R,
		
	}KLLabelAlign;
	
	// S T R U C T U R E
	//=============================================================
	
	typedef struct{
		
		Rgba bg;
		Rgba fill;
		Rgba stroke;
		
	}KLLabelColor;
	
	enum
	{
		KLLabel_DRAWINFO_COLOR,
		KLLabel_DRAWINFO_UNDERLINE,
	};
	
	typedef struct
	{
		u32		mode;
		u32		offset;
		u32		length;
		Rgba	rgba;
	}
	KLLabelDrawInfo;

/*	ラベルについて
 *	
 *	ラベルは任意フォントをスプライト化するテキストスプライト的なオブジェクトです。
 *	OpenGLで手軽に文字列を描画できる機能を目的としています。
 *
 *	ラベルは２種類存在し、文字列をただの１枚絵として使用するラベルと
 *	もう１つは、あらかじめ用意した文字列を組み合わせて文章などに使えるマッピングラベルが
 *	選択できます。
 *
 *	前者は、一度作成したら文字列は変えられませんが、文字列全体が１つのスプライトとなるので、
 *	スプライトで行えるカラー変更や縮尺、回転などの変更を文字列全体にかける事ができます。
 *	ボタンのラベルなど一定の文字と場所が決まっているものに使用するのがよいでしょう。
 *
 *	後者は、使用したい文字をあらかじめスプライトにマッピングしておき、
 *	マッピングされている文字列なら何回でも文字列を組み替えて描画できます。
 *	例えば、「0123456789ダメージゲGだよ」とマッピングしたら(※マッピングに文字の順番は関係ありません)
 *	「123ダメージ!」や、「1ゲージ 5000Gだよ」など登録されている文字を組み合わせた
 *	文字列を任意に表示できます。"ABCDEFGAAA"など重複文字がたくさんあっても内部でトリミングされるので
 *	ゲーム中のセリフ文字列をすべて渡してマップを作成といった事が行えます。
 *	最大GL_MAX_TEXTURE_SIZE*GL_MAX_TEXTURE_SIZEに収まる文字
 *	かつフォントに含まれる文字であれば制限なくマッピングできます。
 *	あらかじめマッピングする文字をどう管理するかにもよりますが、
 *	会話などで表示するメッセージや、ダメージなど不特定に変化する文字列におすすめします。
 *
 */

	typedef struct{
		
		size_t				len;				///< 文字列長
		KLLabelColor		color;				///< 基本色
		CGContextRef		context;			///< コンテキスト
		CGTextDrawingMode	drawMode;			///< スプライト作成時に文字をどう描画するか(デフォルトは線なし塗りのみ)
		KLSprite			sprite;				///< 出力先スプライト
		wchar*				pMap;				///< マッピング時のバイナリサーチ用昇順文字列バッファ
		u32*				pMap32;				///< マッピング時のバイナリサーチ用昇順文字列バッファ(u32文字列)
		GLfloat*			pUv;				///< 各文字のUV ピクセル値
		CGFontRef			reffont;			///< フォント
		KLLabelDrawInfo*	pInfo;				///< 文字ごとに色を変えたい時などの描画情報バッファ
		u32					nInfo, nInfoMax;	///< 使用中の描画情報バッファ数と最大数
		s32					x;					///< 描画位置X
		s32					y;					///< 描画位置Y
		u32					w;					///< 描画幅
		u32					h;					///< 描画高さ
		u16					fontsize;			///< 文字サイズ
		u16					lineSpacing;		///< 行間スペース
		u32					leading;			///< リーディング
		float				originsize;			///< 拡縮用保持サイズ(float)
		float				letterSpacing;		///< 文字間隔X
		float				descent;			///< ディセンダ
		u16					spacew;				///< 空白幅
		bl					isAntialias:1;		///< スプライト作成時にアンチエイリアスを有効にするか
		bl					isSmoothing:1;		///< スプライト作成時にスムースを有効にするか
		bl					isMapping:1;		///< マッピングスプライトにするか 一枚ラベルにするか
		bl					isflipx:1;			///< 左右反転する
	
	}KLLabel;

	void		KLLabel_Init( KLLabel* p );
	void		KLLabel_Unload( KLLabel* pText );

	void		KLLabel_SetFontSize( KLLabel* p, u16 pixel );

	bl			KLLabel_ToSprite( KLLabel* p, char* pCstring, KLFont* pFont );
	bl			KLLabel_ToSpriteKchar( KLLabel* p, wchar* pWchar, KLFont* pFont );

	bl			KLLabel_InitSystem();
	void		KLLabel_UnloadSystem();

	void		KLLabel_SetColorBG( KLLabel* p, float r, float g, float b, float a );
	void		KLLabel_SetColorFill( KLLabel* p, float r, float g, float b, float a );
	void		KLLabel_SetColorStroke( KLLabel* p, float r, float g, float b, float a );

	s32			KLLabel_Draw( KLLabel* p, char* pChar, u32 len, float x, float y, float maxw, Rgba rgba, KLLabelAlign align_x_type );
	s32			KLLabel_DrawWchar( KLLabel* p, wchar* pWchar, u32 len, float x, float y, float maxw, Rgba rgba, KLLabelAlign align_x_type );

	void		KLLabel_InitEmoji( char* pEmojiStr );

	bl			KLLabel_IsDrawableCharacter32( KLLabel* p, u32 c32 );
	s32			KLLabel_GetDrawCharLastX();
	s32			KLLabel_GetDrawCharLastY();
	void		KLLabel_DrawCursorNext( bl isNextDraw, u32 w, Rgba rgba );
	void		KLLabel_NextWordWrap( bl isWordwrap );
	bl			KLLabel_AddInfo( KLLabel* p, u32 mode, u32 offset, u32 len, Rgba rgba );
	void		KLLabel_ResetInfo( KLLabel* );
	void		KLLabel_IgnoreDrawInfo();
	void		KLLabel_UseDrawInfo();
	void		KLLabel_NextIsNotWrap( void* pTextCanvas );
	void		KLLabel_SetCustomLineH( u32 newline_h );
	
#include "c_extern_close.h"

#endif