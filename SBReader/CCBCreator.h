//
//  CCBCreator.h
//
//  Created by Shawn Clovie on 7/2/14.
//
//

#ifndef __CCBCreator__
#define __CCBCreator__

#include "CCBNode.h"

NS_CCB_BEGIN
class Creator {
public:
	// Handle create node (node is null) or set properties.
	typedef std::function<cocos2d::Node *(CCBNode &, cocos2d::Node *, cocos2d::Node *)> Handler;
	std::map<CCBNode::Type, Handler> handlers;
	
	void add(CCBNode::Type type, Handler handler);
	Handler getHandler(CCBNode::Type type);
	virtual cocos2d::Node * create(CCBNode &node, cocos2d::Node *parent = nullptr);
//	virtual void setProperties(
	
	virtual bool init();
};
NS_CCB_END

#endif /* defined(__CCBCreator__) */
