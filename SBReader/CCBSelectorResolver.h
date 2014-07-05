//
//  CCBSelectorResolver.h
//
//  Created by Shawn Clovie on 6/29/14.
//
//

#ifndef _CCBSelectorResolver_h
#define _CCBSelectorResolver_h

#include "CCBEnum.h"

NS_CCB_BEGIN
class SelectorResolver {
public:
	virtual void onResolve(cocos2d::Node *node, const std::string &selectorName){
	}
	virtual cocos2d::CallFuncN * onResolveCallFunc(const std::string &selectorName) {
		return NULL;
	}
};
NS_CCB_END

#endif
