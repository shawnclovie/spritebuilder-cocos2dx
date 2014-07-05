//
//  MemberVariableAssigner.h
//  2048
//
//  Created by Shawn Clovie on 6/28/14.
//
//

#ifndef _MemberVariableAssigner_h
#define _MemberVariableAssigner_h

#include "CCBEnum.h"

NS_CCB_BEGIN
class MemberVariableAssigner {
public:
	virtual bool onAssign(const std::string &name, cocos2d::Node *node) = 0;
};
NS_CCB_END

#endif
