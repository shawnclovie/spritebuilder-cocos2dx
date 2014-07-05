SpriteBuilder for cocos2d-x
======================

Supported cocos2d-x version is 3.0 or above, contribution on 3.1rc0 and Xcode.

**This is alpha version, don't use for production.**

Step by Step
---

Add SBReader into your project (Opened C++ 11).  
Code of read and display .ccbi file:

	#include "lib/SBReader/CCBCreator.h"
	USING_NS_CC;
	...
	auto ccbNode = spritebuilder::Reader::load("sb/MainScene.ccbi");
	auto scene = Scene::create();
	auto layer = Layer::create();
	// Auto add the node into layer.
	// Or pass none and get the node, call layer->addChild(node) manual.
	ccbNode.toNode(layer);
	scene->addChild(layer);
	Director::getInstance()->runWithScene(scene);
