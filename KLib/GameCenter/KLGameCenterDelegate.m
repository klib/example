//
//  KLGameCenterDelegate.m
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

#import "KLGameCenterDelegate.h"
#import "KLString.h"
#import "KLibView.h"
#import <GameKit/GameKit.h>

//=============================================================
/**
 *	@brief	スコアレポートコールバックを設定
 *
 *	@param	reportCallback	結果を受け取るコールバックのポインタ
 *	@param	pContext		コンテキスト
 */
//=============================================================
void KLGameCenter_SetReportCallback( void(*reportCallback)(bl okng, u32 errCode, void* pContext), void* pContext ){
	
	klib.gamecenter.reportCallback	= reportCallback;
	klib.gamecenter.reportContext	= pContext;
	
}

//=============================================================
/**
 *	@brief	GameCenterにスコア送信を行いコールバックに結果を返します<br/>
 *			未ログインの場合はFALSEを返します。
 *
 *	@param	pLeaderboardId	iTunesConnectで設定したリーダーボードのID文字列
 *	@param	value			設定するスコア数値<br/>
 *							floatや時間もiTunesConnectで設定するフォーマットに合わせて<br/>
 *							int64_tで渡してください。
 *	@param	reportCallback	結果を受け取るコールバックのポインタ
 *	@param	pContext		コンテキスト
 */
//=============================================================
bl KLGameCenter_ReportScore( char* pLeaderboardId, int64_t value, void(*reportCallback)(bl okng, u32 errCode, void* pContext), void* pContext ){

	bl ret = TRUE;
	
	GKScore* scoreReporter = [[[GKScore alloc] initWithCategory:[NSString stringWithUTF8String:pLeaderboardId]] autorelease];
	
	KLLog("[ KLGameCenter ] Send score[%s] %llu\n", pLeaderboardId, value);
	
	scoreReporter.value = value;
	
	if( !klib.gamecenter.success )
	{
		KLLog("[ KLGameCenter ] Score report failed. because can not login gamecenter yet.\n");
		ret = FALSE;
	}
	
	KLGameCenter_SetReportCallback(reportCallback,pContext);
	
	[scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
		
		if( klib.gamecenter.reportCallback )
		{
			if (error != nil)
			{
				klib.gamecenter.reportCallback( FALSE, error.code, klib.gamecenter.reportContext );
			}
			else
			{
				klib.gamecenter.reportCallback( TRUE, 0, klib.gamecenter.reportContext );
			}
		}
		
	}];
	
	return ret;
	
}

static void KLGameCenter_Authenticated( GKLocalPlayer* localPlayer ){

	NSString*		alias	= [localPlayer alias];
	NSString*		pid		= [localPlayer playerID];
	const char*		pAlias	= [alias UTF8String];
	const char*		pPid	= [pid UTF8String];
	
	KLLog("[ KLGameCenter ] Auth OK:\n\tAlias:%s\n\tPID  :%s\n", pAlias, pPid );
	
	if( strlen(pAlias)>=KLGAMECENTER_ALIAS_MAXLEN )
	{
		KLLog("[ KLGameCenter ] !!!!! PlayerAlias buffer length is too low. !!!!!\n");
		return;
	}
	
	if( strlen(pPid)>=KLGAMECENTER_PID_MAXLEN )
	{
		KLLog("[ KLGameCenter ] !!!!! PlayerID buffer length is too low. !!!!!\n");
		return;
	}
	
	strcpy( klib.gamecenter.alias,	pAlias );
	strcpy( klib.gamecenter.pid,	pPid );
	
	// 認証成功
	klib.gamecenter.success = TRUE;
	klib.gamecenter.usable	= TRUE;
	
}

static void KLGameCenter_AuthNG( int errCode ){

	KLLog("[ KLGameCenter ] Auth failed.\n");
	
	// エラー
	klib.gamecenter.success = FALSE;
	klib.gamecenter.code	= (int)errCode;
	klib.gamecenter.usable	= klib.gamecenter.code==GKErrorNotSupported;
	
}

//=============================================================
/**
 *	@brief	GameCenterの認証を行いコールバックに結果を返します<br/>
 *			未ログインの場合はGameCenterが認証画面を表示しますが、<br/>
 *			ログイン画面のみランドスケープには非対応のようです。
 *
 *	@param	pFunc	KLGameCenterCallback形式の関数ポインタ
 */
//=============================================================
void KLGameCenter_Auth( KLGameCenterCallback pFunc ){

	GKLocalPlayer *localPlayer = [GKLocalPlayer localPlayer];
	
	klib.gamecenter.connecting	= TRUE;
	klib.gamecenter.success		= FALSE;
	klib.gamecenter.code		= 0;
	klib.gamecenter.usable		= 0;
	
	KLLog("[ KLGameCenter ] StartAuth.\n");
	
	if( klib.device.iosVer<6.0f )
	{
		// IOS 6 未満( IOS6でも警告が消えないのでpragmaで一時的に警告を無視する )
		#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		[localPlayer authenticateWithCompletionHandler: ^(NSError *error)
		{
		#pragma GCC diagnostic warning "-Wdeprecated-declarations"
			// この方式だとGCのログインビューはランドスケープで出ないっぽい？
			if (localPlayer.isAuthenticated)
			{
				
				KLGameCenter_Authenticated( localPlayer );
				 
			}
			else
			{
				
				KLGameCenter_AuthNG( (int)[error code] );
				
			}
			 
			klib.gamecenter.connecting = FALSE;
			 
			if( pFunc )
			{
				pFunc();
			}
			 
		}];
		
	}
	else
	{
		localPlayer.authenticateHandler = ^(UIViewController *viewController, NSError* error)
		{
			
			if (viewController != nil)
			{
				#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				[KLibView_GetController() presentModalViewController:viewController animated:YES];
				#pragma GCC diagnostic warning "-Wdeprecated-declarations"
			}
			else if (localPlayer.isAuthenticated)
			{
				
				KLGameCenter_Authenticated( localPlayer );
				
			}
			else
			{
				
				KLGameCenter_AuthNG( (int)[error code] );
				
			}
			
			klib.gamecenter.connecting = FALSE;
			
			if( pFunc )
			{
				pFunc();
			}
				 
		};
				
	}

}

// !!! コールバックが１つに上書きするので同時に複数ランキング取得は未サポートです。
// 必ず１つが終わってから次を発行してください。

static KLLeaderboard*		KLGameCenter_pLoadObj			= NULL;
static void*				KLGameCenter_pGetRankingContext	= NULL;
static bl					KLGameCenter_isGettingScore		= FALSE;
static bl					KLGameCenter_isGettingPlayer	= FALSE;
static KLLeaderboardScore*	KLGameCenter_pVirtualScore		= NULL;
static u32					KLGameCenter_nVirtualScore		= 0;
static bl					KLGameCenter_isAscending		= TRUE;
static u32					KLLeaderboard_nForLabelLength	= 0;
static char*				KLLeaderboard_pStrForLabel		= NULL;

void KLGameCenter_Unload(){

	sfree(KLGameCenter_pVirtualScore);
	sfree(KLLeaderboard_pStrForLabel);
	
}

//======================================================================
/**
 *	@brief		仮想スコアを作成しLeaderboardをテストしたい場合はまずこれで初期化する
 *
 *	@warning	大量のスコアが必要なアプリの場合、逐一iTunesConnectで<br/>
 *				大量のGameCenterテストアカウントを作成するのは<br/>
 *				現実的ではありません。そこで、スコアデータを仮で作成し、<br/>
 *				Leaderboardの結果をその仮想データで返すことで<br/>
 *				複数プレイヤーがいる状況を想定したデバッグが行えます。<br/>
 *				当然ながら、リリース時のGameCenterの応答を完全再現しているわけではないので<br/>
 *				必ず仮想スコアを使用しないLeaderboardのテストも行なってください。
 */
//======================================================================
bl KLGameCenter_InitVirtualScore( u32 nAlloc ){

	#if KL_DEBUG_LEADERBOARD_RANKING
	KLGameCenter_nVirtualScore	= (nAlloc==0?100:nAlloc);
	KLGameCenter_pVirtualScore	= (KLLeaderboardScore*)realloc( KLGameCenter_pVirtualScore, sizeof(KLLeaderboardScore)*KLGameCenter_nVirtualScore );
	
	if( !KLGameCenter_pVirtualScore )
	{
		KLLog("[ KLGameCenter ] Alloc VirtualScore failed.\n");
		return FALSE;
	}
	
	#endif
	
	return TRUE;
	
}

//======================================================================
/**
 *	@brief		仮想スコアを作成します
 *
 *	@param		topScore		1位のスコア
 *	@param		isAscending		昇順スコアならTRUE / 降順ならFALSE
 *	@param		scoreUpRateMin	2位以降のスコアに加算(減算)される最小値(負数不可)
 *	@param		scoreUpRateMax	2位以降のスコアに加算(減算)される最大値(負数不可)
 *
 *	@warning	大量のスコアが必要なアプリの場合、逐一iTunesConnectで<br/>
 *				大量のGameCenterテストアカウントを作成するのは<br/>
 *				現実的ではありません。そこで、スコアデータを仮で作成し、<br/>
 *				Leaderboardの結果をその仮想データで返すことで<br/>
 *				複数プレイヤーがいる状況を想定したデバッグが行えます。<br/>
 *				当然ながら、リリース時のGameCenterの応答を完全再現しているわけではないので<br/>
 *				必ず仮想スコアを使用しないLeaderboardのテストも行なってください。<br/><br/>
 *
 *				リーダーボードは最高スコアが優先されます。<br/>
 *				過去に今回より良いスコアがある場合、そちらがリーダーボードに残り<br/>
 *				取得できる自分の順位も今回の順位ではなく過去最高の方が優先されます。<br/>
 *				詳しくは公式ドキュメントを御覧ください。
 */
//======================================================================
void KLGameCenter_MakeAutoVirtualScore( int64_t topScore, bl isAscending, u32 scoreUpRateMin, u32 scoreUpRateMax ){

	#if KL_DEBUG_LEADERBOARD_RANKING
	
	u32 diff = 0;
	
	if( scoreUpRateMax<scoreUpRateMin )
	{
		scoreUpRateMax = scoreUpRateMin+1;
	}
	
	KLGameCenter_isAscending	= isAscending;
	
	diff = scoreUpRateMax-scoreUpRateMin;
	
	if( KLGameCenter_pVirtualScore )
	{
		
		for( int i=0; i<KLGameCenter_nVirtualScore; ++i )
		{
			u32 up = rand()%diff+scoreUpRateMin;
			
			KLGameCenter_pVirtualScore[i].rank = i+1;
			KLGameCenter_pVirtualScore[i].score= topScore;
			sprintf( KLGameCenter_pVirtualScore[i].pid, "KLDummy@%u", i+1 );
			
			// 10*n+8位9位は同順テストとしてスコア更新しない
			if( i%10!=8 )
			{
				topScore += (isAscending? up : -up );
			}
			
			switch (i)
			{
				// ひら・カナ・漢字＋絵文字
				case 0:
				{
				
					sprintf(
						KLGameCenter_pVirtualScore[i].alias,
						"ひらカナｶﾅ漢字@ABC"
					);
					
				}
				break;
				
				// スペース＋他言語文字＋サロゲートペア文字
				case 1:
				{
					
					sprintf(
							KLGameCenter_pVirtualScore[i].alias,
							" å∫ç∂　ƒИ 𩸽"
							);
					
				}
				break;
					
					
				default:
				{
					if( i%4==0 )
					{
						sprintf(
							KLGameCenter_pVirtualScore[i].alias,
							"Player %d", i+1
							);
					}
					else if( i%4==1 )
					{
						sprintf(
							KLGameCenter_pVirtualScore[i].alias,
							"ぷれいやー %d", i+1
							);
					}
					else if( i%4==2 )
					{
						sprintf(
							KLGameCenter_pVirtualScore[i].alias,
							"プレイヤー %d", i+1
							);
					}
					else if( i%4==3 )
					{
						sprintf(
							KLGameCenter_pVirtualScore[i].alias,
							"ﾌﾟﾚｲﾔｰ %d", i+1
							);
					}
					
				}
				break;
			
			}
			
		}
		
	}

	#endif
	
}

void KLLeaderboard_Unload( KLLeaderboard* pKLLeaderboard ){

	sfree( pKLLeaderboard->pScore );
	
}

/// KLLabel作成用に指定リーダーボードの全スコアAliasをつなげて返す
char* KLLeaderboard_GetChainAliasStringForLabel( KLLeaderboard* pKLLeaderboard ){

	u32 total = 0;
	
	for( int i=0; i<pKLLeaderboard->nScore; ++i )
	{
		total += strlen(pKLLeaderboard->pScore[i].alias);
	}
	
	if( !total )
	{
		return "";
	}
	
	if( KLLeaderboard_nForLabelLength<total )
	{
		KLLeaderboard_nForLabelLength	= total;
		KLLeaderboard_pStrForLabel		= realloc(KLLeaderboard_pStrForLabel, total+4);
		memset( KLLeaderboard_pStrForLabel, 0, total+4 );
	}
	
	int next = 0;
	
	for( int i=0; i<pKLLeaderboard->nScore; ++i )
	{
		next += sprintf( &KLLeaderboard_pStrForLabel[next], "%s", pKLLeaderboard->pScore[i].alias );
	}
	
	return KLLeaderboard_pStrForLabel;
	
}

// 同時にWeek/Todayなど複数取得はまだ非対応
bl KLGameCenter_GetRanking( char* pLeaderboardId, u32 minRank, u32 maxRank, bl isFriendOnly, u8 typeAll0_Week1_Today2, KLLeaderboard* pKLLeaderboard, void(*getRankingCallback)(bl okng, u32 errCode, void* pContext), void* pContext ){
	
	/*
	 スコアリスト取得はプレイヤー名は返ってこず、IDしかわからないので
	 各プレイヤーIDからさらにプレイヤーリストを取得し
	 スコアリスト取得とプレイヤー名をすべて取得したら成功としています。
	 クロージャを使わるを得ないのでpContextをクロージャから外部参照してもいいのですが
	 今回、処理オブジェクトを適当にまとめる雑なやり方をしてます。
	 故に煩雑かつ汚いコードになってます。
	 */
	KLLog("[ KLGameCenter ] Get rank : %s\n", pLeaderboardId );
	
	if( !KLGameCenter_isGettingScore )
	{
		GKLeaderboard *leaderboardRequest = [[GKLeaderboard alloc] init];
		if (leaderboardRequest != nil)
		{
			KLGameCenter_pLoadObj			= pKLLeaderboard;
			KLGameCenter_pGetRankingContext	= pContext;
			KLGameCenter_isGettingScore		= TRUE;
			
			leaderboardRequest.playerScope =
				isFriendOnly ?	GKLeaderboardPlayerScopeFriendsOnly :
								GKLeaderboardPlayerScopeGlobal;
			
			leaderboardRequest.timeScope =
				typeAll0_Week1_Today2 == 0 ?	GKLeaderboardTimeScopeAllTime:
				typeAll0_Week1_Today2 == 1 ?	GKLeaderboardTimeScopeWeek :
												GKLeaderboardTimeScopeToday;
			
			
			leaderboardRequest.category = [NSString stringWithUTF8String:pLeaderboardId];
			leaderboardRequest.range = NSMakeRange(minRank,maxRank);
			[leaderboardRequest loadScoresWithCompletionHandler: ^(NSArray *scores, NSError *error)
			{
				if( leaderboardRequest.localPlayerScore!=nil )
				{
					
					KLLog("[ KLGameCenter ] Your rank:%u\n", leaderboardRequest.localPlayerScore.rank );
					KLGameCenter_pLoadObj->localPlayerScore.rank = leaderboardRequest.localPlayerScore.rank;
					KLGameCenter_pLoadObj->localPlayerScore.score= leaderboardRequest.localPlayerScore.value;
				
				}
				
				if (error != nil)
				{
					
					KLGameCenter_isGettingScore = FALSE;
					getRankingCallback( FALSE, error.code, KLGameCenter_pGetRankingContext );
					[leaderboardRequest release];
					return;
					
				}
				
				if (scores != nil)
				{
					GKScore* pScore = nil;
					
					int findmypos = -1;
					int m = 0;
					bl  insertMe = 0;
					
					KLGameCenter_pLoadObj->nScore = [scores count];
					
					KLLog("[ KLGameCenter ] received score num:%u\n", KLGameCenter_pLoadObj->nScore);
					
					for( pScore in scores )
					{
						// 既に自分のIDが存在
						if( [[pScore playerID] isEqualToString:leaderboardRequest.localPlayerScore.playerID] )
						{
							findmypos = m;
							break;
						}
						
						m++;
					}
					
					// 自分がいない場合は自分を挿入する
					if( findmypos==-1 )
					{
						insertMe = 1;
						
						// 終端に追加の必要あり
						if( KLGameCenter_pLoadObj->localPlayerScore.rank == KLGameCenter_pLoadObj->nScore+1 )
						{
							KLGameCenter_pLoadObj->nScore++;
						}
					}
					
					if( KLGameCenter_pLoadObj->nScore )
					{
						NSMutableArray*		mary	= [NSMutableArray array];
						//bl foundlp = 0;
						
						KLGameCenter_pLoadObj->pScore = (KLLeaderboardScore*)Calloc(sizeof(KLLeaderboardScore)*KLGameCenter_pLoadObj->nScore);
						
						for( int i=0,s=0; i<KLGameCenter_pLoadObj->nScore; ++i )
						{
							if( insertMe && (i==findmypos||(findmypos<0&&i==KLGameCenter_pLoadObj->nScore-1)) )
							{
							
								pScore = leaderboardRequest.localPlayerScore;
								KLLog("[ KLGameCenter ] insert current score to %u.\n",i+1);
								
							}
							else
							{
								pScore = [scores objectAtIndex:s++];
							}
							
							KLGameCenter_pLoadObj->pScore[i].rank	= pScore.rank;
							KLGameCenter_pLoadObj->pScore[i].score	= pScore.value;
							
							KLLog("[ KLGameCenter ] Rank[%u] %llu\nPID:%s\n", pScore.rank, pScore.value, [pScore.playerID UTF8String]);
							
							[mary addObject:pScore.playerID];
							
						}
						
						KLLog("[ KLGameCenter ] Request players info.\n");
						
						// Player名表示のため再通信
						KLGameCenter_isGettingPlayer	= TRUE;
						[GKPlayer	loadPlayersForIdentifiers:mary
									  withCompletionHandler: ^ (NSArray* players, NSError *error)
						{
							 
							if( error != nil )
							{
								
								KLGameCenter_isGettingScore = FALSE;
								KLGameCenter_isGettingPlayer= FALSE;
								KLLog("[ KLGameCenter ] received player error\n%s\n", [[error localizedDescription] UTF8String]);
								getRankingCallback( FALSE, error.code, KLGameCenter_pGetRankingContext );
								return;
								
							}
							else if( players != nil )
							{
								int i = 0;

								KLLog("[ KLGameCenter ] received player num:%u\n", [players count]);
								
								for( GKPlayer* player in players )
								{
									const char* pAlias	= [player.alias UTF8String];
									const char* pPid	= [player.playerID UTF8String];
									
									u32			len = strlen(pAlias);
									//u32			alen= sizeof(KLGameCenter_pLoadObj->pScore[i].alias)-1;
									u32			pidlen= strlen(pPid);
									//bl			findpl = 0;
									
									if( 1 )
									{
										// 名前の長さを切り詰め
										if( len>=KLGAMECENTER_ALIAS_MAXLEN )
										{
											KLLog("[ KLGameCenter ] over alias length %d.\n", len);
											memcpy( KLGameCenter_pLoadObj->pScore[i].alias, pAlias, sizeof(KLGameCenter_pLoadObj->pScore[i].alias)-4 );
											KLGameCenter_pLoadObj->pScore[i].alias[KLGAMECENTER_ALIAS_MAXLEN-1]	= 0;
											KLGameCenter_pLoadObj->pScore[i].alias[KLGAMECENTER_ALIAS_MAXLEN-2]	= 0;
											KLGameCenter_pLoadObj->pScore[i].alias[KLGAMECENTER_ALIAS_MAXLEN-3]	= 0;
											KLGameCenter_pLoadObj->pScore[i].alias[KLGAMECENTER_ALIAS_MAXLEN-4]	= 0;
										}
										else
										{
											strcpy( KLGameCenter_pLoadObj->pScore[i].alias, pAlias );
											KLLog("[ KLGameCenter ] Alias:%s\n", pAlias);
										}
										
										// PlayerID長を切り詰め
										if( pidlen>=KLGAMECENTER_PID_MAXLEN )
										{
											KLLog("[ KLGameCenter ] !!!!! over playerID buf length %d.\n", pidlen);
											memcpy( KLGameCenter_pLoadObj->pScore[i].pid, pPid, KLGAMECENTER_PID_MAXLEN-1 );
											KLGameCenter_pLoadObj->pScore[i].pid[KLGAMECENTER_PID_MAXLEN-1]	= 0;
											KLGameCenter_pLoadObj->pScore[i].pid[KLGAMECENTER_PID_MAXLEN-2]	= 0;
											KLGameCenter_pLoadObj->pScore[i].pid[KLGAMECENTER_PID_MAXLEN-3]	= 0;
											KLGameCenter_pLoadObj->pScore[i].pid[KLGAMECENTER_PID_MAXLEN-4]	= 0;
										}
										else
										{
											strcpy( KLGameCenter_pLoadObj->pScore[i].pid, pPid );
											KLLog("[ KLGameCenter ] PID:%s\n", pPid);
										}
										
										
										// 仮想スコアがあれば挿入する
										if( KLGameCenter_nVirtualScore )
										{
											int64_t min = KLGameCenter_pVirtualScore[0].score;
											int64_t max = KLGameCenter_pVirtualScore[KLGameCenter_nVirtualScore-1].score;
											
											// 昇順
											if( KLGameCenter_isAscending )
											{
												// トップに挿入
												if( KLGameCenter_pLoadObj->pScore[i].score<=min )
												{
													memmove( &KLGameCenter_pVirtualScore[1], &KLGameCenter_pVirtualScore[0], sizeof(KLLeaderboardScore)*(KLGameCenter_nVirtualScore-1) );
													memcpy( &KLGameCenter_pVirtualScore[0], &KLGameCenter_pLoadObj->pScore[i], sizeof(KLLeaderboardScore) );
												}
												// 最後にも入らない
												else if( KLGameCenter_pLoadObj->pScore[i].score>=max )
												{
													
												}
												// どこかに入る
												else
												{
													for( int r=0; r<KLGameCenter_nVirtualScore; ++r )
													{
														
														if( KLGameCenter_pLoadObj->pScore[i].score <= KLGameCenter_pVirtualScore[r].score )
														{
															// 最後尾
															if( r == KLGameCenter_nVirtualScore-1 )
															{
																memcpy( &KLGameCenter_pVirtualScore[r], &KLGameCenter_pLoadObj->pScore[i], sizeof(KLLeaderboardScore) );
															}
															else
															{
																memmove( &KLGameCenter_pVirtualScore[r+1], &KLGameCenter_pVirtualScore[r], sizeof(KLLeaderboardScore)*(KLGameCenter_nVirtualScore-r-1) );
																memcpy( &KLGameCenter_pVirtualScore[r], &KLGameCenter_pLoadObj->pScore[i], sizeof(KLLeaderboardScore) );
															}
															break;
														}
														
													}
													
												}
											
											}
											// 降順
											else
											{
												// トップに挿入
												if( KLGameCenter_pLoadObj->pScore[i].score>=min )
												{
													memmove( &KLGameCenter_pVirtualScore[1], &KLGameCenter_pVirtualScore[0], sizeof(KLLeaderboardScore)*(KLGameCenter_nVirtualScore-1) );
													memcpy( &KLGameCenter_pVirtualScore[0], &KLGameCenter_pLoadObj->pScore[i], sizeof(KLLeaderboardScore) );
												}
												// 最後にも入らない
												else if( KLGameCenter_pLoadObj->pScore[i].score<=max )
												{
													
												}
												// どこかに入る
												else
												{
													for( int r=0; r<KLGameCenter_nVirtualScore; ++r )
													{
														
														if( KLGameCenter_pLoadObj->pScore[i].score >= KLGameCenter_pVirtualScore[r].score )
														{
															// 最後尾
															if( r == KLGameCenter_nVirtualScore-1 )
															{
																memcpy( &KLGameCenter_pVirtualScore[r], &KLGameCenter_pLoadObj->pScore[i], sizeof(KLLeaderboardScore) );
															}
															else
															{
																memmove( &KLGameCenter_pVirtualScore[r+1], &KLGameCenter_pVirtualScore[r], sizeof(KLLeaderboardScore)*(KLGameCenter_nVirtualScore-r-1) );
																memcpy( &KLGameCenter_pVirtualScore[r], &KLGameCenter_pLoadObj->pScore[i], sizeof(KLLeaderboardScore) );
															}
															break;
														}
														
													}
													
												}
												
											}
											
											KLGameCenter_pLoadObj->nScore = KLGameCenter_nVirtualScore;
											
											// 仮想スコアを返すようコピー
											KLGameCenter_pLoadObj->pScore = (KLLeaderboardScore*)realloc(KLGameCenter_pLoadObj->pScore, sizeof(KLLeaderboardScore)*KLGameCenter_pLoadObj->nScore);
											memcpy( KLGameCenter_pLoadObj->pScore, KLGameCenter_pVirtualScore, sizeof(KLLeaderboardScore)*KLGameCenter_nVirtualScore );
											
										}
									
									}
									
									++i;
									
								}
								
								int n = KLGameCenter_pLoadObj->nScore, find = 0;
								for( i=0; i<n; ++i )
								{
									KLLog("[ KLGameCenter ] %u: %s\n\t\t%llu(%s)\n", i+1, KLGameCenter_pLoadObj->pScore[i].alias, KLGameCenter_pLoadObj->pScore[i].score, KLGameCenter_pLoadObj->pScore[i].pid );
									if( !strcmp(KLGameCenter_pLoadObj->pScore[i].pid, klib.gamecenter.pid) )
									{
										KLLog("[ KLGameCenter ] Your rank:%u(fixed).\n", i+1);
										KLGameCenter_pLoadObj->localPlayerScore.rank = i+1;
										find = 1;
										break;
									}
								}
								
								if( !find )
								{
									KLLog("[ KLGameCenter ] Your rank:%u(fixed&outrank).\n", n+1);
									KLGameCenter_pLoadObj->localPlayerScore.rank = n+1;
								}
								
								KLGameCenter_isGettingScore = FALSE;
								KLGameCenter_isGettingPlayer= FALSE;
								getRankingCallback( TRUE, 0, KLGameCenter_pGetRankingContext );
								return;
								
							}
							else
							{
								KLGameCenter_isGettingScore = FALSE;
								KLGameCenter_isGettingPlayer= FALSE;
								getRankingCallback( FALSE, KLGAMECENTER_ERR_NOPLAYER, KLGameCenter_pGetRankingContext );
							}
							 
						 }];
						
					}
					else
					{
						KLGameCenter_isGettingScore = FALSE;
						getRankingCallback( FALSE, KLGAMECENTER_ERR_NOSCORE, KLGameCenter_pGetRankingContext );
						[leaderboardRequest release];
						return;
					}
					
				}
				else
				{
					KLGameCenter_isGettingScore = FALSE;
					getRankingCallback( FALSE, KLGAMECENTER_ERR_NULLSCORE, KLGameCenter_pGetRankingContext );
					[leaderboardRequest release];
					return;
				}
				
				[leaderboardRequest release];
				
			}
			];
		}
		
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

void loadPlayerData(char** ppPlayerId, u32 count)
{
	NSMutableArray*		mary	= [NSMutableArray array];
	NSString*			nstr	= nil;
	
	for( int i=0; i<count; ++i )
	{
		nstr = [NSString stringWithUTF8String:ppPlayerId[i]];
		[mary addObject:nstr];
	}
	
	[GKPlayer	loadPlayersForIdentifiers:mary
				withCompletionHandler: ^ (NSArray* players, NSError *error)
	{
		if( error != nil )
		{
            // エラーを処理する。
		}
		if( players != nil )
		{
            // GKPlayerオブジェクトの配列を処理する
		}
	}];
}

@implementation KLGameCenterDelegate

@end
