//
//  LogoViewController.m
//  klib
//
//  Copyright (c) 2013年 Keita Nakazawa. All rights reserved.
//

#import "LogoViewController.h"
#import "EmptyScene.h"
#import "Logo.h"

@interface LogoViewController ()

@end

@implementation LogoViewController



- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{

    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    
	if (self) {
	
		// Custom initialization
		
	}
    
	return self;

}

- (void)viewDidLoad
{

	[super viewDidLoad];
	// Do any additional setup after loading the view.

}

- (void) viewDidAppear:(BOOL)animated {
	
	KLLog("[GameView] viewDidAppear\n");
	
	// KLib 初期化時の空のシーンからロゴ用のシーンに切り替える
	KLScene_ChangeTo( Logo_Init, NULL );
	
	// もし、このサンプルのようなロゴ表示後、リストビューなどの
	// 全画面OpenGLのビュー以外に移る必要がない場合、
	// Logo_Update内で最初に出したいシーンの関数を指定した
	// KLScene_ChangeToをコールしてください。
	// 以降も各シーンのcからChangeToしてつなげます。
	
}

- (void) viewDidDisappear:(BOOL)animated {

	KLLog ("[GameView] viewDidDisappear ##\n\n");
	
	// KLView をアンロードする(ビューはアンロードするが、内部で KLibフレームワーク自体 はアンロードしない)
	if ( _klview != nil )
	{
		
		// 内部で空シーンに変更されます
		[_klview unload];
		
		_klview = nil;
		
	}
	
	// ここで KLib をアンロードしてしまうと
	// リストに戻るたびにアンロードが発生し使い回しができなくなります。
	// ゆえに、このサンプルでは、
	// KLib のアンロードを AppDelegate のアプリ終了イベント内で行っています。
	
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
