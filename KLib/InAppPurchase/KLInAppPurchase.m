//
//  KLInAppPurchase.m
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

#import "KLInAppPurchase.h"
#import "KLib.h"
#import "KLibDefine.h"
#import "KLNetwork.h"
#import <Foundation/Foundation.h>
#import <SystemConfiguration/SystemConfiguration.h>

void	(*KLInAppPurchase_getItemCallback)(void* pContext, s32 result, u32 codeOrListCount) = NULL;
void	(*KLInAppPurchase_buyOkCallback)(void* pContext) = NULL;
void	(*KLInAppPurchase_buyNgCallback)(void* pContext, u32 errcode, u32 canceledEnumInt) = NULL;
void	(*KLInAppPurchase_restoreOkCallback)(void* pContext, char* pProductId) = NULL;
void	(*KLInAppPurchase_restoreNgCallback)(void* pContext, u32 errCode, u32 canceledEnumInt) = NULL;
void	(*KLInAppPurchase_restoreAllEndCallback)(void* pContext) = NULL;

static void*			klinapppurchase_pContext= NULL;
static void*			klinapppurchase_pContextBuy= NULL;
static void*			klinapppurchase_pContextRestore= NULL;
static KLInAppPurchase* kliap					= nil;

KLInAppPurchase* KLInAppPurchase_GetInstance(){

	return  kliap;
	
}

void KLPurchase_Init(){
	
	if( kliap==nil )
	{
		kliap = [[KLInAppPurchase alloc] init];
	}
						   
}

void KLPurchase_CancelGetList(){
	
	[kliap cancelList];
	
}

void KLPurchase_BuyItem( char* pProductId, void(*successCallback)(void* pContext), void(*failedCallback)(void* pContext, u32 errcode, u32 canceledEnumInt), void* pContext ){
	
	if( KLPurchase_IsUsable(FALSE) )
	{
		KLInAppPurchase_buyOkCallback	= successCallback;
		KLInAppPurchase_buyNgCallback	= failedCallback;
		klinapppurchase_pContextBuy		= pContext;
		[kliap purchase:pProductId];
	}
	
}

void KLPurchase_RestoreItem(
	void(*successCallback)(void* pContext,char* pProductId),
	void(*failedCallback)(void* pContext, u32 errCode, u32 canceledEnumInt),
	void(*allEndCallback)(void* pContext),
	void* pContext ){
	
	KLInAppPurchase_restoreOkCallback		= successCallback;
	KLInAppPurchase_restoreNgCallback		= failedCallback;
	KLInAppPurchase_restoreAllEndCallback	= allEndCallback;
	klinapppurchase_pContextRestore			= pContext;
	[kliap restore];
	
}

static void KLPurchase_FreeBuffer(){
	
	for( int a=0; a<klib.purchase.length; ++a )
	{
		sfree(klib.purchase.ppTitle[a]);
		sfree(klib.purchase.ppDescription[a]);
		sfree(klib.purchase.ppPrice[a]);
		sfree(klib.purchase.ppPid[a]);
	}
	
}

void KLPurchase_Unload(){
	
	KLPurchase_FreeBuffer();
	sfree(klib.purchase.ppTitle);
	sfree(klib.purchase.ppDescription);
	sfree(klib.purchase.ppPrice);
	sfree(klib.purchase.ppPid);
	klib.purchase.length = 0;
	
	if( kliap )
	{
		[kliap release];
		kliap = nil;
	}
	
}

//=======================================================
/**
 *	@brief	アプリ内課金が使用可能かを調べる
 *
 *	@param	dontPopup	使用不可な時はアラートを表示するが、<br/>
 *						そのアラートを表示せず結果だけ欲しい時はFALSE
 *
 *	@return	アプリ内課金が使用可能ならTRUE / 使用不可ならFALSE
 */
//=======================================================
bl KLPurchase_IsUsable(bl dontPopup){
	
	bl ret = FALSE;
	
	if( kliap )
	{
		ret = [kliap checkUsable:dontPopup];
	}
	
	return ret;
}

//=====================================================================
/**
 *	@brief	課金アイテムのリストを取得しコールバックに渡す
 *
 *	@param	ppProductIdList	プロダクトID文字列リストの先頭ポインタ
 *	@param	nList			プロダクトID文字列リストの要素数
 *	@param	pContext		コールバックに一緒に渡して欲しいものがあればそのポインタ
 *
 *	@return	処理が開始できた場合はTRUE / 既に接続中ならFALSE<br/>
 *			取得したアイテムリストはコールバック経由で渡されます。
 */
//=====================================================================
bl KLPurchase_GetList( char** ppProductIdList, u32 nList, void(*getItemCallback)(void* pContext, s32 result, u32 codeOrListCount), void* pContext ){
	
	if( KLPurchase_IsUsable(FALSE) )
	{
	
		if( !klib.purchase.connecting )
		{
			KLInAppPurchase_getItemCallback = getItemCallback;
			klinapppurchase_pContext = pContext;
			[kliap getItemList:ppProductIdList count:nList];
			return TRUE;
		}
		else
		{
			KLLog("[KLPurchase] Already connecting.\n");
		}
	
	}
	
	return FALSE;
	
}

@implementation KLInAppPurchase
- (id) init{
	
	[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
	return self;
	
}

- (void) dealloc{

	[[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
	[super dealloc];
}

- (bl) isEnableNetwork:(bl)dontPopup {

	if( KLNetwork_IsConnecting() )
	{
		return TRUE;
	}
	
	if( !dontPopup )
	{
		KLAlert_Open("",
					 "Could not connect to network.\nplease retry later or check network connection.","OK",NULL);
	}
	
	return FALSE;
	
}

- (void) purchase: (char*)productIdentifier {
	
	//payment = [SKPayment paymentWithProductIdentifier:[NSString stringWithUTF8String:productIdentifier]];
	SKMutablePayment* payments = [[[SKMutablePayment alloc] init] autorelease];
	[payments setProductIdentifier:[NSString stringWithUTF8String:productIdentifier]];
	[payments setQuantity:(NSInteger)1];
	klib.purchase.connecting = TRUE;
	[[SKPaymentQueue defaultQueue] addPayment:payments];
	
}

- (void) cancelList{

	if( request != nil )
	{
		klib.purchase.connecting = FALSE;
		[request cancel];
		[request release];
		request = nil;
	}
	
}

- (void)restore {
	
	klib.purchase.connecting = TRUE;
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
	
}

- (BOOL) checkUsable:(bl)dontPopup{
	
	if (![SKPaymentQueue canMakePayments])
	{
		
		if(!dontPopup)
		{
			KLAlert_Open("", "You can not use the In-App purchase.\nPlease try again after enabled in your device settings.","OK", NULL);
		}
		
		return FALSE;
	
	}
	
	return TRUE;
}

- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response {
	
	u32 nNg = [response.invalidProductIdentifiers count];
	
	// パラメータ異常
	if( response == nil )
	{
		KLInAppPurchase_getItemCallback( klinapppurchase_pContext, KLINAPPPURCHASE_RES_NIL, nNg );
		klib.purchase.connecting = FALSE;
		return;
	}
	
	// 無効なアイテムをチェック
	if( nNg > 0 )
	{
		for (NSString *identifier in response.invalidProductIdentifiers)
		{
			
			NSLog(@"[KLInAppPurchase] invalid product identifier: %@", identifier);
		
		}
		KLInAppPurchase_getItemCallback( klinapppurchase_pContext, KLINAPPPURCHASE_RES_INVALID_ITEMS, nNg );
		klib.purchase.connecting = FALSE;
		return;
	}
	else
	{
		int n = response.products.count;
		int i = 0;
		NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
		[numberFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
		[numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
		
		if( n > klib.purchase.length )
		{
			KLPurchase_FreeBuffer();
			
			klib.purchase.length		= n;
			klib.purchase.ppTitle		= (char**)realloc(klib.purchase.ppTitle, sizeof(char*)*n );
			klib.purchase.ppDescription	= (char**)realloc(klib.purchase.ppDescription, sizeof(char*)*n );
			klib.purchase.ppPrice		= (char**)realloc(klib.purchase.ppPrice, sizeof(char*)*n );
			klib.purchase.ppPid			= (char**)realloc(klib.purchase.ppPid, sizeof(char*)*n );
		}
		
		for(SKProduct *product in response.products)
		{
			NSString* title	= [product localizedTitle];
			NSString* desc	= [product localizedDescription];
			[numberFormatter setLocale:product.priceLocale];
			NSString* price = [numberFormatter stringFromNumber:product.price];
			NSString* pid	= [product productIdentifier];
			const char* cTitle	= [title cStringUsingEncoding:NSUTF8StringEncoding];
			const char* cDesc	= [desc cStringUsingEncoding:NSUTF8StringEncoding];
			const char* cPrice	= [price cStringUsingEncoding:NSUTF8StringEncoding];
			const char* cPid	= [pid cStringUsingEncoding:NSUTF8StringEncoding];
			u32 lent = strlen(cTitle);
			u32 lend = strlen(cDesc);
			u32 lenp = strlen(cPrice);
			u32 leni = strlen(cPid);
			
			KLLog("\n========================\n[KLInAppPurchase]\n");
			KLLog("Title:\n\t%s\n", cTitle);
			KLLog("Descr:\n\t%s\n", cDesc);
			KLLog("Price:\n\t%s\n", cPrice);
			KLLog("PrdId:\n\t%s\n", cPid);
			KLLog("========================\n\n");
			
			klib.purchase.ppTitle[i]		= (char*)Malloc(lent+1);
			klib.purchase.ppDescription[i]	= (char*)Malloc(lend+1);
			klib.purchase.ppPrice[i]		= (char*)Malloc(lenp+1);
			klib.purchase.ppPid[i]			= (char*)Malloc(leni+1);
			
			strcpy( klib.purchase.ppTitle[i],		cTitle );
			strcpy( klib.purchase.ppDescription[i],	cDesc );
			strcpy( klib.purchase.ppPrice[i],		cPrice );
			strcpy( klib.purchase.ppPid[i],			cPid );
			
			++i;
		}
		
		[numberFormatter release];
		numberFormatter				= nil;
		klib.purchase.connecting	= FALSE;
		
		KLInAppPurchase_getItemCallback( klinapppurchase_pContext, KLINAPPPURCHASE_RES_OK, 0 );

	}
	
}

- (void) getItemList:(char**)pProductIdAry count:(unsigned int)count{
	
	NSSet*				set		= nil;
	NSMutableArray*		mary	= [NSMutableArray array];
	NSString*			nstr	= nil;
	
	for( int i=0; i<count; ++i )
	{
		nstr = [NSString stringWithUTF8String:pProductIdAry[i]];
		[mary addObject:nstr];
	}
	set							= [NSSet setWithArray:mary];
	request						= [[SKProductsRequest alloc] initWithProductIdentifiers:set];
	request.delegate			= self;
	klib.purchase.connecting	= TRUE;
	[request start];
	
}

- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
	
	KLLog("[KLPurchase] Update %d\n", [transactions count]);
								
    for (SKPaymentTransaction *transaction in transactions)
	{
		switch (transaction.transactionState)
		{
		
			case SKPaymentTransactionStatePurchasing:
			{
				// 購入処理中。基本何もしなくてよい。処理中であることがわかるようにインジケータをだすなど。
			}
			break;

			case SKPaymentTransactionStatePurchased:
			{
				// 購入処理成功
				//[self showAlert:@"購入処理成功！"];
				NSLog(@"購入処理成功！");
				NSLog(@"transaction : %@",transaction);
				NSLog(@"transaction.payment.productIdentifier : %@",transaction.payment.productIdentifier);
				klib.purchase.connecting = 0;
				
				// FALSE返しでトランザクションを保持させるようにするかも
				if( KLInAppPurchase_buyOkCallback )
				{
					KLInAppPurchase_buyOkCallback(klinapppurchase_pContextBuy);
				}
				[queue finishTransaction: transaction];
			}
			break;
				
			case SKPaymentTransactionStateFailed:
			{
				[queue finishTransaction:transaction];
				klib.purchase.connecting = 0;
				
				if( KLInAppPurchase_buyNgCallback )
				{
					KLInAppPurchase_buyNgCallback(klinapppurchase_pContextBuy,transaction.error.code,SKErrorPaymentCancelled);
				}
				
				// 購入処理失敗。ユーザが購入処理をキャンセルした場合もここ
				if (transaction.error.code != SKErrorPaymentCancelled)
				{
					NSLog(@"[KLPurchase] Failed purchase.");
					NSLog(@"\t%@",[transaction.error localizedDescription]);
					
					// 規約更新ダイアログなどiOSが購入前に必要なダイアログが出た場合
					// ユーザーが同意後、再び購入するかダイアログがシステム側から表示され
					// 下記エラーコードを捌かずにユーザーが購入を押すと
					// 二重課金が発生する可能性があるようなので注意する
					if (transaction.error.code == SKErrorUnknown)
					{
					
						UIAlertView *alert = [[UIAlertView alloc] initWithTitle:[transaction.error localizedFailureReason]
																		message:[transaction.error localizedDescription]
																	   delegate:self
															  cancelButtonTitle:nil
															  otherButtonTitles:@"OK", nil];
						[alert show];
						[alert release];
					
					}
					
				}
				
			}
			break;
			
			case SKPaymentTransactionStateRestored:
			{
				//リストア処理開始
				//[self showAlert:@"リストア処理開始"];
				NSLog(@"リストア処理");
				[queue finishTransaction:transaction];
				klib.purchase.connecting = FALSE;
				if( KLInAppPurchase_restoreOkCallback )
				{
					KLInAppPurchase_restoreOkCallback(
						klinapppurchase_pContextRestore,
						(char*)[transaction.originalTransaction.payment.productIdentifier cStringUsingEncoding:NSUTF8StringEncoding]
					);
				}
				// この後別イベントが発行されるので注意
			}
			break;
				
			default:
			{
				NSLog(@"[KLPurchase] updated");
			}
			break;
		}

	}
		
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue{
	
	KLLog("[KLPurchase] RestoreOK\n");
	
	if( KLInAppPurchase_restoreAllEndCallback )
	{
		KLInAppPurchase_restoreAllEndCallback( klinapppurchase_pContextRestore );
	}
	/*
	klib.purchase.connecting = 0;
	for (SKPaymentTransaction *transaction in queue.transactions)
	{
		[queue finishTransaction:transaction];
	}
	*/
}

- (void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error{
	
	KLLog("[KLPurchase] RestoreNG\n");
	
	klib.purchase.connecting = 0;
	KLInAppPurchase_restoreNgCallback( klinapppurchase_pContextRestore, error.code,SKErrorPaymentCancelled );
	KLInAppPurchase_restoreAllEndCallback( klinapppurchase_pContextRestore );
	
}

@end
