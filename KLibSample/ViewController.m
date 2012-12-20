//
//  ViewController.m
//  KLibSample
//
//  Copyright (c) 2012年 Keita Nakazawa. All rights reserved.
//

#import "ViewController.h"
#import "KLLogo.h"

@interface ViewController () {
	
}
@end

@implementation ViewController


//===========================================================
/**
 *	@brief	解放処理
 */
//===========================================================
- (void)dealloc
{
	if( pView )
	{
		[pView release];
		pView = nil;
	}
	
	[super dealloc];
}


//===========================================================
/**
 *	@brief	初期化
 */
//===========================================================
- (id)init{
	
	if ((self = [super init]))
	{
		pView = [[KLibView alloc] initWithScene:Logo_Init controller:self];
		self.view = pView;
	}
	
	return self;
	
}


//===========================================================
/**
 *	@brief	ビューの読み込みが完了
 */
//===========================================================
- (void)viewDidLoad{
	
	[super viewDidLoad];

}


//===========================================================
/**
 *	@brief	ビューのアンロードが完了
 */
//===========================================================
- (void)viewDidUnload{

	[super viewDidUnload];

}


//===========================================================
/**
 *	@brief	メモリ警告を受け取った
 */
//===========================================================
- (void)didReceiveMemoryWarning{

	[super didReceiveMemoryWarning];

}

#ifdef KL_ONLY_LANDSCAPE

//===========================================================
/**
 *	@brief	自動回転処理を許可する方向を返す
 */
//===========================================================
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation{

	// LandscapeでGameCenterを使用する場合もプロジェクトの設定はPortraitも入れておかないと落ちる事がある
	return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown &&
			interfaceOrientation != UIInterfaceOrientationPortrait);

}

//===========================================================
/**
 *	@brief	サポートされている端末の方向を返す
 */
//===========================================================
- (NSUInteger)supportedInterfaceOrientations {
	// 対応方向に合わせたフラグを設定

	return UIInterfaceOrientationMaskLandscape;

}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation{

	return UIInterfaceOrientationMaskLandscape;

}

- (BOOL)shouldAutorotate{

	return [[UIDevice currentDevice] orientation] != UIInterfaceOrientationPortrait;

}
#else
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation{

	return YES;

}

- (NSUInteger)supportedInterfaceOrientations {

	// 対応方向に合わせたフラグを設定
	return UIInterfaceOrientationMaskAll;

}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation{

	return UIInterfaceOrientationMaskAll;

}

- (BOOL)shouldAutorotate{
	
	return YES;
	
}
#endif

@end
