//
//  CCBNode.h
//
//  Created by Shawn Clovie on 6/28/14.
//
//

#ifndef __CCBNode__
#define __CCBNode__

#include "CCBSequence.h"
#include "cocos-ext.h"

NS_CCB_BEGIN
class CCBNode {
public:
	enum Type {
		Unknow, Node,
		LabelTTF, LabelBMFont,
		Sprite, Scale9Sprite,
		Layer, LayerColor, LayerGradient, ScrollView,
		Button, Slider,
		Menu, MenuItem, File,
		Particle,
		TextField,
	};
	Type type;
	// Child CCBNodes.
	std::vector<CCBNode> children;
	// Animated property names.
	std::set<std::string> animatedProps;
	// Record keyframes on every sequence.
	SequenceList sequences;
	// Set only on node is root.
	SequenceList rootSequences;
	// Auto play sequence id, default is -1.
	int autoPlaySequenceId;
	// Animation base values.
	Values aniValues;
	Values values;
public:
	// Create node of every sub class.
	static CCBNode createWithClassName(const std::string &cls);
	
	CCBNode();
	bool hasAnimatedProp(const std::string &name) const;
	
	void setAniValue(const std::string &name, const Value &v);
	
	const Value * getValue(const std::string &name) const;
	double getValueNumber(const std::string &name) const;
	std::string getValueString(const std::string &name) const;
	cocos2d::Point getValuePoint(const std::string &name) const;
	cocos2d::Size getValueSize(const std::string &name) const;
	cocos2d::Color3B getValueColor3B(const std::string &name) const;
	cocos2d::Color4B getValueColor4B(const std::string &name) const;
	cocos2d::Color4F getValueColor4F(const std::string &name) const;
	cocos2d::BlendFunc getValueBlendFunc(const std::string &name) const;
	Position::Type getValuePositionType(const std::string &name) const;
	Size::Type getValueSizeType(const std::string &name) const;
	cocos2d::Vec2 getValueVec2(const std::string &name) const;
	cocos2d::Vec3 getValueVec3(const std::string &name) const;
	
	void setValue(const std::string &name, const Value &v);
	void setValue(const std::string &name, CCBNode &v);
	
	cocos2d::Node * toNode(cocos2d::Node *parent = nullptr);
	
	cocos2d::ui::Widget * toWidget(cocos2d::Node *parent = nullptr);
	
	std::string toString();
};
NS_CCB_END

#endif /* defined(__CCBNode__) */
