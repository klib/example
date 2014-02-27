//
//  KLibDefine.hpp
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

#ifndef KLib_KLibDefine_h
#define KLib_KLibDefine_h

#include "c_extern.h"
#include <CoreFoundation/CFBase.h>
#include <stdio.h>
#include <stdlib.h>
#include "KLConfig.h"

/// KLibHeapDebugでのヒープ確保先情報のサイズ
#define KLIBMEM_PATHLEN (128)

#define KLog NSLog
#define KLLog printf

#define SF(v) (v)*klib.device.scalefactor

/// NULL定義
#ifndef NULL
#define NULL (0)
#endif

#ifndef null
#define null NULL
#endif

/// 真偽定義
//#ifndef TRUE
//#define TRUE (1)
//#endif

//#ifndef FALSE
//#define FALSE (0)
//#endif

typedef enum{
	
	KLTextBorderStyleNone,
	KLTextBorderStyleLine,
	KLTextBorderStyleBezel,
	KLTextBorderStyleRoundedRect
	
} KLTextBorderStyle;
// UITextBorderStyleNone
// UITextBorderStyleLine
// UITextBorderStyleBezel
// UITextBorderStyleRoundedRect

typedef enum {
	
    KLKeyboardTypeDefault,                // Default type for the current input method.
    KLKeyboardTypeASCIICapable,           // Displays a keyboard which can enter ASCII characters, non-ASCII keyboards remain active
    KLKeyboardTypeNumbersAndPunctuation,  // Numbers and assorted punctuation.
    KLKeyboardTypeURL,                    // A type optimized for URL entry (shows . / .com prominently).
    KLKeyboardTypeNumberPad,              // A number pad (0-9). Suitable for PIN entry.
    KLKeyboardTypePhonePad,               // A phone pad (1-9, *, 0, #, with letters under the numbers).
    KLKeyboardTypeNamePhonePad,           // A type optimized for entering a person's name or phone number.
    KLKeyboardTypeEmailAddress,           // A type optimized for multiple email address entry (shows space @ . prominently).
    KLKeyboardTypeDecimalPad,             // A number pad with a decimal point.
    KLKeyboardTypeTwitter,                // A type optimized for twitter text entry (easy access to @ #)
	
} KLKeyboardType;

typedef enum {

	KLReturnKeyDefault,
	KLReturnKeyGo,
	KLReturnKeyGoogle,
	KLReturnKeyJoin,
	KLReturnKeyNext,
	KLReturnKeyRoute,
	KLReturnKeySearch,
	KLReturnKeySend,
	KLReturnKeyYahoo,
	KLReturnKeyDone,
	KLReturnKeyEmergencyCall,
	
} KLReturnKeyType;

typedef enum {

	KLTextFieldViewModeNever,			// 表示しない
	KLTextFieldViewModeAlways,			// 常に表示する
	KLTextFieldViewModeUnlessEditing,	// フォーカスが当たっていない時のみ表示する
	KLTextFieldViewModeWhiteEditing,	// フォーカスが当たっている時のみ表示する

} KLTextFieldViewMode;


// typedefだとキャスト警告が鬱陶しいので、とりあえず緩くdefineで定義しておく
//typedef signed char		s8;
//typedef unsigned char	u8;
//typedef unsigned char	uchar;
//typedef signed short		s16;
//typedef unsigned short	u16;
//typedef unsigned short	ushort;
//typedef signed int		s32;
//typedef unsigned int		u32;
//typedef unsigned int		uint;
//typedef signed long long int	s64;
//typedef unsigned long long int	u64;
//typedef unsigned long long int	ulong;
//#ifndef unichar
//typedef unsigned short unichar;
//#endif

#define s8				signed char		///< 符号あり8bit
#define u8				unsigned char	///< 符号なし8bit
#define s16				signed short	///< 符号あり16bit
#define u16				unsigned short	///< 符号なし16bit
#define s32				signed int		///< 符号あり32bit
#define u32				unsigned int	///< 符号なし32bit
#define s64				signed long long int	///< 符号あり64bit
#define u64				unsigned long long int	///< 符号なし64bit
#define f32				GLfixed			///< OpenGLのfixed型と同義
#define wchar			unsigned short	///< ワイド文字
#define path8			char*			///< charのファイルパス
#define path16			short*			///< ワイド文字のファイルパス
#define GlyphTable		signed short	///< グリフテーブル
#define flg64			u64				///< Bitフラグ64bit
#define flg32			u32				///< Bitフラグ32bit
#define flg16			u16				///< Bitフラグ16bit
#define flg8			u8				///< Bitフラグ8bit
#define bl				char			///< 緩いブーリアン(0==FALSE、非0==TRUE扱いで-1などTRUEの種別化をしたい時に使う)
#define wdeg			u16				///< 角度を表現する型
#define	bin8			char			///< バイナリ
//#ifndef BOOL
//#define BOOL			signed char
//#endif

/// RGBAオブジェクト
typedef struct{ s16 r,g,b,a; }			Rgba;

/// XY座標オブジェクト
typedef struct{ float x, y; }			KLPos2d;

/// XYZ座標オブジェクト
typedef struct{ float x, y, z; }		KLPos3d;

/// 方向と距離で表現する力オブジェクト
typedef struct{ float dist; wdeg dir;}	KLPower;

/// 矩形オブジェクト
typedef struct{ s32 x, y, w, h;}		KLRect;

/// RGBA指定マクロ
#define RGBA(r,g,b,a) (Rgba){r,g,b,a}

/// RECT指定マクロ
#define KLRECT(x,y,w,h) (KLRect){x,y,w,h}

/// 非NULL時のみFree
#define sfree( p ) \
\
	if( p ){\
		free( p );\
		p=NULL;\
	}

/// 非NULL時のみDelete
#define sdelete( p ) \
\
	if( p ){\
		delete p;\
	}

/// 非NULL時のみDelete []
#define sdeletea( p ) \
\
	if( p ){\
		delete [] p;\
	}

/// 非NULL時のみダブルポインタと参照先をfree
#define sfreepp( p, _p_count ) \
	if( p && _p_count>0 ){\
		for( int safe_free_temp=_p_count-1; safe_free_temp>=0; --safe_free_temp ){\
			sfree(p[safe_free_temp]);\
			p[safe_free_temp] = NULL;\
		}\
		sfree(p);\
	}\
	p = NULL

/// Safe release context
#define sCGContextRelease(p) if( p ){ CGContextRelease(p); } p = NULL

/// Safe release image
#define sCGImageRelease(p) if( p ){ CGImageRelease(p); } p = NULL

/// Safe free CF object
#define sCFRelease(p) if( p ){ CFRelease(p); } p = NULL

/// Safe release fontref
#define sCGFontRelease( fontRef )	if( fontRef ){ CGFontRelease(fontRef); } fontRef = NULL

/// zero memset
#define klmemclear(address, sizeof_target) memset( address, 0, sizeof(sizeof_target) )

/// Retina縦向き時の高さ
#define KL_RETINA_DEF_H (960)

/// ４インチ縦向き時の高さ
#define KL_4INCH_DEF_H (1136)

/// ステータスバーの高さ
#define KL_STATUSBAR_DEF_H (20)

/// 絶対値を求める
#define KL_Abs(v)	((v) < 0 ? -(v) : (v))

/// ファイルパスのタイプ(tmpフォルダ)
#define KL_FILEPATH_TYPE_TMP	"tmp"

/// ファイルパスのタイプ(Documentsフォルダ)
#define KL_FILEPATH_TYPE_DOCUMENTS	"Documents"

/// ファイルパスのタイプ(Cachesフォルダ)
#define KL_FILEPATH_TYPE_CACHES		"Caches"

/// abs
#define klabs(x) ((x)<0?-(x):(x))

/// 半角アルファベット大文字セット
#define KLSTRSET_ALPHABET_AZ "ABCDEFGHIJKLMNOPQRSTUVWXYZÅıÇÎ´Ï˝ÓˆÔÒÂ˜Ø∏Œ‰Íˇ¨◊„˛Á¸"

/// 半角アルファベット小文字セット
#define KLSTRSET_ALPHABET_az "abcdefghijklmnopqrstuvwxyzå∫ç∂ƒ©˙∆˚¬µøπœ®ß†√∑≈¥Ω"

/// 半角アルファベット記号セット
#define KLSTRSET_SYMBOL "!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~¥"

/// 半角アルファベット大小セット
#define KLSTRSET_ALPHABET KLSTRSET_ALPHABET_AZ KLSTRSET_ALPHABET_az

/// 半角数字セット
#define KLSTRSET_NUMBER "0123456789"

/// 半角アルファベット大小+数字
#define KLSTRSET_ALPHABET_NUMBER KLSTRSET_ALPHABET KLSTRSET_NUMBER

/// 半角セット
#define KLSTRSET_HANKAKU_ALL KLSTRSET_ALPHABET_NUMBER KLSTRSET_SYMBOL

/// 全角アルファベット大文字セット
#define KLSTRSET_ZEN_ALPHABET_AZ "ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ"

/// 全角アルファベット小文字セット
#define KLSTRSET_ZEN_ALPHABET_az "ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ"

/// 全角アルファベット大小セット
#define KLSTRSET_ZEN_ALPHABET KLSTRSET_ZEN_ALPHABET_AZ KLSTRSET_ZEN_ALPHABET_az

/// 全角ひらがなセット
#define KLSTRSET_ZEN_HIRAGANA "あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほまみむめもやゐゆゑよらりるれろわをんがぎぐげござじずぜぞだぢづでどばびぶべぼぱぴぷぺぽぁぃぅぇぉっゃゅょゎゔ〜ー"

/// 全角カタカナセット
#define KLSTRSET_ZEN_KATAKANA "アイウエオカキクケコサシスセソタチツテトナニヌネノハヒフヘホマミムメモヤヰユヱヨラリルレロワヲンガギグゲゴザジズゼゾダヂヅデドバビブベボパピプペポァィゥェォッャュョヮヴ〜ーー"

/// 全角記号セット
#define KLSTRSET_ZEN_SYMBOL "、。！”＃＄％＆’（）＊＋，−．／：；＜＝＞？＠［＼］＾＿｀｛｜｝〜￥゛゜・∴∵‥…丶〃ゝゞヽヾ○◎●✓※★☆▲△▼▽"

/// 全角各種セット
#define KLSTRSET_ZEN_ALL KLSTRSET_ZEN_ALPHABET KLSTRSET_ZEN_HIRAGANA KLSTRSET_ZEN_KATAKANA KLSTRSET_ZEN_SYMBOL

/// 全角半角セット
#define KLSTRSET_ALL  KLSTRSET_HANKAKU_ALL KLSTRSET_ZEN_ALL

/// サロゲートペアテスト用
#define KLSTRSET_SURROGATEPAIR_TEST "𩸽"

/// 絵文字セット(CombiningCharは今回取り除いています)
#define KLSTRSET_EMOJI  "😄😃😀😊☺😉😍😘😚😗😙😜😝😛😳😁😔😌😒😞😣😢😂😭😪😥😰😅😓😩😫😨😱😠😡😤😖😆😋😷😎😴😵😲😟😦😧😈👿😮😬😐😕😯😶😇😏😑👲👳👮👷💂👶👦👧👨👩👴👵👱👼👸😺😸😻😽😼🙀😿😹😾👹👺🙈🙉🙊💀👽💩🔥✨🌟💫💥💢💦💧💤💨👂👀👃👅👄👍👎👌👊✊✌👋✋👐👆👇👉👈🙌🙏☝👏💪🚶🏃💃👫👪👬👭💏💑👯🙆🙅💁🙋💆💇💅👰🙎🙍🙇🎩👑👒👟👞👡👠👢👕👔👚👗🎽👖👘👙💼👜👝👛👓🎀🌂💄💛💙💜💚❤💔💗💓💕💖💞💘💌💋💍💎👤👥💬👣💭🐶🐺🐱🐭🐹🐰🐸🐯🐨🐻🐷🐽🐮🐗🐵🐒🐴🐑🐘🐼🐧🐦🐤🐥🐣🐔🐍🐢🐛🐝🐜🐞🐌🐙🐚🐠🐟🐬🐳🐋🐄🐏🐀🐃🐅🐇🐉🐎🐐🐓🐕🐖🐁🐂🐲🐡🐊🐫🐪🐆🐈🐩🐾💐🌸🌷🍀🌹🌻🌺🍁🍃🍂🌿🌾🍄🌵🌴🌲🌳🌰🌱🌼🌐🌞🌝🌚🌑🌒🌓🌔🌕🌖🌗🌘🌜🌛🌙🌍🌎🌏🌋🌌🌠⭐☀⛅☁⚡☔❄⛄🌀🌁🌈🌊🎍💝🎎🎒🎓🎏🎆🎇🎐🎑🎃👻🎅🎄🎁🎋🎉🎊🎈🎌🔮🎥📷📹📼💿📀💽💾💻📱☎📞📟📠📡📺📻🔊🔉🔈🔇🔔🔕📢📣⏳⌛⏰⌚🔓🔒🔏🔐🔑🔎💡🔦🔆🔅🔌🔋🔍🛁🛀🚿🚽🔧🔩🔨🚪🚬💣🔫🔪💊💉💰💴💵💷💶💳💸📲📧📥📤✉📩📨📯📫📪📬📭📮📦📝📄📃📑📊📈📉📜📋📅📆📇📁📂✂📌📎✒✏📏📐📕📗📘📙📓📔📒📚📖🔖📛🔬🔭📰🎨🎬🎤🎧🎼🎵🎶🎹🎻🎺🎷🎸👾🎮🃏🎴🀄🎲🎯🏈🏀⚽⚾🎾🎱🏉🎳⛳🚵🚴🏁🏇🏆🎿🏂🏊🏄🎣☕🍵🍶🍼🍺🍻🍸🍹🍷🍴🍕🍔🍟🍗🍖🍝🍛🍤🍱🍣🍥🍙🍘🍚🍜🍲🍢🍡🍳🍞🍩🍮🍦🍨🍧🎂🍰🍪🍫🍬🍭🍯🍎🍏🍊🍋🍒🍇🍉🍓🍑🍈🍌🍐🍍🍠🍆🍅🌽🏠🏡🏫🏢🏣🏥🏦🏪🏩🏨💒⛪🏬🏤🌇🌆🏯🏰⛺🏭🗼🗾🗻🌄🌅🌃🗽🌉🎠🎡⛲🎢🚢⛵🚤🚣⚓🚀✈💺🚁🚂🚊🚉🚞🚆🚄🚅🚈🚇🚝🚋🚃🚎🚌🚍🚙🚘🚗🚕🚖🚛🚚🚨🚓🚔🚒🚑🚐🚲🚡🚟🚠🚜💈🚏🎫🚦🚥⚠🚧🔰⛽🏮🎰♨🗿🎪🎭📍🚩🇯🇵🇰🇷🇩🇪🇨🇳🇺🇸🇫🇷🇪🇸🇮🇹🇷🇺🇬🇧1⃣2⃣3⃣4⃣5⃣6⃣7⃣8⃣9⃣0⃣🔟🔢#⃣🔣⬆⬇⬅➡🔠🔡🔤↗↖↘↙↔↕🔄◀▶🔼🔽↩↪ℹ⏪⏩⏫⏬⤵⤴🆗🔀🔁🔂🆕🆙🆒🆓🆖📶🎦🈁🈯🈳🈵🈴🈲🉐🈹🈺🈶🈚🚻🚹🚺🚼🚾🚰🚮🅿♿🚭🈷🈸🈂Ⓜ🛂🛄🛅🛃🉑㊙㊗🆑🆘🆔🚫🔞📵🚯🚱🚳🚷🚸⛔✳❇❎✅✴💟🆚📳📴🅰🅱🆎🅾💠➿♻♈♉♊♋♌♍♎♏♐♑♒♓⛎🔯🏧💹💲💱©®™〽〰🔝🔚🔙🔛🔜❌⭕❗❓❕❔🔃🕛🕧🕐🕜🕑🕝🕒🕞🕓🕟🕔🕠🕕🕖🕗🕘🕙🕚🕡🕢🕣🕤🕥🕦✖➕➖➗♠♥♣♦💮💯✔☑🔘🔗➰🔱🔲🔳◼◻◾◽▪▫🔺⬜⬛⚫⚪🔴🔵🔻🔶🔷🔸🔹"

#include "c_extern_close.h"
#endif
