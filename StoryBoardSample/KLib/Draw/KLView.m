//
//  KLView.m
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
#import "KLView.h"
#import "KLib.h"
#import "KLDrawQueue.h"
#import "KLFile.h"
#import "KLDevice.h"
#import "KLGL.h"
#import "EmptyScene.h"

/// View
KLView* pKLView = nil;

/// Log buffer
static char KLView_logBuffer[10240];

void KL_StartLoop(){
	
	[pKLView startLoop];
	
}

void KL_StopLoop(){
	
	[pKLView stopLoop];
	
}

void KLView_DidEnterBackground(){
	
	[pKLView stopLoop];
	
}

void KLView_DidEnterForeground(){
	
	[pKLView startLoop];
	
}

void KLView_Unload () {
	
	[pKLView unload];
	
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
	
    GLuint	prog	= 0;
    int		status	= 0;
	int		logLen	= 0;
	
    prog = glCreateProgram();
	
	klib.shader.vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(klib.shader.vshader, 1, &vstext, NULL);
	glCompileShader(klib.shader.vshader);
	
    glGetShaderiv(klib.shader.vshader, GL_COMPILE_STATUS, &status);
	
    if (status == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderInfoLog(klib.shader.vshader, 10240, &logLength, KLView_logBuffer);
		
        KLLog("Vertex shader failed: %s\n", KLView_logBuffer);
		
        glDeleteShader(klib.shader.vshader);
        glDeleteProgram(prog);
		
        return 0;
    }
	
	glAttachShader(prog, klib.shader.vshader);
	KLLog("Vertex shader compiled & attached\n");
	
	klib.shader.fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(klib.shader.fshader, 1, &fstext, NULL);
	glCompileShader(klib.shader.fshader);
	glGetShaderiv(klib.shader.fshader, GL_COMPILE_STATUS, &status);
	
    if (status == GL_FALSE)
    {
        int logLength = 0;
        glGetShaderInfoLog(klib.shader.fshader, 10240, &logLength, KLView_logBuffer);
		
        KLLog("Fragment shader failed: %s\n", KLView_logBuffer);
		
        glDeleteShader(klib.shader.fshader);
        glDeleteProgram(prog);
		
        return 0;
    }
	
	glAttachShader(prog, klib.shader.fshader);
	
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

void (*alert1btnCallback)(u64) = NULL;

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
	[pKLView openAlert:pTitle msg:pMsg btnLabel:pBtnLabel];

}

char* KLString_AsciiToUTF8 ( char* pStr, size_t sz ) {

	NSString* s = [[NSString alloc] initWithBytes:pStr length:sz encoding:NSASCIIStringEncoding];
	return (char*) [s UTF8String];
	
}


// A class extension to declare private methods
@interface KLView ()

- (BOOL) createFramebuffer;
- (void) destroyFramebuffer;

@end

@implementation KLView

+ (EAGLContext*) getContext {

	return pKLView? pKLView->context : nil;
	
}

//==================================================================
/**
 *	@brief	テキスト編集時のコールバック
 *
 *	@param	textField	イベントが発生したテキストフィールド
 *	@param	range		文字列が置き換わる範囲(入力された範囲)
 *	@param	string		置き換わる文字列(入力された文字列)
 *
 *	@return	YES 入力を許可する場合 / NO 許可しない場合
 */
//==================================================================
- (BOOL)textField:(UITextField *)textField
shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)string {
	
	// すでに入力されているテキストを取得
	NSMutableString *text = [textField.text mutableCopy];

	// すでに入力されているテキストに今回編集されたテキストをマージ
	[text replaceCharactersInRange:range withString:string];

	if ( kbdEditCallback ) {
		
		kbdEditCallback ( (char*) [text UTF8String], pKbdContext );
		
	}
	
	// 結果が文字数をオーバーしていないならYES，オーバーしている場合はNO
	return kbd_maxlen < 0 ? YES : ([text length] <= kbd_maxlen);

}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    
	if ( kbdReturnCallback ) {
		
		kbdReturnCallback ( (char*) [textField.text UTF8String], pKbdContext );
		
	}
	
	[self closeKeyboard];
	
    return YES;
}

//==================================================================
/**
 *	@brief	レイヤーの取得
 */
//==================================================================
+ (Class)layerClass {

    return  [CAEAGLLayer class];

}

- (void) initNotify {

	self->firstScene	= NULL;
	
	pKLView = self;
	self.multipleTouchEnabled = KLTOUCH_DEFAULT_MULTITOUCH_ENABLED;
	
	[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationWillResignActive)
                                                 name:UIApplicationWillResignActiveNotification
                                               object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(applicationDidBecomeActive)
                                                 name:UIApplicationDidBecomeActiveNotification
                                               object:nil];

	
}

- (id) init {

	if ( self = [super init] )
	{
		[self initNotify];
	}
	else
	{
		KLLog("Could not init KLView.\n");
	}
	
    return self;
	
}

- (id) initWithFrame:(CGRect)frame {
	
	if ( self = [super initWithFrame:frame] )
	{
		backingWidth = frame.size.width;
		backingHeight= frame.size.height;
		[self initNotify];
	}
	else
	{
		KLLog ("Could not initWithFrame KLView.\n");
	}
	
	return self;
}


- (id)initWithCoder:(NSCoder*)coder {
	
    if ((self = [super initWithCoder:coder]))
	{
		[self initNotify];
    }
	else
	{
		KLLog("Could not initWithCoder KLView.\n");
	}
	
    return self;
	
}

- (void) applicationWillResignActive {

	KLLog ("\n\n[KLView] applicationWillResignActive.\n\n");
	[self stopLoop];
	glFinish();

}

- (void) applicationDidBecomeActive {

	KLLog ("\n\n[KLView] applicationDidBecomeActive.\n\n");
	[self startLoop];
	
}

//==================================================================
/**
 *	@brief	OpenGLとKLibの初期化
 */
//==================================================================
- (void)initGL{

	CAEAGLLayer *eaglLayer = NULL;
	
	GLchar* vertexShader  = NULL;
	GLchar* fragmentShader= NULL;
	
	KLPath_Init();
	
	eaglLayer = (CAEAGLLayer *) self.layer;
	eaglLayer.backgroundColor = nil;
	eaglLayer.opaque = YES; // NOにすると重いのでYES必須
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
									[NSNumber numberWithBool:NO],
									kEAGLDrawablePropertyRetainedBacking,
									//kEAGLColorFormatRGBA8,
									kEAGLColorFormatRGB565,
									kEAGLDrawablePropertyColorFormat,
									nil];
	self.contentScaleFactor = [UIScreen mainScreen].scale;
	eaglLayer.contentsScale = [UIScreen mainScreen].scale;
	
	if ( !context )
	{
		
		context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
	
		
		// 端末回転イベント通知
		[[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(didRotate:)
													 name:UIDeviceOrientationDidChangeNotification
												   object:nil];
		
		KLIndicator_Init();
		[self addSubview:indicator];
		
		KLKeyboard_Init();
		[self addSubview:kbdfield];
		
	}
	
	if (!context || ![EAGLContext setCurrentContext:context])
	{
		KLLog("GLContext make error.\n");
#if __has_feature(objc_arc)
#else
		[self release];
#endif
		return;
		
	}
	
	[EAGLContext setCurrentContext:context];
	
	[self createFramebuffer];

	// 初期化済みなら飛ばす
	if( !klib.shader.program.handler )
	{
		
		// シェーダが読めない時はプロジェクトのBuildPhasesのCopyBundleResourcesに含まれてるか確認
		vertexShader	= KLFile_GetBinary("KLSLdefault.vc", NULL, 1);
		fragmentShader	= KLFile_GetBinary("KLSLdefault.fc", NULL, 1);
		
		klib.shader.program.handler = CreateShader(vertexShader, fragmentShader);
		
		sfree(vertexShader);
		sfree(fragmentShader);
		
	}
	
	// エラーチェック
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
		
		KL_Init( backingWidth, backingHeight );
		
		glUniform1i(glGetUniformLocation(klib.shader.program.handler, "tex"), 0);
	
	}
	
	
	// FPSの設定(NSTimer式の場合)
	interval = 1.0f / klib.fps;
	KLLog("[ KLView ] FPS:%.1f\n", klib.fps);
	
	// 初期背景色はここで設定
	glClearColor(KL_BGCOLOR_R/255.f, KL_BGCOLOR_G/255.f, KL_BGCOLOR_B/255.f, KL_BGCOLOR_A/255.f);
	
	
	eaglLayer = nil;

}

//==================================================================
/**
 *	@brief	メインループ
 */
//==================================================================
- (void)mainloop {
	
	//KLLog("\n--- LoopStart ---\n\n");

	if ( [EAGLContext currentContext] == self->context )
	{
	
		[EAGLContext setCurrentContext:context];
	
	}
	
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

	KLDevice_InitScreenSize( klib.view.w, klib.view.h, KLDevice_IsPortrait() );
	KLScreen_InitWH(NULL, klib.view.w, klib.view.h);
	KLScreen_SetWH( NULL, klib.view.w, klib.view.h);
	
}

//==================================================================
/**
 *	@brief	サブビューの設定
 */
//==================================================================
- (void)layoutSubviews {
	
	[super layoutSubviews];
	
	if ( context )
	{
		
		[self initGL];

		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
		
		KLLog("[KLView] BackingW:%d H:%d\n", backingWidth, backingHeight);
		
		KLDevice_InitScreenSize ( backingWidth, backingHeight, KLDevice_IsPortrait() );
		KLScreen_SetCamera2D ( &klib.view );
	}
	else
	{
	
		[self initGL];
		
		[self setupScreenSize];
		
		[self rebuildBuffer];
	
	}
	
	[self startLoop];
	
	KLLog("[KLView] LayoutSubviews\n");
	
}

//==================================================================
/**
 *	@brief	バッファの再構築
 */
//==================================================================
- (void) rebuildBuffer {
	
	[self destroyFramebuffer];
	
	[self createFramebuffer];
	
	KLScreen_Init(NULL);
	
	KLScreen_SetCamera2D( &klib.view );
	
	KLLog("[KLView] rebuildBuffer\n");
	
}

//==================================================================
/**
 *	@brief	フレームバッファ関連の作成
 */
//==================================================================
- (BOOL)createFramebuffer {

	
	backingWidth = self.frame.size.width;
	backingHeight= self.frame.size.height;
	
	KLLog("[KLView] BackingW:%d H:%d\n", backingWidth, backingHeight);
	
	[self destroyFramebuffer];
	
	glGenFramebuffers(1, &fbuf);
	glGenRenderbuffers(1, &cbuf);
	
	glsBindFramebufferf(fbuf);
	glsBindRenderbufferf(cbuf);
	[context renderbufferStorage :GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, cbuf);

	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);
	
	// Depth buffer
	//glGenRenderbuffers(1, &dbuf);
	//glsBindDepthbufferf(dbuf);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbuf);
	glGenRenderbuffers(1, &dbuf);
	glsBindDepthbufferf(dbuf);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, backingWidth, backingHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dbuf);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, dbuf);
	
	// Check( シミュレータだと内部でリークしてるっぽい実機では発生せず )
	GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( stat != GL_FRAMEBUFFER_COMPLETE)
	{
		NSLog(@"failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		return NO;
	}

	glsBindRenderbufferf(cbuf);
	
	return YES;
}

//==================================================================
/**
 *	@brief	フレームバッファ関連の解放
 */
//==================================================================
- (void)destroyFramebuffer
{
	GLenum discards[] = {GL_DEPTH_ATTACHMENT,0};
	
	if( fbuf )
	{
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &fbuf);
		
	}
	
	if( cbuf )
	{
		
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glDeleteRenderbuffers(1, &cbuf);
    	discards[0] = GL_RENDERBUFFER;
		glDiscardFramebufferEXT( GL_RENDERBUFFER, 1, discards );
	
	}

	if( dbuf )
	{
		
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glDeleteRenderbuffers(1, &dbuf);
		discards[0] = GL_DEPTH_ATTACHMENT;
		glDiscardFramebufferEXT( GL_FRAMEBUFFER, 1, discards );
		
	}
	
	fbuf	= 0;
	cbuf	= 0;
	dbuf	= 0;
	sbuf	= 0;
	
}

//==================================================================
/**
 *	@brief	メインループの開始
 */
//==================================================================
- (void)startLoop {

#if KLIB_USE_NSTIMERLOOP
	// NSTimerでのループ回し
	// スタッタリングやティアリング回避の保証がないとの事なので非推奨
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
		// ただし、FPSは60を自然数で割った数(60,30,20,15,12,10,6,5,4,3,2,1)に限られる
		int fps = klib.fps>60 ? 60 : klib.fps<=0 ? 1 : klib.fps;
		
		displayLink = [[self window].screen displayLinkWithTarget:self selector:@selector(mainloop)];
		displayLink.frameInterval = 60.f / fps;
		[displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		KLLog("Loop started by DisplayLink\n");
	}
	else
	{
		
		if ( [displayLink isPaused] )
		{
			
			[displayLink setPaused:FALSE];
			KLLog("Loop started by DisplayLink\n");
		
		}
		
	}
	
#endif
	
}

//==================================================================
/**
 *	@brief	メインループの停止
 */
//==================================================================
- (void)stopLoop {

#if KLIB_USE_NSTIMERLOOP
	if ( self.loopTimer )
    {
		self.loopTimer = nil;
		KLLog("Loop stoped.(NSTimer)\n");
	}
#else
	if (![displayLink isPaused])
	{
		[displayLink setPaused:TRUE];
		KLLog("Loop stoped.(DisplayLink)\n");
	}
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

- (void)unload {

	[self stopLoop];
	
	displayLink = nil;
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
													name:UIDeviceOrientationDidChangeNotification
												  object:nil];
	
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
                                                 name:UIApplicationWillResignActiveNotification
                                               object:nil];
	
	[[NSNotificationCenter defaultCenter] removeObserver:self
                                                 name:UIApplicationDidBecomeActiveNotification
                                               object:nil];
	
	glUseProgram(klib.shader.program.handler);
    
	if (klib.shader.vshader)
	{
		
		glDetachShader(klib.shader.program.handler, klib.shader.vshader);
		glDeleteShader(klib.shader.vshader);
		klib.shader.vshader = 0;
		
	}
	
	if (klib.shader.fshader)
	{
	
		glDetachShader(klib.shader.program.handler, klib.shader.fshader);
		glDeleteShader(klib.shader.fshader);
		klib.shader.fshader = 0;
	
	}
	
	if (klib.shader.program.handler)
	{
	
		glDeleteProgram(klib.shader.program.handler);
		klib.shader.program.handler = 0;
	
	}
	
	glUseProgram(0);
	
	if ( [EAGLContext currentContext] == self->context )
	{
    
		[EAGLContext setCurrentContext:nil];
    
	}
	
	self->context = nil;
	
	KLIndicator_Unload();
	KLKeyboard_Unload();
	
	// ビューを移行するたびにKLibを解放する場合はここを使用する(非推奨)
	//KL_Unload();
	
	KL_PrepareReuse();
	
	KLLog ("\n[KLView] unloaded.\n");
	
}

//==================================================================
/**
 *	@brief	解放処理
 */
//==================================================================
- (void)dealloc {

#if __has_feature(objc_arc)
#else
	[context release];
	[super dealloc];
#endif
	
	KLLog("![KLView] dealloc.\n");
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
	/*
	else if (o == UIDeviceOrientationFaceUp)
	{
	
	}
	else if (o == UIDeviceOrientationFaceDown)
	{
		
	}
	*/
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
	
	for (UITouch *touch in touches)
	{
	
		tpos = [touch locationInView:self];
		KLTouch_Start( [touch hash], tpos.x*s, tpos.y*s);
		
	}
	
	if ( pKLView && !klib.kbd.isnotCloseOutTap && pKLView->kbdfield && touches.count==1 )
	{
	
		[pKLView closeKeyboard];
		
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

//==================================================================
/**
 *	@brief	指定 Segue に移動する
 *
 *	@param	pSegueName	Segue名
 */
//==================================================================
void KL_GotoSegue ( char* pSegueName ){
	
	[pKLView gotoSegue:pSegueName];
	
}

//==================================================================
/**
 *	@brief	簡易アラートコールバック
 */
//==================================================================
- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
	
	if( alert1btnCallback )
	{
		alert1btnCallback(buttonIndex);
	}
	
}

//==================================================================
/**
 *	@brief	簡易アラート
 */
//==================================================================
-(void) openAlert:(char*) pTitle msg:(char*) pMsg btnLabel:(char*) pBtnLabel {
	
	NSString* pm = [NSString stringWithUTF8String:pMsg];
	
	UIAlertView *alert = [ [UIAlertView alloc]
						  initWithTitle:pTitle?[NSString  stringWithUTF8String:pTitle]:@""
						  message:pm//[NSString stringWithUTF8String:pMsg]
						  delegate:self
						  cancelButtonTitle:pBtnLabel?[NSString stringWithUTF8String:pBtnLabel]:@"OK"
						  otherButtonTitles:nil];
	
	[alert show];
#if __has_feature(objc_arc)
#else
	[alert release];
#endif
}

- (void) setKeyboardBorderType : (UITextBorderStyle) style{

	kbdfield.borderStyle = style;
	
}

- (void) openKeyboard {

	[kbdfield becomeFirstResponder];
	
}

- (void) closeKeyboard {
	
	[kbdfield resignFirstResponder];
	
}

- (void)keyboardWillHide:(NSNotification *)notification
{
	
    //NSLog(@"keyboardWillHide");
	if (kbdfield)
	{
	
		CGRect r = CGRectMake( kbdfield.frame.origin.x, klib.device.h, kbdfield.frame.size.width, kbdfield.frame.size.height );
		kbdfield.hidden = YES;
		kbdfield.frame = r;
		
	}
	
}

- (void)keyboardDidHide:(NSNotification *)notification
{
	
	//NSLog(@"keyboardDidHide");
	
}

- (void)keyboardUpdateShow:(NSNotification *)notification
{

	//NSLog(@"keyboardWillShow");
	
}

- (void)keyboardDidShow:(NSNotification *)notification
{
	
	CGRect r, kr = [[notification.userInfo
								objectForKey:UIKeyboardFrameEndUserInfoKey] CGRectValue];
    
	kr = [self convertRect:kr fromView:self.window];
	r  = CGRectMake ( kbdfield.frame.origin.x, kr.origin.y - kbdfield.frame.size.height, kr.size.width, kbdfield.frame.size.height );
	
	kbdfield.frame = r;
	
	kbdfield.hidden = NO;
	
	//NSLog(@"keyboardDidShow");
	
}

- (void)keyboardBorder:(int) width color:(UIColor*) color cornerRadius:(int)cornerRadius {
	
	if ( pKLView && pKLView->kbdfield )
	{
	
		pKLView->kbdfield.layer.borderWidth	= width;
		pKLView->kbdfield.layer.borderColor	= [color CGColor];
		pKLView->kbdfield.layer.cornerRadius= cornerRadius;

	}
	
}

@end

//======================================================================
/**
 *	@brief	インジケータを表示
 */
//======================================================================
void KLIndicator_Start(){
	
	if( pKLView->indicator )
	{
		[pKLView->indicator startAnimating];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータのアルファを設定
 */
//======================================================================
void KLIndicator_SetAlpha(u8 a){
	
	if( pKLView->indicator )
	{
		[pKLView->indicator setAlpha:a/255.0f];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータの座標を設定
 */
//======================================================================
void KLIndicator_SetXY( s32 x, s32 y ){
	
	if( pKLView->indicator )
	{
		float r = 1.0f/KLDevice_GetScaleFactor();
		[pKLView->indicator setFrame: CGRectMake(
										   x*r, y*r,
										   pKLView->indicator.frame.size.width,
										   pKLView->indicator.frame.size.height )];
	}
	
}

//======================================================================
/**
 *	@brief	インジケータの座標を設定
 */
//======================================================================
void KLIndicator_SetWH( u16 w, u16 h ){
	
	if( pKLView->indicator )
	{
		float r = 1.0f/KLDevice_GetScaleFactor();
		[pKLView->indicator setFrame: CGRectMake(pKLView->indicator.frame.origin.x,
										   pKLView->indicator.frame.origin.y, w*r,h*r )];
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
	
	if( pKLView->indicator )
	{
		[pKLView->indicator stopAnimating];
		
		if( withHide )
		{
			pKLView->indicator.hidesWhenStopped = TRUE;
		}
	}
	
}

void KLIndicator_Init(){
	
	float r = 1.0f;
	
	if( KLDevice_GetScaleFactor()==1.0f )
	{
		r = 0.5f;
	}
	
	pKLView->indicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:KLINDICATOR_FIRSTSTYLE];
	
	KLIndicator_SetXY(klib.device.w*0.5f-pKLView->indicator.frame.size.width*r, klib.device.h*0.5f-pKLView->indicator.frame.size.height*r);
	
}

void KLIndicator_Unload(){
	
	if( pKLView->indicator )
	{
		[pKLView->indicator stopAnimating];
		[pKLView->indicator removeFromSuperview];
#if __has_feature(objc_arc)
#else
		[pKLView->indicator release];
#endif
		pKLView->indicator = nil;
	}
	
}

void KLKeyboard_MaxStringSize( int maxlen ) {
	
	if ( pKLView )
	{
		
		pKLView->kbd_maxlen = maxlen;
		
	}
	
}

void KLKeyboard_SetColor( Rgba bgcolor, Rgba textcolor ) {
	
	if ( pKLView )
	{
		
		pKLView->kbd_bgcolor	= bgcolor;
		pKLView->kbd_textcolor	= textcolor;
		
		if ( pKLView->kbdfield )
		{
		
			pKLView->kbdfield.backgroundColor	= [UIColor colorWithRed:bgcolor.r/255.0f green:bgcolor.g/255.0f blue:bgcolor.b/255.0f alpha:bgcolor.a/255.0f];
			pKLView->kbdfield.textColor			= [UIColor colorWithRed:textcolor.r/255.0f green:textcolor.g/255.0f blue:textcolor.b/255.0f alpha:textcolor.a/255.0f];

		}
		
	}
	
}

void KLKeyboard_SetPlaceholder ( char* pPlaceholderStr ) {
	
	if ( !pPlaceholderStr )
	{
		pPlaceholderStr = "";
	}
	
	if ( pKLView && pKLView->kbdfield )
	{
		pKLView->kbdfield.placeholder = [NSString stringWithUTF8String:pPlaceholderStr];
	}
	
}

void KLKeyboard_Init() {
	
	pKLView->kbdfield = [[UITextField alloc] initWithFrame:CGRectMake(0,klib.view.h,klib.view.w,30)];
	pKLView->kbdfield.delegate = pKLView;
	
	KLKeyboard_MaxStringSize (-1);
	KLKeyboard_SetColor ( RGBA(0xFF,0xFF,0xFF,0xFF), RGBA(0,0,0,0xFF) );
	KLKeyboard_SetBorderStyle ( KLTextBorderStyleBezel );
	KLKeyboard_SetType ( KLKeyboardTypeDefault );
	KLKeyboard_SetReturnKeyType ( KLReturnKeyDone );
	KLKeyboard_SetPlaceholder ("");
	
	[[NSNotificationCenter defaultCenter]
	 addObserver:pKLView
	 selector:@selector(keyboardUpdateShow:)
	 name:UIKeyboardWillShowNotification
	 object:nil];
	
	[[NSNotificationCenter defaultCenter]
	 addObserver:pKLView
	 selector:@selector(keyboardDidShow:)
	 name:UIKeyboardDidShowNotification
	 object:nil];
	 
	[[NSNotificationCenter defaultCenter]
	 addObserver:pKLView
	 selector:@selector(keyboardDidHide:)
	 name:UIKeyboardDidHideNotification
	 object:nil];
	[[NSNotificationCenter defaultCenter]
	 addObserver:pKLView
	 selector:@selector(keyboardWillHide:)
	 name:UIKeyboardWillHideNotification
	 object:nil];
	
}

void KLKeyboard_Unload(){
	
	if( pKLView->kbdfield )
	{
		
		[[NSNotificationCenter defaultCenter]
		 removeObserver:pKLView
		 name:UIKeyboardWillShowNotification
		 object:nil];
		[[NSNotificationCenter defaultCenter]
		 removeObserver:pKLView
		 name:UIKeyboardDidShowNotification
		 object:nil];
		[[NSNotificationCenter defaultCenter]
		 removeObserver:pKLView
		 name:UIKeyboardDidHideNotification
		 object:nil];
		[[NSNotificationCenter defaultCenter]
		 removeObserver:pKLView
		 name:UIKeyboardWillHideNotification
		 object:nil];
		
		[pKLView closeKeyboard];
		[pKLView removeFromSuperview];
#if __has_feature(objc_arc)
#else
		[pKLView->kbdfield release];
#endif
		pKLView->kbdfield.delegate = nil;
		pKLView->kbdfield = nil;
		
	}
	
}


//======================================================================
/**
 *	@brief	キーボード用テキストフィールドの座標を設定
 */
//======================================================================
void KLKeyboard_SetXY( s32 x, s32 y ){
	
	if( pKLView->kbdfield )
	{
		float r = 1.0f/KLDevice_GetScaleFactor();
		[pKLView->kbdfield setFrame: CGRectMake(
												 x*r, y*r,
												 pKLView->kbdfield.frame.size.width,
												 pKLView->kbdfield.frame.size.height )];
	}
	
}

//======================================================================
/**
 *	@brief	キーボード用テキストフィールドの座標を設定
 */
//======================================================================
void KLKeyboard_SetWH( u16 w, u16 h ){
	
	if( pKLView->kbdfield )
	{
		float r = 1.0f/KLDevice_GetScaleFactor();
		[pKLView->kbdfield setFrame: CGRectMake(pKLView->kbdfield.frame.origin.x,
												 pKLView->kbdfield.frame.origin.y, w*r,h*r )];
	}
	
}

//======================================================================
/**
 *	@brief	KLibのキーボードを開く関数 / 自前のテキストフィールドを使用するなら不要
 */
//======================================================================
void KLKeyboard_Open ( const char* pFirstStrOrContinueBeforeTextByNULL, void (*returnedCallback)(char* pInputText, void* pContext), void (*closedCallback)(char* pInputText, void* pContext), void* pContext ) {

	if ( pKLView->kbdfield )
	{
		
		if ( pFirstStrOrContinueBeforeTextByNULL )
		{
			
			pKLView->kbdfield.text = [NSString stringWithUTF8String:pFirstStrOrContinueBeforeTextByNULL];
			
		}
		
		pKLView->closeKbdCallback = closedCallback;
		pKLView->kbdReturnCallback= returnedCallback;
		pKLView->kbdfield.hidden = NO;
		pKLView->pKbdContext = pContext;
		[pKLView openKeyboard];
		
	}
	
}

//======================================================================
/**
 *	@brief	KLTextBorderStyle を UITextBorderStyle に変換して返す
 */
//======================================================================
UITextBorderStyle KLTextBorderStyleToUI ( KLTextBorderStyle style ) {
	
	UITextBorderStyle ret =	UITextBorderStyleRoundedRect;

	switch ( (int) style ) {
	
		case KLTextBorderStyleNone:
		{
			ret = UITextBorderStyleNone;
		}
		break;
			
		case KLTextBorderStyleLine:
		{
			ret = UITextBorderStyleLine;
		}
		break;
		
		case KLTextBorderStyleBezel:
		{
			ret = UITextBorderStyleBezel;
		}
		break;
		
	}
	
	return ret;
	
}

//======================================================================
/**
 *	@brief	KLReturnKeyType を UIReturnKeyType に変換して返す
 */
//======================================================================
UIReturnKeyType KLKeyboardReturnKeyTypeToUI ( KLReturnKeyType type ) {
	
	UIReturnKeyType ret = UIReturnKeyDefault;
		
	switch ( (int) type ) {
			
		case KLReturnKeyGo:
		{
			ret = UIReturnKeyGo;
		}
		break;
			
		case KLReturnKeyGoogle:
		{
			ret = UIReturnKeyGoogle;
		}
		break;
			
		case KLReturnKeyJoin:
		{
			ret = UIReturnKeyJoin;
		}
		break;
		
		case KLReturnKeyNext:
		{
			ret = UIReturnKeyNext;
		}
		break;
	
		case KLReturnKeyRoute:
		{
			ret = UIReturnKeyRoute;
		}
		break;
		
		case KLReturnKeySearch:
		{
			ret = UIReturnKeySearch;
		}
		break;
		
		case KLReturnKeySend:
		{
			ret = UIReturnKeySend;
		}
		break;
		
		case KLReturnKeyYahoo:
		{
			ret = UIReturnKeyYahoo;
		}
		break;
		
		case KLReturnKeyDone:
		{
			ret = UIReturnKeyDone;
		}
		break;
		
		case KLReturnKeyEmergencyCall:
		{
			ret = UIReturnKeyEmergencyCall;
		}
		break;
		
		default:
		{
			ret = UIReturnKeyDefault;
		}
		break;
			
	}
	
	return ret;
	
}

//======================================================================
/**
 *	@brief	KLTextFieldViewMode を UITextFieldViewMode に変換して返す
 */
//======================================================================
UITextFieldViewMode KLTextFieldViewModeToUI ( KLTextFieldViewMode mode ) {
	
	UITextFieldViewMode ret;
	
	switch ( (int)mode) {

		default:			// 表示しない
			
			ret = UITextFieldViewModeNever;
			
			break;
			
			
		case KLTextFieldViewModeAlways:			// 常に表示する
			
			ret = UITextFieldViewModeAlways;
			
			break;
			
			
		case KLTextFieldViewModeUnlessEditing:	// フォーカスが当たっていない時のみ表示する
		
			ret = UITextFieldViewModeUnlessEditing;
		
			break;
			
			
		case KLTextFieldViewModeWhiteEditing:	// フォーカスが当たっている時のみ表示する
			
			ret = UITextFieldViewModeWhileEditing;
			
			break;
			
	}
	
	return ret;
	
}

//======================================================================
/**
 *	@brief	KLKeyboardType を UIKeyboardType に変換して返す
 */
//======================================================================
UIKeyboardType KLKeyboardTypeToUI ( KLKeyboardType type ) {
	
	UIKeyboardType ret = UIKeyboardTypeDefault;
	
	switch ( (int) type ) {
			
		case KLKeyboardTypeDefault:
		{
			ret = UIKeyboardTypeDefault;
		}
		break;
			
		case KLKeyboardTypeASCIICapable:
		{
			ret = UIKeyboardTypeASCIICapable;
		}
		break;
			
		case KLKeyboardTypeNumbersAndPunctuation:
		{
			ret = UIKeyboardTypeNumbersAndPunctuation;
		}
		break;
			
		case KLKeyboardTypeURL:
		{
			ret = UIKeyboardTypeURL;
		}
		break;
			
		case KLKeyboardTypeNumberPad:
		{
			ret = UIKeyboardTypeNumberPad;
		}
		break;
			
		case KLKeyboardTypePhonePad:
		{
			ret = UIKeyboardTypePhonePad;
		}
		break;
			
		case KLKeyboardTypeNamePhonePad:
		{
			ret = UIKeyboardTypeNamePhonePad;
		}
		break;
			
		case KLKeyboardTypeEmailAddress:
		{
			ret = UIKeyboardTypeEmailAddress;
		}
		break;
			
		case KLKeyboardTypeDecimalPad:
		{
			ret = UIKeyboardTypeDecimalPad;
		}
		break;
			
		case KLKeyboardTypeTwitter:
		{
			ret = UIKeyboardTypeTwitter;
		}
		break;
			
		default:
		{
			ret = UIKeyboardTypeDefault;
		}
		break;
			
	}
	
	return ret;
	
}

//======================================================================
/**
 *	@brief	KLKeyboard のプレビューテキストフィールドの枠線を変更します
 */
//======================================================================
void KLKeyboard_SetBorderStyle ( KLTextBorderStyle style ) {
	
	if ( pKLView && pKLView->kbdfield )
	{
		
		pKLView->kbdfield.borderStyle = KLTextBorderStyleToUI (style);
		
	}
	
}

//======================================================================
/**
 *	@brief	KLKeyboard のキーボードタイプを変更します
 */
//======================================================================
void KLKeyboard_SetType ( KLKeyboardType type ) {

	if ( pKLView && pKLView->kbdfield )
	{
		
		pKLView->kbdfield.keyboardType = KLKeyboardTypeToUI(type);
	
	}
	
}

//======================================================================
/**
 *	@brief	KLKeyboard のリターンキータイプを変更します
 */
//======================================================================
void KLKeyboard_SetReturnKeyType ( KLReturnKeyType type ) {
	
	if ( pKLView && pKLView->kbdfield )
	{
		
		pKLView->kbdfield.returnKeyType = KLKeyboardReturnKeyTypeToUI (type);
		
	}
	
}

//======================================================================
/**
 *	@brief	KLKeyboard のクリアボタンタイプを変更します
 */
//======================================================================
void KLKeyboard_SetTextFieldClearBtn ( KLTextFieldViewMode mode ) {
	
	if ( pKLView && pKLView->kbdfield )
	{
		
		pKLView->kbdfield.clearButtonMode = KLTextFieldViewModeToUI (mode);
		
	}
	
}
