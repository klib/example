//
//  KLDevice.h
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


#ifndef KLib_KLDevice_h
#define KLib_KLDevice_h
#include "c_extern.h"
#include "KLibDefine.h"

// D E F I N E
//====================================

	/// 画面幅ピクセル数
	#define KL_SCW (klib.device.w)

	/// 画面高さピクセル数
	#define KL_SCH (klib.device.h)




// E N U M
//====================================

	/// デバイス定数
	typedef enum{
		
		// Unknown
		KL_UNKNOWN_DEVICE	=	0,
		KL_SIMULATOR		=	1,
		KL_UNKNOWN_IPHONE	=	2,
		KL_UNKNOWN_IPAD		=	3,
		KL_UNKNOWN_IPODTOUCH=	4,
		
		// iPhone
		KL_IPHONE		=	10,
		KL_IPHONE3G		=	30,
		KL_IPHONE3GS	=	35,
		KL_IPHONE4		=	40,
		KL_IPHONE4S		=	45,
		KL_IPHONE5		=	50,
		KL_IPHONE5C		=	52,
		KL_IPHONE5S		=	55,
		KL_IPHONENEW	=	999,
		
		// iPad
		KL_IPAD1G		=	1000,
		KL_IPAD2G		=	2000,
		KL_IPAD3G		=	3000,
		KL_IPAD4G		=	4000,
		
		KL_IPADMINI		=	5000, // non retina
		KL_IPADMINI_R	=	5500, // retina
		KL_IPADAIR		=	6000,
		KL_IPADNEW		=	99999,
		
		// iPodTouch
		KL_IPODTOUCH1G	=	100000,
		KL_IPODTOUCH2G	=	200000,
		KL_IPODTOUCH3G	=	300000,
		KL_IPODTOUCH4G	=	400000,
		KL_IPODTOUCH5G	=	500000,
		
		KL_IPODTOUCHNEW	=	9999999,
		
	}KLDeviceType;




// O B J E C T
//====================================

	typedef struct{

		KLDeviceType	type;						///< 端末種類のID
		float			iosVer;						///< iOSバージョン
		float			scalefactor;				///< ScaleFactor
		bl				iPad:1, iPhone:1, iPodTouch:1,simulator:1;	///< 端末タイプ判別用フラグ
		bl				autolockdisabled:1;			///< オートロック無効化は終了時orバックグラウンド移行時に自動解除
		u32				w,h;						///< 端末画面ピクセル数
		u8				series;						///< 端末のシリーズ番号(iPhone4・iPad4Gなら4)
		char			uiid[KLUIID_SIZE];			///< UIID
		char			language[32];				///< 使用中の言語
		
	}KLDevice;




// F U N C T I O N
//====================================

	KLDeviceType KLDevice_Get();

	void	KLDevice_Init();
	void	KLDevice_InitScreenSize( u32 glview_width, u32 glview_height, bl isPortrait );
	void	KLDevice_SetupUser();

	bl		KLDevice_IsiPad();
	bl		KLDevice_IsiPhone();
	bl		KLDevice_IsiPodTouch();
	bl		KLDevice_IsiPadType( KLDeviceType type );
	bl		KLDevice_IsiPhoneType( KLDeviceType type );
	bl		KLDevice_IsiPodTouchType( KLDeviceType type );
	bl		KLDevice_IsSimulator ();

	u16		KLDevice_GetSeriesNumber();
	u16		KLDevice_GetSeriesNumberType( KLDeviceType type );

	float	KLDevice_GetOSVer();
	float	KLDevice_GetScaleFactor();

	s32		KLDevice_GetOrientationWhenBoot();
	s32		KLDevice_GetOrientation();
	bl		KLDevice_IsPortrait();
	bl		KLDevice_IsLandscape();
	bl		KLDevice_IsPortraitEx(s32 orientation);
	bl		KLDevice_IsLandscapeEx(s32 orientation);

	bl		KLDevice_Is4inch();

	u32		KLDevice_GetW();
	u32		KLDevice_GetH();

	void	KLDevice_Viberate();

	void	KLDevice_ForSimulator();

	char*	KLDevice_GetLanguage();

	void	KLDevice_DisableAutoLock();
	void	KLDevice_EnableAutoLock();

	bl		KLDevice_IsLangEnglish();
	bl		KLDevice_IsLangJapanese();

	#define KL_GetDeviceW KLDevice_GetW
	#define KL_GetDeviceH KLDevice_GetH

#include "c_extern_close.h"
#endif
