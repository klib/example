//
//  GameView.m
//  StoryboardTest
//
//  Created by Keita Nakazawa on 2013/04/19.
//  Copyright (c) 2013年 Keita Nakazawa. All rights reserved.
//

// リストから選択したサンプルに移行後のビューコントローラです。

#import "GameView.h"
#import "KLDrawQueue.h"
#import "Game.h"
#import "EmptyScene.h"

@implementation GameView

static int			sid		= 0;

+ (void) SetSceneId: (int) sceneid {
	
	sid = sceneid;
	
}

- (void)loadView {
	
	[super loadView];
		
	// サンプル内の KLib オブジェクトの確保について
	//===========================================================
	//
	// 各種サンプルに入る際に KLib を初期化し、リストに戻ると解放という流れを
	// 繰り返してもいいのですが、都度初期化は時間がかかりメモリ効率も悪いので、
	// アプリ起動時に一回のみ初期化し、全シーンで KLib を共有します。
	// KLib未使用のリストビューの時は、空のゲームシーンを設定し、
	// サンプルに入るとサンプルに合わせたシーンへ切り替えます。

	KLLog("[GameView] loadView & klibview\n");

}

- (void) viewDidLoad {
	
	[super viewDidLoad];
	
	KLLog("[GameView] viewDidload.\n");

}

- (void) viewDidAppear:(BOOL)animated {
	
	KLLog("[GameView] viewDidAppear\n");
	
	// 空のシーン( Game )から指定したシーンに切り替える
	KLScene_ChangeTo( gSceneInfo[sid].func, NULL );
	
		
}

- (void) viewWillUnload {

	KLLog("[GameView] ViewWillUnload ##\n\n");
	
}

- (void) viewDidDisappear:(BOOL)animated {

	KLLog ("[GameView] viewDidDisappear ##\n\n");
	
	// KLView をアンロードする(ビューはアンロードするが、内部で KLibフレームワーク自体 はアンロードしない)
	if ( _klview != nil )
	{
	
		[_klview unload];
		
		_klview = nil;
		
	}
	
	// ここで KLib をアンロードしてしまうと
	// リストに戻るたびにアンロードが発生し使い回しができなくなります。
	// ゆえに、このサンプルでは、
	// KLib のアンロードを AppDelegate のアプリ終了イベント内で行っています。
	
}

@end
