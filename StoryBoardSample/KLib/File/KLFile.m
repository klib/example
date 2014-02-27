//
//  KLFile.m
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

#include "KLib.h"
#include "KLFile.h"

//======================================================================
/*
 *	[ファイルシステムについてのメモ]
 *
 *	アプリのバージョンアップ時にファイル継承が保証されるのは
 *	Documents下とPreferences下のみ
 *	設計には要注意
 */
//======================================================================

//======================================================================
/**
 *	@brief	ファイルを開く
 *
 *	@param	pPath		アプリルート以下のファイルパス
 *	@param	pSize		開いたファイルのサイズを知りたい時は返却先のポインタ / 不要ならNULL
 *
 *	@return	ファイルポインタ / 失敗時はNULL
 */
//======================================================================
FILE* KLFile_Open( path8 pPath, size_t* pSize ){

	char		path[256];
	FILE*		fp		= NULL;
	
	sprintf( path, "%s/%s", klib.path.appRoot, pPath );
	fp = fopen( path, "rb" );
	
	if( fp )
	{
		if( pSize )
		{
			*pSize = 0;
			
			fseek( fp, 0, SEEK_END );
			*pSize = ftell( fp );
			fseek( fp, 0, SEEK_SET );
		}
		
	}
	else
	{
		
		KLLog("[KLFile_Open] Couldn't open file '(%s)\n", pPath);
		
	}
	
	return fp;
	
}

//======================================================================
/**
 *	@brief	ファイルを読み込みバイナリ配列で取得する<br/>
 *			内部でバイナリ配列をmallocするので、呼び出し元で解放が必要
 *
 *	@param	pPath		アプリルート以下のファイルパス
 *	@param	pSize		開いたファイルのサイズを知りたい時は返却先のポインタ / 不要ならNULL
 *	@param	szOverAlloc	余分に確保するサイズ<br/>
 *						テキストファイルを読み込む時などは終端NULL用に1を指定する <br/>
 *						特に必要がなければ0
 *
 *	@return	正常終了したらバイナリの先頭ポインタ / 失敗時はFALSE
 */
//======================================================================
bin8* KLFile_GetBinary( path8 pPath, size_t* pSize, size_t szOverAlloc ){
	
	size_t	sz	= 0;
	FILE*	fp	= KLFile_Open( pPath, &sz );
	bin8*	pRet= NULL;
	
	if( fp )
	{
		pRet = (bin8*)Calloc(sz+szOverAlloc);
		
		if( pRet )
		{
			
			if( pSize )
			{
				*pSize = sz;
			}
			
			fread( pRet, sz, 1, fp );
			
		}
		
		KLFile_Close(fp);
	}
	
	return pRet;
}

//======================================================================
/**
 *	@brief	ファイルを閉じる
 *
 *	@param	fp	ファイルポインタ
 */
//======================================================================
void KLFile_Close( FILE* fp ){
	
	fclose(fp);
	
}


//======================================================================
/**
 *	@brief	ファイル存在確認処理
 *
 *	@param	pFileName		ファイル名
 *	@param	pDirType		フォルダタイプ(KL_FILEPATH_TYPE_****)
 *
 *	@return	ファイルが存在する場合はTRUE / 存在しない場合はFALSE
 */
//======================================================================
static bl KLFile_IsExistsCore( const char* pFileName, const char* pFileType )
{
	NSString*		pName	= [NSString stringWithCString:pFileName encoding:NSUTF8StringEncoding];
	NSString*		pHomeDir= NSHomeDirectory();
	NSString*		pType	= [NSString stringWithCString:pFileType encoding:NSUTF8StringEncoding];
	NSString*		pDocDir	= [pHomeDir	stringByAppendingPathComponent:pType];
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	BOOL				isDir	= FALSE;
	[pFileMgr changeCurrentDirectoryPath:pDocDir];
	return [pFileMgr fileExistsAtPath:pName isDirectory:&isDir]==YES;
}


//======================================================================
/**
 *	@brief	ファイル読み込み処理
 *
 *	@param	pFileName		ファイル名
 *	@param	pDirType		フォルダタイプ(KL_FILEPATH_TYPE_****)
 *	@param	pSize			読み込んだファイルのサイズを返す先のポインタ 不要ならNULL
 *	@param	pFileModDateStr	読み込んだファイルの最終変更日を文字列で返す先のポインタ 不要ならNULL
 *
 *	@return	正常処理した場合は読み込んだファイルのバイナリの先頭ポインタ / 失敗時はNULL
 */
//======================================================================
static bin8* KLFile_ReadCore( const char* pFileName, const char* pDirType, size_t* pSize, char* pFileModDateStr ){
	
	NSString*		pName	= NULL;//[NSString stringWithCString:pFileName encoding:NSUTF8StringEncoding];
	NSString*		pHomeDir= NULL;//NSHomeDirectory();
	NSString*		pType	= NULL;//[NSString stringWithCString:pFileType encoding:NSUTF8StringEncoding];
	NSString*		pDocDir	= NULL;//[pHomeDir	stringByAppendingPathComponent:pType];
	NSString*		pPath	= NULL;//[pDocDir	stringByAppendingPathComponent:pName];
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSData*			pData	= NULL;
	size_t			len		= 0;
	bin8*			pRet	= NULL;
	
	if( strcmp(pDirType,KL_FILEPATH_TYPE_TMP)==0 )
	{
		pName	= [NSString stringWithCString:pFileName encoding:NSUTF8StringEncoding];
		pDocDir=NSTemporaryDirectory();
		pPath = [pDocDir stringByAppendingPathComponent:pName];
	}
	else
	{
		pName	= [NSString stringWithCString:pFileName encoding:NSUTF8StringEncoding];
		pHomeDir= NSHomeDirectory();
		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
		pDocDir	= [pHomeDir	stringByAppendingPathComponent:pType];
		pPath	= [pDocDir	stringByAppendingPathComponent:pName];
	}
	
	[pFileMgr changeCurrentDirectoryPath:pDocDir];
	
	if(pFileModDateStr)
	{
		char*			pTmp	= NULL;
		NSDictionary*	pDic 	= [pFileMgr attributesOfFileSystemForPath:pPath error:nil];
		NSString*		pNstr	= [pDic objectForKey:NSFileModificationDate];
		pTmp					= (char*)[pNstr cStringUsingEncoding:NSUTF8StringEncoding];
		memcpy( pFileModDateStr, pTmp, strlen(pTmp) );
	}
	
	if( strcmp(pDirType,KL_FILEPATH_TYPE_TMP) && !KLFile_IsExistsDocuments(pFileName) )
	{
		return NULL;
	}
	else
	{
		NSFileHandle* fp = [NSFileHandle fileHandleForReadingAtPath:pPath];
		[fp seekToFileOffset:0];
		pData = [fp readDataToEndOfFile];
		[fp closeFile];
		len = [pData length];
		if(pSize)
		{
			*pSize = len;
		}
	}
	
	if(len)
	{
		pRet = (bin8*)Malloc(len);
		if(pRet)
		{
			memcpy( pRet, (u8*)[pData bytes], len );
		}
		else
		{
			
		}
	}
	//[pData release];
	return pRet;
	
}


//======================================================================
/**
 *	@brief	ファイル追記処理
 *
 *	@param	pFileName	ファイル名
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *	@param	pBytes		書き込むバイト配列
 *	@param	szWrite		書き込むサイズ
 *
 *	@return	正常処理した場合はTRUE / 失敗時はFALSE
 */
//======================================================================
static bl KLFile_WriteAddCore( const char* pFileName, const char* pDirType, bin8* pBytes, size_t szWrite ){
	
	NSString*		pName	= [NSString stringWithCString:pFileName encoding:NSUTF8StringEncoding];
	NSString*		pHomeDir= NSHomeDirectory();
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString*		pDocDir	= [pHomeDir	stringByAppendingPathComponent:pType];
	NSString*		pPath	= [pDocDir	stringByAppendingPathComponent:pName];
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSData*			pData	= [NSData dataWithBytes:pBytes length:szWrite];
	bl				ret		= TRUE;
	[pFileMgr changeCurrentDirectoryPath:pDocDir];
	
	NSFileHandle* fp = [NSFileHandle fileHandleForUpdatingAtPath:pPath];//fileHandleForWritingAtPath:pPath];
	[fp seekToEndOfFile];
	[fp writeData:pData];
	[fp closeFile];
	
	return ret;
	
}

//======================================================================
/**
 *	@brief	ファイル書き込み処理
 *
 *	@param	pFileName	ファイル名
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *	@param	pBytes		書き込むバイト配列
 *	@param	szWrite		書き込むサイズ
 *	@param	szTruncate	書き込むサイズ未満ならそこまで切り詰める<br/>
 *						書き込むサイズより大きい場合はそこまで0埋めで広げる<br/>
 *						切り詰めも広げもしないなら0を指定する
 *
 *	@return	正常処理した場合はTRUE / 失敗時はFALSE
 */
//======================================================================
static bl KLFile_WriteCore( const path8 pFileName, const char* pDirType, bin8* pBytes, size_t szWrite, size_t szTruncate ){
	
	NSString*		pName	= [NSString stringWithCString:pFileName encoding:NSUTF8StringEncoding];
	NSString*		pHomeDir= NSHomeDirectory();
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString*		pDocDir	= [pHomeDir	stringByAppendingPathComponent:pType];
	NSString*		pPath	= [pDocDir	stringByAppendingPathComponent:pName];
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSData*			pData	= [NSData dataWithBytes:pBytes length:szWrite];
	bl			ret		= TRUE;
	[pFileMgr changeCurrentDirectoryPath:pDocDir];
	
	if( !KLFile_IsExistsDocuments(pFileName) )
	{
		ret = [pFileMgr createFileAtPath:pName contents:pData attributes:nil];
	}
	else
	{
		NSFileHandle* fp = [NSFileHandle fileHandleForWritingAtPath:pPath];
		[fp seekToFileOffset:0];
		[fp writeData:pData];
		if(szTruncate)
		{
			[fp truncateFileAtOffset:szTruncate];
		}
		[fp closeFile];
	}
	return ret;
	
}

//======================================================================
/**
 *	@brief	指定パスにディレクトリを作成
 *
 *	@param	pPath		ファイルパス
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *
 *	@return	正常処理した場合はTRUE / 失敗時はFALSE
 */
//======================================================================
static bl KLFile_MakeDirCore(const path8 pPath, const char* pDirType){
	
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSString*		pName	= [NSString stringWithCString:pPath encoding:NSUTF8StringEncoding];
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString* path=[NSHomeDirectory() stringByAppendingPathComponent:pType];
	path=[path stringByAppendingPathComponent:pName];
	
	return [pFileMgr createDirectoryAtPath:path withIntermediateDirectories:YES attributes:nil error:nil];
	
}

//======================================================================
/**
 *	@brief	指定パス内にいくつファイルがあるかを返す<br/>
 *			またそれらファイルの各パスを取得する
 *
 *	@param	pPath		ファイルパス
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *	@param	ppEachPath	それぞれのパスの出力先のポインタ 不要ならNULL
 *
 *	@return	指定パスのchar文字列
 */
//======================================================================
static u32 KLFile_GetNumInDirCore(const path8 pPath, const char* pDirType, path8* ppEachPath){
	
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSString*		pName	= [NSString stringWithCString:pPath encoding:NSUTF8StringEncoding];
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString* path=[NSHomeDirectory() stringByAppendingPathComponent:pType];
	path=[path stringByAppendingPathComponent:pName];
	
	NSArray* pAry = [pFileMgr contentsOfDirectoryAtPath:path error:nil];
	u32 c = (u32) [pAry count];
	
	if( ppEachPath )
	{
		for( int i=0; i<c; ++i )
		{
			NSString* nsstr = [pAry objectAtIndex:i];
			ppEachPath[i] = (char*)[nsstr cStringUsingEncoding:NSUTF8StringEncoding];
			//NSNumber* nm = [pDic objectForKey:NSFileSize];
			//pAryEachSize[i] = [nm unsignedIntegerValue];
		}
	}
	return c;
	
}

//======================================================================
/**
 *	@brief	指定フォルダタイプのパスをchar文字列で取得する
 *
 *	@param	pPath		ファイルパス
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *
 *	@return	指定パスのchar文字列
 */
//======================================================================
const path8 KLFile_GetPath(const path8 pPath, const char* pDirType){
	
	NSString*		pName	= [NSString stringWithCString:pPath encoding:NSUTF8StringEncoding];
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString* path=[NSHomeDirectory() stringByAppendingPathComponent:pType];
	path=[path stringByAppendingPathComponent:pName];
	return [path cStringUsingEncoding:NSUTF8StringEncoding];
	
}

//======================================================================
/**
 *	@brief	指定ディレクトリにあるファイルサイズを取得
 *
 *	@param	pPath		ファイルパス
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *
 *	@return	ファイルサイズ
 */
//======================================================================
static size_t KLFile_GetSizeInDirCore(const path8 pPath, const char* pDirType){
	
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSString*		pName	= [NSString stringWithCString:pPath encoding:NSUTF8StringEncoding];
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString* path=[NSHomeDirectory() stringByAppendingPathComponent:pType];
	path=[path stringByAppendingPathComponent:pName];
	
	NSDictionary*	pDic 	= [pFileMgr attributesOfFileSystemForPath:path error:nil];
	NSNumber* ret = [pDic objectForKey:NSFileSize];
	return (size_t)ret;
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内の指定ディレクトリにあるディレクトリ数とパスリストを返す
 *
 *	@param	pPath		調べるディレクトリパス
 *	@param	ppEachPath	検出したパスのリストを取得する場合はその出力先ポインタ 不要ならNULL
 *
 *	@return	ファイル数
 */
//======================================================================
u32 KLFile_GetNumDirDocuments(const path8 pPath, path8* ppEachPath){
	
	if(!KLFile_IsExistsDocuments(pPath))
	{
		return 0;
	}
	return KLFile_GetNumInDirCore( pPath, KL_FILEPATH_TYPE_DOCUMENTS, ppEachPath );
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内の指定ディレクトリにあるファイルサイズを取得
 *
 *	@param	pPath	ディレクトリパス
 *
 *	@return	ファイルサイズ
 */
//======================================================================
size_t KLFile_GetSizeDirDocuments(const path8 pPath){
	
	if(!KLFile_IsExistsDocuments(pPath))
	{
		return 0;
	}
	return KLFile_GetSizeInDirCore( pPath, KL_FILEPATH_TYPE_DOCUMENTS );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内に指定名でディレクトリを作成
 *
 *	@param	pPath	ディレクトリパス
 *
 *	@return	正常に処理した場合はTRUE・失敗時はFALSE
 */
//======================================================================
bl KLFile_MakeDirDocuments(const path8 pPath){
	
	if(KLFile_IsExistsDocuments(pPath))
	{
		return FALSE;
	}
	return KLFile_MakeDirCore( pPath, KL_FILEPATH_TYPE_DOCUMENTS );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内に指定名ファイルが存在するかを確認する
 *
 *	@param	pFileName		対象ファイル名
 *
 *	@return	存在する場合はTRUE・しない場合はFALSE
 */
//======================================================================
inline bl KLFile_IsExistsDocuments(const path8 pFileName){
	
	return KLFile_IsExistsCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内に指定名の空ファイルを作成する
 *
 *	@param	pFileName		対象ファイル名
 *
 *	@return	正常に処理した場合はTRUE・失敗時はFALSE
 */
//======================================================================
inline bl KLFile_MakeDocuments( const path8 pFileName ){
	
	return KLFile_WriteCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS, "", 0, 0 );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内の指定ファイルに追記する
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pBytes			書き込むデータの先頭をcharポインタで渡す
 *	@param	szWrite			書き込むサイズ
 *	@param	szTruncate		通常0を指定する<br/>
 *							サイズを切り詰めたり、書き込み後このサイズまで0で埋めたい時はそのサイズを指定する
 *
 *	@return	正常に処理した場合はTRUE・失敗時はFALSE
 */
//======================================================================
bl KLFile_WriteDocuments( const path8 pFileName, void* pBytes, size_t szWrite, size_t szTruncate ){
	
	return KLFile_WriteCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS, pBytes, szWrite, szTruncate );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内の指定ファイルに追記する
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pBytes			書き込むデータの先頭をcharポインタで渡す
 *
 *	@return	正常に処理した場合はTRUE・失敗時はFALSE
 */
//======================================================================
bl KLFile_WriteAddDocuments( const path8 pFileName, void* pBytes, size_t szWrite ){
	
	return KLFile_WriteAddCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS, (bin8*)pBytes, szWrite );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内に指定ファイルを書き込む<br/>
 *			その際、ファイルの先頭4byteにファイル整合用ハッシュを追加する
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pBytes			書き込むデータの先頭をcharポインタで渡す
 *
 *	@return	正常に処理した場合はTRUE・失敗時はFALSE
 */
//======================================================================
bl KLFile_WriteDocumentsWithHash( const path8 pFileName, void* pBytes, size_t szWrite ){
	
	u32 hash = KLMath_GetHash32(pBytes,szWrite);
	KLFile_WriteDocuments(pFileName,&hash,4,4);
	return KLFile_WriteAddDocuments(pFileName,pBytes,szWrite);
	
}

//======================================================================
/**
 *	@brief	Documentsフォルダ内の指定ファイルをchar配列に読み込んで返す<br/>
 *			また、ファイル編集日付も文字列として受け取る
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pSize			ファイルサイズを返す先になるsize_t型のポインタ(サイズ情報が不要ならNULL)
 *	@param	pModDateStr		ファイル編集日を受け取る場合は文字列のポインタ(不要ならNULL)
 *
 *	@return	読み込んだバイナリ配列 ・ 失敗時はNULL
 */
//======================================================================
bin8* KLFile_ReadDocumentsModDate( const path8 pFileName, size_t* pSize, char* pModDateStr ){
	
	return KLFile_ReadCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS, pSize, pModDateStr );
	
}

//======================================================================
/**
 *	@brief	Documentsフォルダ内の指定ファイルをchar配列に読み込んで返す
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pSize			ファイルサイズを返す先になるsize_t型のポインタ(サイズ情報が不要ならNULL)
 *
 *	@return	読み込んだバイナリ配列 ・ 失敗時はNULL
 */
//======================================================================
bin8* KLFile_ReadDocuments( const path8 pFileName, size_t* pSize ){
	
	return KLFile_ReadCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS, pSize, NULL );
	
}

//======================================================================
/**
 *	@brief	tmpフォルダ内の指定ファイルをchar配列に読み込んで返す
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pSize			ファイルサイズを返す先になるsize_t型のポインタ(サイズ情報が不要ならNULL)
 *
 *	@return	読み込んだバイナリ配列 ・ 失敗時はNULL
 */
//======================================================================
bin8* KLFile_ReadTmp( const path8 pFileName, size_t* pSize ){
	
	return KLFile_ReadCore( pFileName, KL_FILEPATH_TYPE_TMP, pSize, NULL );
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内の指定ファイルをchar配列に読み込む<br/>
 *			その際、ファイルの先頭4byteがファイル整合用ハッシュという前提で読み込み後に照合する
 *
 *	@param	pFileName		対象ファイル名
 *	@param	pSize			ファイルサイズを返す先になるsize_t型のポインタ(サイズ情報が不要ならNULL)
 *	@param	ppOut			読み込んだファイルを格納するためのchar配列のポインタ
 *
 *	@return	正常に処理した場合はTRUE・存在しない、ハッシュが異なる時はFALSE
 */
//======================================================================
bl KLFile_ReadDocumentsWithHash( const char* pFileName, size_t* pSize, char** ppOut ){
	
	char* pOut = NULL;
	size_t sz	= 0;
	
	bin8* pData	= KLFile_ReadDocuments(pFileName,&sz);
	
	if( !pData )
	{
		KLLog("!!! Read file err (read documents failed). !!!\n%s\n\n", pFileName);
		return FALSE;
	}
	else
	{
		u32	hash	= ((u32*)pData)[0];
		u32	hash2	= KLMath_GetHash32(pData+4,sz-4);
		
		sz		-= 4;
		
		if ( !pOut )
		{
			
			pOut = Malloc(sz);
			if( !pOut )
			{
				KLLog("!!! KLFile_ReadDocumentsWithHash Malloc error. !!!\n%s(%d / %d)\n\n", pFileName, hash, hash2);
				return FALSE;
			}
			
		}
		
		memcpy( pOut, pData+4, sz );
		sfree(pData);
		
		if( hash!=hash2 )
		{
			KLLog("!!! Strange hash detected. !!!\n%s(%d / %d)\n\n", pFileName, hash, hash2);
			return FALSE;
		}
		
		*ppOut = pOut;
		
		
	}
	
	if( pSize )
	{
		*pSize = sz; // ハッシュ分減らす
	}
	
	return TRUE;
	
}


//======================================================================
/**
 *	@brief	フォルダ内の指定ファイルを削除する
 *
 *	@param	pPath		削除するファイルパス
 *	@param	pDirType	フォルダタイプ(KL_FILEPATH_TYPE_****)
 *
 *	@return	削除した場合はTRUE・存在しない等削除できなかった時はFALSE
 */
//======================================================================
static bl KLFile_DeleteCore(const path8 pPath, const char* pDirType){
	
	NSFileManager*	pFileMgr= [NSFileManager defaultManager];
	NSString*		pName	= [NSString stringWithCString:pPath encoding:NSUTF8StringEncoding];
	NSString*		pType	= [NSString stringWithCString:pDirType encoding:NSUTF8StringEncoding];
	NSString*		path	= [NSHomeDirectory() stringByAppendingPathComponent:pType];
	path=[path stringByAppendingPathComponent:pName];
	
	return [pFileMgr removeItemAtPath:path error:nil];
	
}

//======================================================================
/**
 *	@brief	ドキュメントフォルダ内の指定ファイルを削除する
 *
 *	@param	pFileName	削除するファイルパス
 *
 *	@return	削除した場合はTRUE・存在しない等削除できなかった時はFALSE
 */
//======================================================================
bl KLFile_DeleteDocuments( const path8 pFileName ){
	
	return KLFile_DeleteCore( pFileName, KL_FILEPATH_TYPE_DOCUMENTS );
	
}

//======================================================================
/**
 *	@brief	charをURLRefに変換
 *
 *	@param	pPath	変換するchar文字列
 *
 *	@return	CFURLRef
 */
//======================================================================
CFURLRef KLFile_CharToURLRef( path8 pPath ){
	
	NSString*	nsPath	= [NSString stringWithCString:pPath encoding:NSUTF8StringEncoding];
	NSURL*		url;
	url = [NSURL fileURLWithPath:nsPath];
#if __has_feature(objc_arc)
	// ! 要呼び出し元でCFRelease
	return (__bridge_retained CFURLRef) url;
#else
	return (CFURLRef) url;
#endif
}

