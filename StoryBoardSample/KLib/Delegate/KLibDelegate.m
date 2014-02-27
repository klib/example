//
//  KLib.m
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

#import "KLibDelegate.h"
#import "KLString.h"
#import "KLView.h"
#import "LKKeychain.h"
#import <UIKit/UIKit.h>
#import <AudioToolbox/AudioServices.h>

//===========================================================
/**
 *	@brief	UUIDを作成しキーチェーンに保存する / すでに作成済みの場合はそれを読み込む
 *
 *	@return	正常終了時TRUE / 失敗時FALSE
 */
//===========================================================
bl KLUUID_Init(){
	
	NSString* ps = [LKKeychain getPasswordWithAccount:@"kluuid" service:@KLUUID_SERVICE];
	//NSDictionary* dict = accounts
	
	// 新規作成
	if( ps==nil || !strcmp( [ps UTF8String], "00000000-0000-0000-0000-000000000000" ) )
	{
		KLLog("[KLUUID] Generate new uuid.\n");
		CFUUIDRef	cfUuid	= CFUUIDCreate(nil);
#if __has_feature(objc_arc)
		NSString*	nstr	= (NSString*)CFBridgingRelease(CFUUIDCreateString(nil, cfUuid));
#else
		NSString*	nstr	= (NSString*)CFUUIDCreateString(nil, cfUuid);
#endif
		char* pCstr= (char*)[nstr cStringUsingEncoding:NSUTF8StringEncoding];
		strcpy( klib.device.uiid, pCstr );
		sCFRelease(cfUuid);
#if __has_feature(objc_arc)
#else
		[ nstr release ];
#endif
		if( strcmp( klib.device.uiid, "00000000-0000-0000-0000-000000000000" ) )
		{
			[LKKeychain updatePassword:[NSString stringWithCString:klib.device.uiid encoding:NSUTF8StringEncoding] account:@"kluuid" service:@KLUUID_SERVICE];
			KLLog("[KLUUID] Updated.\n");
		}
		else
		{
			KLLog("[KLUUID] !!! invalid UUID.\n");
			return FALSE;
		}
		
		KLLog("Generated UUID:\n\t%s\n", klib.device.uiid);
	}
	else
	{
		char* pCstr= (char*)[ps cStringUsingEncoding:NSUTF8StringEncoding];
		strcpy( klib.device.uiid, pCstr );
		KLLog("Loaded UUID from keychains:\n\t%s\n", klib.device.uiid);
	}
	
	return TRUE;
}

//=========================================================
/**
 *	@brief	端末のオートロックを無効にする
 *
 */
//=========================================================
void KLDevice_DisableAutoLock()
{
	klib.device.autolockdisabled = TRUE;
	[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
}

//=========================================================
/**
 *	@brief	端末のオートロックを有効にする
 */
//=========================================================
void KLDevice_EnableAutoLock()
{
	klib.device.autolockdisabled = FALSE;
	[[UIApplication sharedApplication] setIdleTimerDisabled:NO];
}

//=========================================================
/**
 *	@brief	現在端末が縦向きかを取得する
 *
 *	@return	縦持ち状態ならTRUE
 */
//=========================================================
bl KLDevice_IsPortraitEx( s32 orientation ){
	
	if( orientation==0 )
	{
		orientation = KLDevice_GetOrientationWhenBoot();
	}
	
	return	orientation==UIDeviceOrientationPortrait || orientation==UIDeviceOrientationPortraitUpsideDown || orientation==UIDeviceOrientationFaceUp || orientation==UIDeviceOrientationFaceDown;
	
}

//=========================================================
/**
 *	@brief	指定向き情報が横向きかを取得する
 *
 *	@param	UIInterfaceOrientationをs32化した値
 *
 *	@return	横持ち状態ならTRUE
 */
//=========================================================
bl KLDevice_IsLandscapeEx( s32 orientation ){
		
	if( orientation==0 )
	{
		orientation = KLDevice_GetOrientationWhenBoot();
	}
	
	return	orientation==UIDeviceOrientationLandscapeLeft || orientation==UIDeviceOrientationLandscapeRight || orientation==UIDeviceOrientationFaceUp || orientation==UIDeviceOrientationFaceDown;
	
}


//=========================================================
/**
 *	@brief	現在端末が縦向きかを取得する
 *
 *	@return	縦持ち状態ならTRUE
 */
//=========================================================
bl KLDevice_IsPortrait(){
	
	return KLDevice_IsPortraitEx( KLDevice_GetOrientation() );
	
}

//=========================================================
/**
 *	@brief	現在端末が横向きかを取得する
 *
 *	@return	横持ち状態ならTRUE
 */
//=========================================================
bl KLDevice_IsLandscape(){
	
	return KLDevice_IsLandscapeEx( KLDevice_GetOrientation() );

}

//=========================================================
/**
 *	@brief	KLib経由で画面サイズを取得するための情報を初期化
 *
 *	@param	glview_width	OpenGLを表示するビューの幅　(0で全画面)
 *	@param	glview_height	OpenGLを表示するビューの高さ(0で全画面)
 *	@param	isPortrait	端末を縦持ち状態で計算するならTRUE / 横持ち状態で計算ならFALSE
 */
//=========================================================
void KLDevice_InitScreenSize( u32 glview_width, u32 glview_height, bl isPortrait ){
	
	CGRect	dframe	= [[UIScreen mainScreen] bounds];
	//CGRect	vframe	= [[UIScreen mainScreen] applicationFrame];
	CGRect	vframe	= [(KLView*)pKLView frame];
	float	scale	= [UIScreen mainScreen].scale;

	klib.view.w = vframe.size.width* scale;
	klib.view.h = vframe.size.height* scale;
	
	if ( isPortrait )
	{
		klib.device.w = dframe.size.width * scale;
		klib.device.h = dframe.size.height* scale;
	}
	else
	{
		klib.device.h = dframe.size.width * scale;
		klib.device.w = dframe.size.height* scale;
	}

}

//========================================================
/**
 *	@brief	Path情報の初期化
 */
//========================================================
void KLPath_Init(){

	NSString*	nstr			= [ [ [NSBundle mainBundle] resourcePath ] stringByAppendingPathComponent : @"" ];
	NSArray*	pCacheArray		= NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
	NSArray*	pDocArray		= NSSearchPathForDirectoriesInDomains(NSDocumentationDirectory, NSUserDomainMask, YES);
	NSString*	pAppDir			= nstr;
	char*		pAppRoot		= (char*)[ pAppDir cStringUsingEncoding : NSUTF8StringEncoding ];
	char*		pCacheRoot		= (char*)[ [pCacheArray objectAtIndex:0] cStringUsingEncoding : NSUTF8StringEncoding ];
	char*		pDocRoot		= (char*)[ [pDocArray objectAtIndex:0] cStringUsingEncoding:NSUTF8StringEncoding];
	char*		pTmpRoot		= (char*)[ NSTemporaryDirectory() cStringUsingEncoding:NSUTF8StringEncoding];
				
	strcpy( klib.path.appRoot,	pAppRoot );
	strcpy( klib.path.cacheRoot,pCacheRoot );
	strcpy( klib.path.docRoot,	pDocRoot );
	strcpy( klib.path.tmpRoot,	pTmpRoot );
		
}

//========================================================
/**
 *	@brief	UUIDのセットアップ（作成or読み込み）
 */
//========================================================
void KLDevice_SetupUser(){

	KLDevice_GetLanguage();
	KLUUID_Init();
	
}

//========================================================
/**
 *	@brief	デバイスの画面ピクセル数(幅)を取得
 */
//========================================================
inline u32 KLDevice_GetW(){
	
	return klib.device.w;
	
}

//========================================================
/**
 *	@brief	デバイスの画面ピクセル数(幅)を取得
 *
 *	@param	withStatusBar	ステータスバーも含めたサイズならTRUE
 */
//========================================================
inline u32 KLDevice_GetH(){
	
	return klib.device.h;
	
}

//========================================================
/**
 *	@brief	iOSVersionを返す
 */
//========================================================
float KLDevice_GetOSVer(){
	
	return [[[UIDevice currentDevice] systemVersion] floatValue];

}

//========================================================
/**
 *	@brief	ScaleFactorを返す
 */
//========================================================
float KLDevice_GetScaleFactor(){

	return [[UIScreen mainScreen] scale];

}

//=========================================================
/**
 *	@brief	シミュレータ用の端末判別
 */
//=========================================================
void KLDevice_ForSimulator()
{
	CGRect		r = [[UIScreen mainScreen]bounds];
	//NSString*	n = [[UIDevice currentDevice] name];
	NSString*	m = [[UIDevice currentDevice] model];
	//NSString*	l = [[UIDevice currentDevice] localizedModel];
	//float		s = KLDevice_GetScaleFactor();
	
	if(r.size.width==568 || r.size.height==568)
	{
		klib.device.simulator = 1;
		klib.device.iPhone	= 1;
		klib.device.iPad	= 0;
		klib.device.iPodTouch = 0;
		klib.device.type	= KL_IPHONE5;
		klib.device.series	= KLDevice_GetSeriesNumber();
	}
	else if(r.size.width==480 || r.size.height==480)
	{
		klib.device.simulator = 1;
		klib.device.iPad	= [m hasPrefix:@"iPad"];
		klib.device.iPhone	= !klib.device.iPad;
		klib.device.iPodTouch = 0;
		klib.device.type	= klib.device.iPad?KL_IPAD2G:KL_IPHONE4;
		klib.device.series	= KLDevice_GetSeriesNumber();
	}
	
}

//=========================================================
/**
 *	@brief	簡易バイブレーション
 */
//=========================================================
void KLDevice_Viberate()
{
	AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

//======================================================================
/**
 *	@brief	アプリ起動中のオートスリープ有無を設定
 *
 *	@param	isEnableAutoSleep	オートスリープを許可する場合はTRUE
 */
//======================================================================
void KL_AutoSleep(bl isEnableAutoSleep){
	
	[UIApplication sharedApplication].idleTimerDisabled = !isEnableAutoSleep;

}

//========================================================
/**
 *	@brief	c/cppソースからUIGraphicsPushContextをコール
 *
 *	@param	context	CGContextRef
 */
//========================================================
void KLUIGraphicsPushContext( CGContextRef context ){

	UIGraphicsPushContext(context);

}

//========================================================
/**
 *	@brief	c/cppソースからUIGraphicsPopContextをコール
 *
 *	@param	context	CGContextRef
 */
//========================================================
void KLUIGraphicsPopContext(){
	
	UIGraphicsPopContext();
	
}

static UIInterfaceOrientation kl_bootot = (UIInterfaceOrientation)0;

//========================================================
/**
 *	@brief	起動時の端末の現在の向きを返す
 *
 *	@return UIInterfaceOrientationをs32化した値
 */
//========================================================
s32 KLDevice_GetOrientationWhenBoot(){
	
	if( kl_bootot==UIDeviceOrientationUnknown )
	{
		kl_bootot = [[UIApplication sharedApplication] statusBarOrientation];
	}
	
	return kl_bootot;
	
}
//========================================================
/**
 *	@brief	端末の現在の向きを返す
 *
 *	@return UIInterfaceOrientationをs32化した値
 */
//========================================================
s32 KLDevice_GetOrientation(){

	return [[UIDevice currentDevice] orientation];
	
}

//=============================================
/**
 *	@brief	現在の時刻をYYYYMMDDhhmm形式の数値で返す
 *
 *	@return	日付、時刻を64bit数値化したもの
 */
//=============================================
u64	KL_GetNowU64(){
	
	NSDateFormatter* format = [[NSDateFormatter alloc] init];
	format.dateFormat		= @"yyyyMMddHHmmss";
	NSString* pNs			= [format stringFromDate:[NSDate date]];
	char* pStr				= (char*)[pNs UTF8String];
	u64 ret					= KLString_ParseInt(pStr, FALSE);
#if __has_feature(objc_arc)
#else
	[format release];
#endif
	return ret;
	
}

//=============================================
/**
 *	@brief	現在端末に設定されている言語情報を返す<br/>
 *			一度使用後は、klib.device.languageから取得可能
 *
 *	@return	設定言語識別文字列
 */
//=============================================
char* KLDevice_GetLanguage(){
	
	NSArray *languages = [NSLocale preferredLanguages];
	char* pStr = (char*)[ [languages objectAtIndex:0] cStringUsingEncoding : NSUTF8StringEncoding ];
	strcpy( klib.device.language, pStr );
	
	return klib.device.language;
}

//=============================================
/**
 *	@brief	アプリがバックグラウンド処理に入った時にライブラリが行う処理
 */
//=============================================
void KL_DidEnterBackground(){
	
	/*
	if( KLInAppPurchase_GetInstance() )
	{
		KLPurchase_CancelGetList();
		[[SKPaymentQueue defaultQueue] removeTransactionObserver:KLInAppPurchase_GetInstance()];
	}
	*/
	
	KLView_DidEnterBackground();
	
	if( klib.device.autolockdisabled )
	{
		KLDevice_EnableAutoLock();
	}
	
	// シーン関係なく通知
	if( klib.func.callbackGotoBackground )
	{
		klib.func.callbackGotoBackground( klib.func.pContext );
	}
	
	// 現在のシーンだけに通知
	if( klib.scene.callbackGotoBackground )
	{
		klib.scene.callbackGotoBackground( klib.scene.pContext );
	}
	
}

//=============================================
/**
 *	@brief	アプリがバックグラウンドから復帰した時にライブラリが行う処理
 */
//=============================================
void KL_WillEnterForeground(){
	
	// 言語情報を再取得
	KLDevice_GetLanguage();
	
	// シーン関係なく通知
	if( klib.func.callbackGotoBackground )
	{
		klib.func.callbackGotoBackground( klib.func.pContextFore );
	}
	
	// 現在のシーンだけに通知
	if( klib.scene.callbackEnterForeground )
	{
		klib.scene.callbackEnterForeground( klib.scene.pContextFore );
	}
	
	KLTouch_Reset();
	
	KLView_DidEnterForeground();

	//if( KLInAppPurchase_GetInstance() )
	//{
	//	[[SKPaymentQueue defaultQueue] addTransactionObserver:KLInAppPurchase_GetInstance()];
	//}
	
}

//=============================================
/**
 *	@brief	例外ハンドラ
 */
//=============================================
void KL_uncaughtExceptionHandler( NSException *exception ){

	NSLog(@"CRASH: %@", exception);
	NSLog(@"Stack Trace: %@", [exception callStackSymbols]);
    
}
