//
//  KLibViewController.m
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

#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGLDrawable.h>

#import "KLibDelegate.h"
#import "KLibView.h"
#import "KLib.h"
#import "KLDrawQueue.h"
#import "KLFile.h"
#import "KLScreen.h"
#import "KLTouch.h"

/// View
static KLibView* pKLibView = nil;

/// Log buffer
static char KLibView_logBuffer[10240];

void KLibView_DidEnterBackground(){
	
	[pKLibView stopLoop];
	
}

void KLibView_DidEnterForeground(){
	
	[pKLibView startLoop];
	
}

//==================================================================
/**
 *	@brief	シェーダの作成
 *
 *	@param	*vstext	頂点シェーダの内容文字列
 *	@param	*fstext	フラグメントシェーダの内容文字列
 */
//==================================================================
GLuint CreateShader(const GLchar *vstext, const GLchar *fstext){
	
    GLuint	shader	= 0;
	GLuint	prog	= 0;
    int		status	= 0;
	int		logLen	= 0;
	
    prog = glCreateProgram();
	
	shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader, 1, &vstext, NULL);
	glCompileShader(shader);
	
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	
    if (status == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderInfoLog(shader, 10240, &logLength, KLibView_logBuffer);
		
        KLLog("Vertex shader failed: %s\n", KLibView_logBuffer);
		
        glDeleteShader(shader);
        glDeleteProgram(prog);
		
        return 0;
    }
	
	glAttachShader(prog, shader);
	glDeleteShader(shader);
	KLLog("Vertex shader compiled & attached\n");
	
	shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader, 1, &fstext, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	
    if (status == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderInfoLog(shader, 10240, &logLength, KLibView_logBuffer);
		
        KLLog("Fragment shader failed: %s\n", KLibView_logBuffer);
		
        glDeleteShader(shader);
        glDeleteProgram(prog);
		
        return 0;
    }
	
	glAttachShader(prog, shader);
	glDeleteShader(shader);
	KLLog("Fragment shader compiled & attached\n");
	glLinkProgram(prog);
	glValidateProgram(prog);
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLen);
    
	if(logLen > 0)
    {
        char *log = (char*)malloc(logLen + 1);
		
		glGetProgramInfoLog(prog, logLen, &logLen, log);
        KLLog("Shader link error:  %s\n", log);
		free(log);
		glDeleteProgram(prog);
		
        return 0;
    }
	
    return prog;
}

void (*alert1btnCallback)(u32) = NULL;

//========================================================
/**
 *	@brief	アラートの表示
 *
 *	@param	pTitle				ダイアログタイトル（NULLでタイトルなし）
 *	@param	pMsg				表示するメッセージ
 *	@param	pBtnLabel			ボタンラベル（NULLでOKと表示）
 *	@param	btnPressedCallback	ボタンを押した時のコールバックポインタ（NULLでコールバックなし）
 */
//========================================================
void KLAlert_Open( char* pTitle, char* pMsg, char* pBtnLabel, void(*btnPressedCallback)() ){
	
	alert1btnCallback	= btnPressedCallback;
	[pKLibView openAlert:pTitle msg:pMsg btnLabel:pBtnLabel];

}



// A class extension to declare private methods
@interface KLibView ()

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, assign) NSTimer *loopTimer;

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation KLibView

@synthesize context;
@synthesize loopTimer;
@synthesize interval;

-(void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	
	if( alert1btnCallback )
	{
		alert1btnCallback(buttonIndex);
	}
	
}

-(void) openAlert:(char*) pTitle msg:(char*) pMsg btnLabel:(char*) pBtnLabel {
	
	UIAlertView *alert = [ [UIAlertView alloc]
		initWithTitle:pTitle?[NSString  stringWithUTF8String:pTitle]:@""
		message:[NSString stringWithUTF8String:pMsg]
		delegate:self 
		cancelButtonTitle:pBtnLabel?[NSString stringWithUTF8String:pBtnLabel]:@"OK"
		otherButtonTitles:nil];

	[alert show];
	[alert release];
}

//==================================================================
/**
 *	@brief	レイヤーの取得
 */
//==================================================================
+ (Class)layerClass {

    return  [CAEAGLLayer class];

}

//==================================================================
/**
 *	@brief	初期化
 *
 *	@param	firstSceneFunc	最初のシーンとするシーン初期化関数
 */
//==================================================================
- (id)initWithScene:(KLObjFunc)firstSceneFunc controller:(UIViewController*)controller {

    if ((self = [super init]))
	{
		self->firstScene	= firstSceneFunc;
		self->ctrler		= controller;
    }
	
	pKLibView = self;
	self.multipleTouchEnabled = KLTOUCH_DEFAULT_MULTITOUCH_ENABLED;
	
    return self;
}
/*
- (id)initWithCoder:(NSCoder*)coder {
	
    if ((self = [super initWithCoder:coder]))
	{
	
    }
	
    return self;

}
*/

//==================================================================
/**
 *	@brief	OpenGLとKLibの初期化
 */
//==================================================================
- (void)initGL{

	CAEAGLLayer *eaglLayer = NULL;
	
	GLchar* vertexShader  = NULL;
	GLchar* fragmentShader= NULL;
	
	// 初期化済み
	if( initend )
	{
		return;
	}
	
	KLPath_Init();
	
	eaglLayer = (CAEAGLLayer *)self.layer;
	eaglLayer.backgroundColor = nil;
	eaglLayer.opaque = YES; // NOにすると重いのでYES必須
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:NO],
									kEAGLDrawablePropertyRetainedBacking,
									kEAGLColorFormatRGBA8,
									kEAGLDrawablePropertyColorFormat,
									nil];
	self.contentScaleFactor = [UIScreen mainScreen].scale;
	eaglLayer.contentsScale = [UIScreen mainScreen].scale;
	
	context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	
	if (!context || ![EAGLContext setCurrentContext:context])
	{
		KLLog("GLContext make error.\n");
		
		[self release];
		
		return;
		
	}
	
	
	// FPSの設定(NSTimer式の場合)
	interval = 1.0 / klib.fps;
	
	[self createFramebuffer];
	
	// シェーダが読めない時はプロジェクトのBuildPhasesのCopyBundleResourcesに含まれてるか確認
	vertexShader	= KLFile_GetBinary("KLSLdefault.vc", NULL, 1);
	fragmentShader	= KLFile_GetBinary("KLSLdefault.fc", NULL, 1);
	
	klib.shader.program.handler = CreateShader(vertexShader, fragmentShader);
	
	sfree(vertexShader);
	sfree(fragmentShader);
	
	if( !klib.shader.program.handler )
	{
		KLLog("Shader compile !!! NG !!!\n");
		klib.shader.pos.handler		= 0;
		klib.shader.color.handler	= 0;
		klib.shader.colormul.handler= 0;
		klib.shader.uv.handler		= 0;
	}
	else
	{
		KLLog("Shader compile ok\n");
		klib.shader.pos.handler		= glGetAttribLocation(klib.shader.program.handler, "klpos");
		klib.shader.color.handler	= glGetAttribLocation(klib.shader.program.handler, "klcol");
		klib.shader.colormul.handler= glGetAttribLocation(klib.shader.program.handler, "klcolRate");
		klib.shader.uv.handler		= glGetAttribLocation(klib.shader.program.handler, "kluv");
				
		KL_Init();
		KLIndicator_Init();
		[self addSubview:indicator];
		
		glUniform1i(glGetUniformLocation(klib.shader.program.handler, "tex"), 0);
	}
	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	
	// 端末回転イベント通知
	[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(didRotate:)
												 name:UIDeviceOrientationDidChangeNotification
											   object:nil];
	
	
	initend = true;
	
}

//==================================================================
/**
 *	@brief	メインループ
 */
//==================================================================
- (void)mainloop {
	
	//KLLog("\n--- LoopStart ---\n\n");

    [EAGLContext setCurrentContext:context];
		
    glsClear();
	
    glsBindFramebuffer(fbuf);
    
	glsBindTexturef( 0 );
	
	glsViewport(0, 0, backingWidth, backingHeight);

    glUseProgram(klib.shader.program.handler);
    {
		glsLinkProjectionMatrix();
		KLTouch_Update();
		KLDrawQueue_Clear();
		KLScene_Update(NULL);
		KLDrawQueue_Flush();
		
	}
    glUseProgram(0);
	
    glsBindRenderbuffer(cbuf);
    [context presentRenderbuffer:GL_RENDERBUFFER];
	
	//KLLog("\n--- LoopEnd ---\n\n");
	
}

- (void)setupScreenSize {

	KLDevice_InitScreenSize( KLDevice_IsPortrait() );
	KLScreen_InitWH(NULL, klib.device.w, klib.device.h);
	
}

//==================================================================
/**
 *	@brief	サブビューの設定
 */
//==================================================================
- (void)layoutSubviews {
	
	[EAGLContext setCurrentContext:context];
    
	[self initGL];
	
	[self setupScreenSize];
	
	[self rebuildBuffer];
	
	[self startLoop];
	
	if( firstScene )
	{
		KLScene_ChangeTo( self->firstScene, NULL );
		self->firstScene = nil;
	}
	
	KLLog("LayoutSubviews\n");
}

//==================================================================
/**
 *	@brief	バッファの再構築
 */
//==================================================================
- (void) rebuildBuffer{
	
	[self destroyFramebuffer];
	
	[self createFramebuffer];
	
	KLScreen_Init(NULL);
	
	KLScreen_SetCamera2D( NULL );
	
}

//==================================================================
/**
 *	@brief	フレームバッファ関連の作成
 */
//==================================================================
- (BOOL)createFramebuffer {

	glGenFramebuffers(1, &fbuf);
	glGenRenderbuffers(1, &cbuf);

	backingWidth	= KL_GetDeviceW();
	backingHeight	= KL_GetDeviceH();

	glsBindFramebufferf(fbuf);
	glsBindRenderbufferf(cbuf);
	[context renderbufferStorage :GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, cbuf);

	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

	
	// Depth buffer
	glGenRenderbuffers(1, &dbuf);
	glsBindRenderbufferf(dbuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbuf);

	// Check
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return NO;
	}
	
	KLScreen_SetWH( NULL, KLDevice_GetW(), KLDevice_GetH() );
	
	return YES;
}

//==================================================================
/**
 *	@brief	フレームバッファ関連の解放
 */
//==================================================================
- (void)destroyFramebuffer
{
	if( fbuf )
	{
		
		glDeleteFramebuffers(1, &fbuf);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
	}
	
	if( cbuf )
	{

		glDeleteRenderbuffers(1, &cbuf);
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);
    
	}

	if( dbuf )
	{
	
		glDeleteRenderbuffers(1, &dbuf);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		
	}

	fbuf	= 0;
	cbuf	= 0;
	dbuf	= 0;

}

//==================================================================
/**
 *	@brief	メインループの開始
 */
//==================================================================
- (void)startLoop {

#if KLIB_USE_NSTIMERLOOP
	// NSTimerでのループ回し
	// 公式ドキュメント曰く、スタッタリングやティアリング回避の保証がないとの事なので非推奨
	// リフレッシュレートの同期を気にしないのであれば、こちらは細かいFPS設定が可能
	if( !self.loopTimer )
	{
		self.loopTimer = [NSTimer	scheduledTimerWithTimeInterval:1.0 / klib.fps
										target:self
										selector:@selector(mainloop)
										userInfo:nil
										repeats:YES];
		KLLog("Loop started by NSTimer\n");
	}
#else
	if( !displayLink )
	{
		// リフレッシュレート同期型で、スタッタリングとティアリングが起こらないとの事
		// ただし、FPSは60を整数で割った数(60,30,20,15,12,10,6,5,4,3,2,1)に限られる
		int fps = klib.fps>60 ? 60 : klib.fps<=0 ? 1 : klib.fps;
		
		displayLink = [[self window].screen displayLinkWithTarget:self selector:@selector(mainloop)];
		displayLink.frameInterval = 60 / fps;
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
	}
	else
	{
		[displayLink setPaused:FALSE];
	}
	KLLog("Loop started by DisplayLink\n");
#endif
	
}

//==================================================================
/**
 *	@brief	メインループの停止
 */
//==================================================================
- (void)stopLoop {

#if KLIB_USE_NSTIMERLOOP
    self.loopTimer = nil;
	KLLog("Loop stoped.(NSTimer)\n");
#else
	[displayLink setPaused:TRUE];
	KLLog("Loop stoped.(DisplayLink)\n");
#endif
	
}

//==================================================================
/**
 *	@brief	メインループタイマーの設定
 */
//==================================================================
- (void)setloopTimer:(NSTimer *)newTimer {

#if KLIB_USE_NSTIMERLOOP
    [loopTimer invalidate];
    loopTimer = newTimer;
#else
	
#endif
	
}

//==================================================================
/**
 *	@brief	ループインターバルの設定
 *
 *	@param	loopInterval	ループインターバル
 */
//==================================================================
- (void)setAnimationInterval:(NSTimeInterval)loopInterval {

#if KLIB_USE_NSTIMERLOOP
    interval = loopInterval;
	
    if (loopTimer)
	{
        [self stopLoop];
        [self startLoop];
    }
#else
	[self stopLoop];
	[self startLoop];
#endif
}

//==================================================================
/**
 *	@brief	解放処理
 */
//==================================================================
- (void)dealloc {
	
	[self stopLoop];
	
	if (klib.shader.program.handler)
	{
		glDeleteProgram(klib.shader.program.handler);
		klib.shader.program.handler = 0;
	}

	if ([EAGLContext currentContext] == context)
	{
		[EAGLContext setCurrentContext:nil];
	}

	KL_Unload();
	
	[context release];
	[super dealloc];

}

//==================================================================
/**
 *	@brief	端末回転イベントコールバック<br/>
 *			端末回転時にKlibの処理に加え自前で何か処理をしたい時に登録する<br/>
 *			コールバックでFALSEを返すとKLibの回転処理は行わない
 *
 *	@param	funcDidRotateCallback	コールバック関数
 */
//==================================================================
- (void) setRotateCallback:(void*)funcDidRotateCallback{
	
	didRotateCallback = funcDidRotateCallback;
	
}

static UIDeviceOrientation oldo = UIDeviceOrientationUnknown;
//==================================================================
/**
 *	@brief	端末回転時の処理
 *
 *	@param	notification	通知情報
 */
//==================================================================
- (void) didRotate:(NSNotification *)notification {
	
    UIDeviceOrientation o			= [[notification object] orientation];
	//bl					toPortrait	= FALSE;
	bl					pass		= FALSE;
	
	if( didRotateCallback )
	{
		if( !didRotateCallback(notification) )
		{
			return;
		}
	}
	
    if (o == UIDeviceOrientationLandscapeLeft)
	{
		
	}
	else if (o == UIDeviceOrientationLandscapeRight)
	{
		
	}
	else if (o == UIDeviceOrientationPortraitUpsideDown)
	{
		//toPortrait = TRUE;
		//pass = TRUE;
	}
	else if (o == UIDeviceOrientationPortrait)
	{
		//toPortrait = TRUE;
	}
	else
	{
		pass = TRUE;
	}
	
	if( !pass && oldo!=o )
	{
		KLLog("Device rotation detected.\n");
		oldo = o;
		// 処理したい何かはここに書く
	}
	
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event{
	
	CGPoint tpos;
	float	s = KLDevice_GetScaleFactor();
	
	for (UITouch *touch in touches) {
	
		tpos = [touch locationInView:self];
		KLTouch_Start( [touch hash], tpos.x*s, tpos.y*s);
		
	}
	
	//KLLog("\n\n----------\nTouchStart\n----------\n\n");
	
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{

	CGPoint tpos;
	float	s = KLDevice_GetScaleFactor();
	
	for (UITouch *touch in touches) {
		
		tpos = [touch locationInView:self];
		KLTouch_Move( [touch hash], tpos.x*s, tpos.y*s);
		
	}
	
	//KLLog("\n\n----------\nTouchMoved\n----------\n\n");
	
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event{

	CGPoint tpos;
	float	s = KLDevice_GetScaleFactor();
	
	for (UITouch *touch in touches) {
		
		tpos = [touch locationInView:self];
		KLTouch_End( [touch hash], tpos.x*s, tpos.y*s);
		
	}
	
	//KLLog("\n\n----------\nTouchEnded\n----------\n\n");
	
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{

	// 端末が許容している最大マルチタッチ数を超えてタッチしても来る模様
	KLLog("\n\n----------\nTouchCancelled\ntoo many touches\n----------\n\n");
	KLTouch_Cancel();
	
}

@end

//======================================================================
/**
 *	@brief	インジケータを表示
 */
//======================================================================
void KLIndicator_Start(){
	
	if( pKLibView->indicator )
	{
		[pKLibView->indicator startAnimating];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータのアルファを設定
 */
//======================================================================
void KLIndicator_SetAlpha(u8 a){
	
	if( pKLibView->indicator )
	{
		[pKLibView->indicator setAlpha:a/255.0f];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータの座標を設定
 */
//======================================================================
void KLIndicator_SetXY( s32 x, s32 y ){
	
	if( pKLibView->indicator )
	{
		float r = 1.0f/KLDevice_GetScaleFactor();
		[pKLibView->indicator setFrame: CGRectMake(
										   x*r, y*r,
										   pKLibView->indicator.frame.size.width,
										   pKLibView->indicator.frame.size.height )];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータの座標を設定
 */
//======================================================================
void KLIndicator_SetWH( u16 w, u16 h ){
	
	if( pKLibView->indicator )
	{
		float r = 1.0f/KLDevice_GetScaleFactor();
		[pKLibView->indicator setFrame: CGRectMake(pKLibView->indicator.frame.origin.x,
										   pKLibView->indicator.frame.origin.y, w*r,h*r )];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータを停止
 *
 *	@param	withHide	停止後非表示にする場合はTRUE
 */
//======================================================================
void KLIndicator_Stop( bl withHide ){
	
	if( pKLibView->indicator )
	{
		[pKLibView->indicator stopAnimating];
		
		if( withHide )
		{
			pKLibView->indicator.hidesWhenStopped = TRUE;
		}
	}
	
}

void KLIndicator_Init(){
	
	pKLibView->indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:KLINDICATOR_FIRSTSTYLE];
	KLIndicator_SetXY(klib.device.w*0.5f-pKLibView->indicator.frame.size.width, klib.device.h*0.5f-pKLibView->indicator.frame.size.height);
	
}

void KLIndicator_Unload(){
	
	if( pKLibView->indicator )
	{
		[pKLibView->indicator stopAnimating];
		[pKLibView->indicator removeFromSuperview];
		[pKLibView->indicator release];
		pKLibView->indicator = nil;
	}
	
}

UIViewController* KLibView_GetController(){

	return pKLibView->ctrler;
	
}
