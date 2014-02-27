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

#import "KLHttp.h"

@implementation KLHttp

/// サーバからレスポンスが送られてきたときのデリゲート
- (void)connection:(NSURLConnection *)i_connection didReceiveResponse:(NSURLResponse *)i_response{

	// 一応受信はできたのでタイマー再設定で続行
	[self->timer invalidate];
	[self startTimer];
	
	statusCode = (int)[((NSHTTPURLResponse *)i_response) statusCode];
	KLLog("didReceiveResponse:%d\n", statusCode);
	
	[receivedData setLength:0];
	
}

/// サーバからデータが送られてきたときのデリゲート
- (void)connection:(NSURLConnection *)i_connection didReceiveData:(NSData *)i_data{

	// 一応受信はできたのでタイマー再設定で続行
	[self->timer invalidate];
	[self startTimer];
	KLLog("receivedData\n");
	[receivedData appendData:i_data];
	
}

/// データのロードか完了した時のデリゲート
- (void)connectionDidFinishLoading:(NSURLConnection *)i_connection{

	connecting = 0;
	[self->timer invalidate];
	
	// 旧式では文字列しか対応しないのでバイナリ対応するためにそのまま渡す
#if 0
	NSString* receivedString = [[NSString alloc] initWithData:receivedData encoding:NSUTF8StringEncoding];
	if (receivedString == nil)
	{
		KLLog("[KLHttp] failed rdata to string.\n");
		callback( (char*)[receivedData bytes], KLHTTP_CANT_TOSTRING, [receivedData length], statusCode, pContext );
	}
	else
	{
		callback( (char*)[receivedString UTF8String], KLHTTP_OK, [receivedData length],statusCode, pContext );
	}
	
	KLLog("connectionDidFinishLoading\n");
#if __has_feature(objc_arc)
#else
	[receivedString release];
#endif
	receivedString = nil;

#endif

	if (callback)
	{
		callback ( (char*)[receivedData bytes], KLHTTP_OK,  statusCode, [receivedData length], pContext );
	}
	
	[self endConnect];
	
}

/// サーバからエラーが返されたときのデリゲート
- (void)connection:(NSURLConnection *)i_connection didFailWithError:(NSError *)i_error{

	connecting = 0;
	[self->timer invalidate];
	
	KLLog("didFailWithError\n");
	
}

- (void)unload{
	
	if( receivedData!=nil )
	{
#if __has_feature(objc_arc)
#else
		[receivedData release];
#endif
		receivedData = nil;
	}
	[self endConnect];
	
}

- (void)allocRequest{

	if( req!=nil )
	{
#if __has_feature(objc_arc)
#else
		[req release];
#endif
		req = nil;
	
		KLLog("[KLHttp] released post.\n");
		
	}
	req = [NSMutableURLRequest alloc];
	KLLog("[KLHttp] alloced post.\n");
	
}

-(void)endConnect{
	
	if( req )
	{
#if __has_feature(objc_arc)
#else
		[req release];
#endif
		req = nil;
	}
	
	if( nscon )
	{
		[nscon cancel];
#if __has_feature(objc_arc)
#else
		[nscon release];
#endif
		nscon = nil;
	}
	
}

-(void)cancelConnect{

	[self endConnect];
	KLLog("[KLHttp] connect canceled.\n");
	
}

-(void)timeout:(NSTimer*)timer{

	[self->timer invalidate];
	KLLog("[KLHttp] Timeout\n");
	
	if( connecting )
	{
		
		[self cancelConnect];
		
		if( self->callback )
		{
			self->callback( NULL, KLHTTP_TIMEOUT, 0, receivedData ? [receivedData length] : 0, pContext );
		}
		
		timeouted = 1;
		
	}
	
}

- (void)startTimer{
	
	timeouted = 0;
	
	if( timer!=nil )
	{
		[timer invalidate];
		timer = nil;
		
		KLLog("[KLHttp] stop timer.\n");
		
	}
	timer = [NSTimer
				scheduledTimerWithTimeInterval:timeout
				target:self
				selector:@selector(timeout:)
				userInfo:nil
				repeats:NO
	];
	KLLog("[KLHttp] start timer.\n");
	
}

- (void)setupGet:(char*)pStrUrl timeout:(float)fTimeout{
	
	NSString*	nstr	= [NSString stringWithUTF8String:pStrUrl];
    NSURL*		url		= [NSURL URLWithString:nstr];
	
	self->timeout = fTimeout;
	
	[self allocRequest];
	
	req = [req initWithURL:url];
	[req setHTTPMethod:@"GET"];
	
}

- (void)setupPost:(char*)pStrUrl params:(char*)pStrParams timeout:(float)fTimeout{
	
	NSString*	nstr	= [NSString stringWithUTF8String:pStrUrl];
    NSString*	content = pStrParams ? [NSString stringWithUTF8String:pStrParams] : NULL;
	NSURL*		url		= [NSURL URLWithString:nstr];
	
	self->timeout = fTimeout;
	
	[self allocRequest];
	
	req = [req initWithURL:url];
	[req setHTTPMethod:@"POST"];
	
	if ( content )
	{
	
		[req setHTTPBody:[content dataUsingEncoding:NSUTF8StringEncoding]];
	
	}
	
}

- (bl)startConnect{

	NSError*	error	= nil;
	
	if( !connecting)
	{
		if( receivedData!=nil )
		{
#if __has_feature(objc_arc)
#else
			[receivedData release];
#endif
			receivedData = nil;
		}
#if __has_feature(objc_arc)
		receivedData = [NSMutableData data];
#else
		receivedData = [[NSMutableData data] retain];
#endif
		[self startTimer];
		nscon = [[NSURLConnection alloc] initWithRequest:req delegate:self];
		connecting = 1;
	}
	else
	{
		KLLog("POST error = already connecting\n");
		return 0;
	}
	
	if(error)
	{
		KLLog("POST error = %s\n", [[error localizedDescription] UTF8String] );
		return 0;
	}
	
	return 1;
	
}

- (void)addHeader:(char*)pStrHeaderType value:(char*)pStrValue{
	
	[req addValue:[NSString stringWithUTF8String:pStrHeaderType] forHTTPHeaderField:[NSString stringWithUTF8String:pStrValue]];
	
}

@end

void KLHttp_AddHeader( KLHttpReq* p, char* pStrType, char* pStrValue ){

#if __has_feature(objc_arc)

	[(__bridge KLHttp*)p->http addHeader:pStrType value:pStrValue];

#else
	
	[(KLHttp*)p->http addHeader:pStrType value:pStrValue];
	
#endif
	
}

void KLHttp_InitGet( KLHttpReq* p, char* pStrUrl, float timeout ){
#if __has_feature(objc_arc)
	p->http = (void *)CFBridgingRetain([[KLHttp alloc] init]);
	[(__bridge KLHttp*)(p->http) setupGet:pStrUrl timeout:timeout];
#else
	p->http = [[KLHttp alloc] init];
	[(KLHttp*)p->http setupPost:pStrUrl params:pStrParams timeout:timeout];	
#endif
}

void KLHttp_InitPost( KLHttpReq* p, char* pStrUrl, char* pStrParams, float timeout ){
#if __has_feature(objc_arc)
	p->http = (void *)CFBridgingRetain([[KLHttp alloc] init]);
	[(__bridge KLHttp*)(p->http) setupPost:pStrUrl params:pStrParams timeout:timeout];
#else
	p->http = [[KLHttp alloc] init];
	[(KLHttp*)p->http setupPost:pStrUrl params:pStrParams timeout:timeout];
#endif
}

void KLHttp_Start( KLHttpReq* p, void(*callback)(bin8*,KLHttpEvent,int,size_t,void*), void* pContext ){

#if __has_feature(objc_arc)
	
	((__bridge KLHttp*)p->http)->callback = callback;
	((__bridge KLHttp*)p->http)->pContext = pContext;
	[(__bridge KLHttp*)p->http startConnect];
	
#else
	
	((KLHttp*)p->http)->pContext = pContext;
	((KLHttp*)p->http)->callback = callback;
	[(KLHttp*)p->http startConnect];

#endif

}

void KLHttp_Unload( KLHttpReq* p ){

	if( p && p->http )
	{
#if __has_feature(objc_arc)

		[(__bridge KLHttp*)p->http unload];

#else
		[(KLHttp*)p->http unload];
		[(KLHttp*)p->http release];
#endif
		p->http = nil;
	}
	
}

void KLHttp_Cancel( KLHttpReq* p ){
	
	if( p && p->http )
	{
#if __has_feature(objc_arc)
		
		[(__bridge KLHttp*) p->http cancelConnect];
		
#else
		
		[(KLHttp*) p->http cancelConnect];

#endif
		
		
	}
	
}