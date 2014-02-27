//
//  UIViewPlus.h
//
//  Created by Keita Nakazawa on 2013/04/19.
//  Copyright (c) 2013年 Keita Nakazawa. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIViewPlus : UIView
{
	id owner;	///< スーパービュー的な親オブジェクトIDを把握したい時にどうぞ
}

- (id) initCommon;
- (UIViewController *) controller;
- (UINavigationController *) navController;
- (void) gotoSegue:(char*)pSegueName;
+ (CGPoint) absPoint : (UIView*) view;
+ (CGPoint) absPoint : (UIView*) view localx:(float)localx localy:(float)localy;

@property (readwrite, nonatomic) id owner;

@property (readwrite, nonatomic, setter = setX:,  getter = getX )					float x;
@property (readwrite, nonatomic, setter = setY:,  getter = getY )					float y;
@property (readwrite, nonatomic, setter = setW:,  getter = getW )					float width;
@property (readwrite, nonatomic, setter = setH:,  getter = getH )					float height;
@property (readwrite, nonatomic, setter = setXb:, getter = getXb )					float xb;
@property (readwrite, nonatomic, setter = setYb:, getter = getYb )					float yb;
@property (readwrite, nonatomic, setter = setWb:, getter = getWb )					float widthb;
@property (readwrite, nonatomic, setter = setHb:, getter = getHb )					float heightb;
@end
