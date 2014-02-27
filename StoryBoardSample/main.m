//
//  main.m
//  StoryBoardSample
//

#import <UIKit/UIKit.h>

#import "AppDelegate.h"

int main(int argc, char * argv[]) {
	
#ifdef DEBUG
	
	int ret;
	
	@autoreleasepool {
		
		@try {
			
			ret = UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
			
		}
		
		@catch (NSException *exception) {
			
			NSLog( @"%@", [exception callStackSymbols] );
			@throw exception;
			
		}
		
	}
	
	return ret;
	
#else
	
	@autoreleasepool {
		
		return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
		
	}
	
#endif

}
