//
//  KLDevice.c
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

#include "KLib.h"
#include "KLDevice.h"
#include <sys/sysctl.h>

//=========================================================
/**
 *	@brief	KLib内のKLibDeviceを初期化
 */
//=========================================================
void KLDevice_Init(){
	
	KLDevice_SetupUser();
	
	klib.device.type		= KLDevice_Get();
	
	klib.device.iPad		= KLDevice_IsiPad();
	klib.device.iPhone		= KLDevice_IsiPhone();
	klib.device.iPodTouch	= KLDevice_IsiPodTouch();
	
	klib.device.series		= KLDevice_GetSeriesNumber();
	
	klib.device.iosVer		= KLDevice_GetOSVer();
	klib.device.scalefactor	= KLDevice_GetScaleFactor();
	
	klib.device.autolockdisabled = FALSE;
	
}

//=========================================================
/**
 *	@brief	起動中のデバイスタイプのシリーズ番号を返す<br/>
 *			不明時は0、未定義で最新と思われる場合は0xFFFFを返す
 */
//=========================================================
u16 KLDevice_GetSeriesNumber(){
	
	return KLDevice_GetSeriesNumberType(klib.device.type);
	
}

//=========================================================
/**
 *	@brief	指定デバイスタイプのシリーズ番号を返す<br/>
 *			不明時は0、未定義で最新と思われる場合は0xFFFFを返す
 */
//=========================================================
u16 KLDevice_GetSeriesNumberType( KLDeviceType type ){
	
	switch( type )
	{
		case KL_UNKNOWN_DEVICE:
		case KL_UNKNOWN_IPAD:
		case KL_UNKNOWN_IPHONE:
		case KL_UNKNOWN_IPODTOUCH:
			return 0;
			
		case KL_IPAD1G:
		case KL_IPHONE:
		case KL_IPODTOUCH1G:
			return 1;
			
		case KL_IPAD2G:
		case KL_IPODTOUCH2G:
			return 2;
			
		case KL_IPAD3G:
		case KL_IPODTOUCH3G:
		case KL_IPHONE3G:
		case KL_IPHONE3GS:
			return 3;
			
		case KL_IPAD4G:
		case KL_IPODTOUCH4G:
		case KL_IPHONE4:
		case KL_IPHONE4S:
			return 4;
			
		case KL_IPADMINI:
		case KL_IPADMINI_R:
		case KL_IPADAIR:
		case KL_IPODTOUCH5G:
		case KL_IPHONE5:
		case KL_IPHONE5C:
		case KL_IPHONE5S:
			return 5;
			
		default:
			break;
			
	}
	
	return 0xFFFF;
	
}

//=========================================================
/**
 *	@brief	起動中のデバイスタイプはiPadか
 */
//=========================================================
bl KLDevice_IsiPad(){
	
	return KLDevice_IsiPadType(klib.device.type);
	
}

//=========================================================
/**
 *	@brief	起動中のデバイスタイプはiPhoneか
 */
//=========================================================
bl KLDevice_IsiPhone(){
	
	return KLDevice_IsiPhoneType(klib.device.type);
}


//=========================================================
/**
 *	@brief	起動中のデバイスタイプはiPodTouchか
 */
//=========================================================
bl KLDevice_IsiPodTouch(){
	
	return KLDevice_IsiPodTouchType(klib.device.type);
	
}

//=========================================================
/**
 *	@brief	指定デバイスタイプはiPadか
 */
//=========================================================
bl KLDevice_IsiPadType( KLDeviceType type ){
	
	switch( type )
	{
		case KL_IPAD1G:
		case KL_IPAD2G:
		case KL_IPAD3G:
		case KL_IPAD4G:
		case KL_IPADAIR:
		case KL_IPADMINI:
		case KL_IPADMINI_R:
		case KL_IPADNEW:
		case KL_UNKNOWN_IPAD:
			
			return TRUE;
		
		// コンパイラの警告回避の為に空でも設置
		default:
		
			break;
	
	}
	
	return FALSE;
	
}

//=========================================================
/**
 *	@brief	指定デバイスタイプはiPhoneか
 */
//=========================================================
bl KLDevice_IsiPhoneType( KLDeviceType type ){
	
	switch( type )
	{
		case KL_IPHONE:
		case KL_IPHONE3G:
		case KL_IPHONE3GS:
		case KL_IPHONE4:
		case KL_IPHONE4S:
		case KL_IPHONE5:
		case KL_IPHONE5C:
		case KL_IPHONE5S:
		case KL_IPHONENEW:
		case KL_UNKNOWN_IPHONE:
		
			return TRUE;
			
		// コンパイラの警告回避の為に空でも設置
		default:
			
			break;
			
	}
	
	return FALSE;
	
}


//=========================================================
/**
 *	@brief	指定デバイスタイプはiPodTouchか
 */
//=========================================================
bl KLDevice_IsiPodTouchType( KLDeviceType type ){
	
	switch( type )
	{
		case KL_IPODTOUCH1G:
		case KL_IPODTOUCH2G:
		case KL_IPODTOUCH3G:
		case KL_IPODTOUCH4G:
		case KL_IPODTOUCH5G:
		case KL_IPODTOUCHNEW:
		case KL_UNKNOWN_IPODTOUCH:
			
			return TRUE;
			
		// コンパイラの警告回避の為に空でも設置
		default:
			
			break;
			
	}
	
	return FALSE;
	
}

//=========================================================
/**
 *	@brief	実行中のデバイスが4インチデバイスかを取得する
 */
//=========================================================
inline bl KLDevice_Is4inch(){
	
	return klib.device.iPhone && klib.device.series>=5;
	
}

//=========================================================
/**
 *	@brief	実行中のデバイス情報を元に作成したKLDeviceTypeを取得する
 */
//=========================================================
KLDeviceType KLDevice_Get(){
	
	/*
	 
	 Platforms
	 
	 iFPGA ->        ??
	 
	 iPhone1,1 ->    iPhone 1G, M68
	 iPhone1,2 ->    iPhone 3G, N82
	 iPhone2,1 ->    iPhone 3GS, N88
	 iPhone3,1 ->    iPhone 4/AT&T, N89
	 iPhone3,2 ->    iPhone 4/Other Carrier?, ??
	 iPhone3,3 ->    iPhone 4/Verizon, TBD
	 iPhone4,1 ->    (iPhone 4S/GSM), TBD
	 iPhone4,2 ->    (iPhone 4S/CDMA), TBD
	 iPhone4,3 ->    (iPhone 4S/???)
	 iPhone5,1 ->    iPhone 5, TBD
	 iPhone6,1 ->    iPhone 5s
	 iPhone6,2 ->    iPhone 5s
	 
	 iPod1,1   ->    iPod touch 1G, N45
	 iPod2,1   ->    iPod touch 2G, N72
	 iPod2,2   ->    Unknown, ??
	 iPod3,1   ->    iPod touch 3G, N18
	 iPod4,1   ->    iPod touch 4G, N80
	 iPod5,1   ->	 iPod touch 5G
	 
	 iPad1,1   ->    iPad, WiFi and 3G, K48
	 iPad2,1   ->    iPad 2, WiFi, K93
	 iPad2,2   ->    iPad 2, GSM 3G, K94
	 iPad2,3   ->    iPad 2, CDMA 3G, K95
	 iPad2,4   ->    iPad 2, Wifi only(2,4)
	 
	 iPad3,1   ->    (iPad 3G, WiFi)
	 iPad3,2   ->    (iPad 3G, GSM)
	 iPad3,3   ->    (iPad 3G, CDMA)
	 iPad3,4   ->    (iPad 3G, WiFi)
	 iPad3,5   ->    (iPad 3G, WiFi/AT&T/GPS)
	 iPad3,6   ->    (iPad 3G, WiFi/Verizon & Sprint/GPS)
	
	 iPad2,5   ->	 (iPad mini, WiFi.1st Gen)
	 iPad2,6   ->	 (iPad mini, WiFi/AT&T/GPS)
	 iPad2,7   ->	 (iPad mini, WiFi/Verizon & Sprint/GPS)
	 
	 iPad4,1   ->    (iPad Air, WiFi)
	 iPad4,2   ->    (iPad Air, GSM)
	 
	 iPad4,4   ->    (iPad mini, Retina/2G WiFi)
	 iPad4,5   ->    (iPad mini, Retina/2G WiFi Cellular)

	 
	 AppleTV2,1 ->   AppleTV 2, K66
	 AppleTV3,1 ->   AppleTV 3, ??
	 
	 i386, x86_64 -> iPhone Simulator
	 
	 Refered from:
	 http://www.everyi.com/by-identifier/ipod-iphone-ipad-specs-by-model-identifier.html
	 
	 */
	
	KLDeviceType	type = KL_UNKNOWN_DEVICE;
	size_t			size;
	char			buf[256];
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	sysctlbyname("hw.machine", buf, &size, NULL, 0);
	
	if( size>=7 && ((buf[0]=='i' && buf[1]=='3') || (buf[0]=='x' && buf[1]=='8')) )
	{
		// Simulator
		KLDevice_ForSimulator();
	}
	else if( size>=7 && buf[0]=='i' )
	{
		// iPhone
		if( buf[2]=='h' )
		{
			type = KL_UNKNOWN_IPHONE;
			
			// 2G/3G
			if( buf[5+1]=='1' )
			{
				// 2G
				if( buf[5+3]=='1' )
				{
					type = KL_IPHONE;
				
				// 3G
				}
				else if( buf[5+3]=='2' )
				{
					type = KL_IPHONE3G;
				}
				
			// 3GS
			}
			else if( buf[5+1]=='2' )
			{
				type = KL_IPHONE3GS;
				
			// 4
			}
			else if( buf[5+1]=='3' )
			{
				type = KL_IPHONE4;
				
			// 4S
			}
			else if( buf[5+1]=='4' )
			{
				type = KL_IPHONE4S;
				
			// 5
			}
			else if( buf[5+1]=='5' )
			{
				type = KL_IPHONE5;
			
				if ( buf[5+2]==',' && (buf[5+3]=='3'||buf[5+3]=='4') )
				{
					// 5c
					type = KL_IPHONE5C;
					
				}
				
			// 5s
			}
			else if( buf[5+1]=='6' )
			{
				type = KL_IPHONE5S;
				
			// 最新？
			//@todo:雑な判別をもっとちゃんとする
			}
			else if( buf[size-3]>='6' && buf[size-3]<='9' )
			{
				type = KL_IPHONENEW;
			}
			
		// iPad
		}
		else if( buf[2]=='a' )
		{
			type = KL_UNKNOWN_IPAD;
			
			// 1G
			if( buf[5+1]=='1' )
			{
				type = KL_IPAD1G;
				
			// 2G / mini non retina
			}
			else if( buf[5+1]=='2' )
			{
				type = KL_IPAD2G;
				
				if ( buf[5+2]==',' && (buf[5+3]=='5'||buf[5+3]=='6'||buf[5+3]=='7') )
				{
					// mini non retina
					type = KL_IPADMINI;
					
				}
				
			// 3G
			}
			else if( buf[5+1]=='3' )
			{
				type = KL_IPAD3G;
				
			// AIR / mini retina
			}
			else if( buf[5+1]=='4' )
			{
				type = KL_IPADAIR;
				
				if ( buf[5+2]==',' && (buf[5+3]=='4'||buf[5+3]=='5') )
				{
					// mini retina
					type = KL_IPADMINI_R;
					
				}
				
			// 最新？
			}
			else if( buf[size-3]>='5' && buf[size-3]<='9' )
			{
				type = KL_IPADNEW;
			}
			
		// touch
		}
		else if( buf[2]=='o' )
		{
			type = KL_UNKNOWN_IPAD;
			
			// 1G
			if( buf[size-3]=='1' )
			{
				type = KL_IPODTOUCH1G;
				
			// 2G
			}
			else if( buf[size-3]=='2' )
			{
				type = KL_IPODTOUCH2G;
				
			// 3G
			}
			else if( buf[size-3]=='3' )
			{
				type = KL_IPODTOUCH3G;
				
			// 4G
			}
			else if( buf[size-3]=='4' )
			{
				type = KL_IPODTOUCH4G;
				
			// 5G
			}
			else if( buf[size-3]=='5' )
			{
				type = KL_IPODTOUCH5G;
				
			// 最新？
			}
			else if( buf[size-3]>='5' && buf[size-3]<='9' )
			{
				type = KL_IPODTOUCHNEW;
			}
			
		}
		
	}

	return type;
	
}


//=========================================================
/**
 *	@brief	シミュレータで実行中かを返す
 */
//=========================================================
bl KLDevice_IsSimulator () {
	
	char deviceName[64];
	size_t size;
	sysctlbyname("hw.machine", NULL, &size, NULL, 0);
	sysctlbyname("hw.machine", deviceName, &size, NULL, 0);
	return (!strcmp(deviceName,"i386")||!strcmp(deviceName,"x86_64"));

}


//=========================================================
/**
 *	@brief	デバッグ端末かを返す
 */
//=========================================================
bl KLDevice_IsDeveloperDevice() {
	
#ifdef DEBUG
	return TRUE;
#else
	return KLDevice_IsSimulator();
#endif
	
}

//=========================================================
/**
 *	@brief	端末の言語設定が英語かを返す
 */
//=========================================================
inline bl KLDevice_IsLangEnglish(){
	
	return !strcmp(klib.device.language,"en");
	
}

//=========================================================
/**
 *	@brief	端末の言語設定が日本語かを返す
 */
//=========================================================
inline bl KLDevice_IsLangJapanese(){
	
	return !strcmp(klib.device.language,"ja");
	
}
