//
//  GameView.h
//  StoryboardTest
//
//  Created by Keita Nakazawa on 2013/04/19.
//  Copyright (c) 2013年 Keita Nakazawa. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "UIViewPlus.h"
#import "KLView.h"

@interface GameView : UIViewController
{
	
	
	
}

+ (void) SetSceneId: (int) sceneid;

@property (weak, nonatomic) IBOutlet KLView *klview;

@end
