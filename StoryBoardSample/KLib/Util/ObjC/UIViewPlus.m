//
//  UIViewPlus.m
//
//  Created by Keita Nakazawa.
//  Copyright (c) 2013年 Keita Nakazawa. All rights reserved.
//

#import "UIViewPlus.h"

@interface UIResponder (NextResponderWithClass)
- (UIResponder*) nextResponderWithClass:(Class)aClass;
@end

@implementation UIResponder (NextResponderWithClass)
- (UIResponder*) nextResponderWithClass:(Class)aClass {
    UIResponder* responder = [self nextResponder];
    if ( responder ) {
        if ( [responder isKindOfClass:aClass] ) {
            return responder;
        }
        else {
            return [responder nextResponderWithClass:aClass];
        }
    }
    return nil;
}
@end


@implementation UIViewPlus

@synthesize owner, x,y,width,height, xb, yb, widthb, heightb;

+ (CGPoint) absPoint : (UIView*) view {
	
	CGPoint ret = CGPointMake(view.frame.origin.x, view.frame.origin.y);
	
	if ([view superview] != nil) {
		
		CGPoint addPoint = [UIViewPlus absPoint: [view superview]];
		ret = CGPointMake(ret.x + addPoint.x, ret.y + addPoint.y);
		
	}
	
	
	return ret;
	
}

+ (CGPoint) absPoint : (UIView*) view localx:(float)localx localy:(float)localy {
	
	CGPoint ret = CGPointMake( localx, localy);
	
	if ([view superview] != nil) {
		
		CGPoint addPoint = [UIViewPlus absPoint: [view superview]];
		ret = CGPointMake(ret.x + addPoint.x, ret.y + addPoint.y);
		
	}
	
	
	return ret;
	
}

//===============================================
/**
 *	@brief	init系から共通して呼ばれる初期化用メソッド
 */
//===============================================
- (id) initCommon {
	
	owner	= nil;
	
	return self;
	
}

- (id) init {
	
	if ( self = [super init] ) {
	
		return [self initCommon];
	
	}
	
	return self;
	
}

- (id) initWithCoder:(NSCoder *)aDecoder {
	
	if ( self = [super initWithCoder:aDecoder] ) {
		
		return [self initCommon];
		
	}
	
	return self;
	
}

- (id) initWithFrame:(CGRect)frame {
	
	if ( self = [super initWithFrame:frame] ) {
		
		return [self initCommon];
		
	}
	
	return self;
	
}

//===============================================
/**
 *	@brief	フレームのX座標を取得
 *
 *	@return	frame内のX座標
 */
//===============================================
- (float) getX {
	
	return self.frame.origin.x;
	
}

//===============================================
/**
 *	@brief	フレームのX座標を設定
 *
 *	@param	posx	設定するX座標
 */
//===============================================
- (void) setX : (float)posx {
	
	self.frame = CGRectMake (posx, self.frame.origin.y, self.frame.size.width, self.frame.size.height);
	
}

//===============================================
/**
 *	@brief	フレームのY座標を取得
 *
 *	@return	frame内のY座標
 */
//===============================================
- (float) getY {
	
	return self.frame.origin.y;
	
}

//===============================================
/**
 *	@brief	フレームのY座標を設定
 *
 *	@param	posy	設定するY座標
 */
//===============================================
- (void) setY : (float)posy {
	
	self.frame = CGRectMake (self.frame.origin.x, posy, self.frame.size.width, self.frame.size.height);
	
}

//===============================================
/**
 *	@brief	フレームの幅を取得
 *
 *	@return	frameのwidth
 */
//===============================================
- (float) getW {
	
	return self.frame.size.width;
	
}

//===============================================
/**
 *	@brief	フレームの幅を設定
 *
 *	@param	framew	設定する幅
 */
//===============================================
- (void) setW : (float)framew {
	
	self.frame = CGRectMake (self.frame.origin.x, self.frame.origin.y, framew, self.frame.size.height);
	
}

//===============================================
/**
 *	@brief	フレームの高さを取得
 *
 *	@return	frameのwidth
 */
//===============================================
- (float) getH {
	
	return self.frame.size.height;
	
}

//===============================================
/**
 *	@brief	フレームの高さを設定
 *
 *	@param	frameh	設定する高さ
 */
//===============================================
- (void) setH : (float)frameh {
	
	self.frame = CGRectMake (self.frame.origin.x, self.frame.origin.y, self.frame.size.width, frameh);
	
}

//===============================================
/**
 *	@brief	バウンドのX座標を取得
 *
 *	@return	bounds内のX座標
 */
//===============================================
- (float) getXb {
	
	return self.bounds.origin.x;
	
}

//===============================================
/**
 *	@brief	バウンドのX座標を設定
 *
 *	@param	posx	設定するX座標
 */
//===============================================
- (void) setXb : (float)posx {
	
	self.bounds = CGRectMake (posx, self.bounds.origin.y, self.bounds.size.width, self.bounds.size.height);
	
}

//===============================================
/**
 *	@brief	バウンドのY座標を取得
 *
 *	@return	bounds内のY座標
 */
//===============================================
- (float) getYb {
	
	return self.bounds.origin.y;
	
}

//===============================================
/**
 *	@brief	バウンドのY座標を設定
 *
 *	@param	posy	設定するY座標
 */
//===============================================
- (void) setYb : (float)posy {
	
	self.bounds = CGRectMake (self.bounds.origin.x, posy, self.bounds.size.width, self.bounds.size.height);
	
}

//===============================================
/**
 *	@brief	バウンドの幅を取得
 *
 *	@return	boundsのwidth
 */
//===============================================
- (float) getWb {
	
	return self.bounds.size.width;
	
}

//===============================================
/**
 *	@brief	バウンドの幅を設定
 *
 *	@param	boundsw	設定する幅
 */
//===============================================
- (void) setWb : (float)boundsw {
	
	self.bounds = CGRectMake (self.bounds.origin.x, self.bounds.origin.y, boundsw, self.bounds.size.height);
	
}

//===============================================
/**
 *	@brief	バウンドの高さを取得
 *
 *	@return	boundsのwidth
 */
//===============================================
- (float) getHb {
	
	return self.bounds.size.height;
	
}

//===============================================
/**
 *	@brief	バウンドの高さを設定
 *
 *	@param	boundsh	設定する高さ
 */
//===============================================
- (void) setHb : (float)boundsh {
	
	self.bounds = CGRectMake (self.bounds.origin.x, self.bounds.origin.y, self.bounds.size.width, boundsh);
	
}

//===============================================
/**
 *	@brief	ビューのコントローラを取得
 */
//===============================================
- (UIViewController *) controller {

	UIResponder* responder = [self nextResponderWithClass:[UIViewController class]];
	
	if ( responder )
	{
		
		UIViewController* vc = (UIViewController*)responder;
		return vc;
	
	}
	
	return nil;

}

//===============================================
/**
 *	@brief	ビューのナビゲーションコントローラを取得
 */
//===============================================
- (UINavigationController *) navController {
	
	UIView *v = (UIView *)self.nextResponder;
	
	if ([v.nextResponder isKindOfClass:[UINavigationController class]])
	{
		return (UINavigationController *)v.nextResponder;
	}
	
	return nil;
	
}


//======================================================================
/**
 *	@brief	指定の Segue にビューを移動する<br />
 *			ビューのコントローラに対してコールするのでコントローラなしのビューには使用できない
 */
//======================================================================
- (void) gotoSegue:(char*)pSegueName {
	
	UIViewController* ctrler = self.controller;
	NSString* nstr = [NSString stringWithUTF8String:pSegueName];
	
	[ctrler performSegueWithIdentifier:nstr sender:ctrler];
	
}

@end
