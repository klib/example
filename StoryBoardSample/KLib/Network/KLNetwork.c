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

#include "KLNetwork.h"
#include <string.h>
#include <SystemConfiguration/SystemConfiguration.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>

// Thanks! (Unknown Lisence)
// http://www.servin.com/iphone/iPhone-Network-Status.html
// http://devcafe.jp/blog/

//===================================================================
/**
 *	@brief	Wifiまたは3Gでネットワークに接続しているかを返す
 *
 *	@return	接続していればTRUE
 */
//===================================================================
bl KLNetwork_IsConnecting(){
	
	struct sockaddr_in			s;
	SCNetworkReachabilityRef	r = NULL;
	klmemclear(&s, s);
	
	s.sin_family	= AF_INET;
	s.sin_len		= sizeof(s);
	
	inet_aton("0.0.0.0", &s.sin_addr);
	
	r = SCNetworkReachabilityCreateWithAddress(NULL, (struct sockaddr *) &s);

	if(r)
	{
		SCNetworkReachabilityFlags f = 0;
		SCNetworkReachabilityGetFlags(r, &f);
		bl isReachable		= ((f & kSCNetworkFlagsReachable) != 0);
		bl needsConnection	= ((f & kSCNetworkFlagsConnectionRequired) != 0);
		if( isReachable && !needsConnection )
		{
			if ( (f & kSCNetworkReachabilityFlagsIsWWAN) != 0 )
			{
				return KLNETWORK_STAT_3G;
			}
			
			if ( KLNetwork_GetWifiAddress() )
			{
				return KLNETWORK_STAT_WIFI;
			}
			
		}
	}
	
	return KLNETWORK_STAT_NOCONNECTION;

}

static char KLNetwork_WifiAddress[KLNETWORK_WIFIADRS_BUFSIZE];
char* KLNetwork_GetWifiAddress(){
	
	struct		 ifaddrs*	pAddr	= NULL;
	const struct ifaddrs*	pCursor	= NULL;
	bl						ret		= getifaddrs(&pAddr) == 0;
	
	if( ret )
	{
		pCursor = pAddr;
		
		while( pCursor )
		{
			if( pCursor->ifa_addr->sa_family == AF_INET && (pCursor->ifa_flags & IFF_LOOPBACK) == 0)
			{
				if( strlen(pCursor->ifa_name) )
				{
					strcpy(KLNetwork_WifiAddress, inet_ntoa(((struct sockaddr_in*)pCursor->ifa_addr)->sin_addr));
					freeifaddrs(pAddr);
					pAddr = NULL;
					return KLNetwork_WifiAddress;
				}
			}
			
			pCursor = pCursor->ifa_next;
		}
		
		freeifaddrs(pAddr);
		pAddr = NULL;
	
	}
	
	return NULL;
	
}