//
//  CCBNode.cpp
//
//  Created by Shawn Clovie on 6/28/14.
//
//

#include "CCBNode.h"
#include "CCBCreator.h"

USING_NS_CC;
USING_NS_CC_EXT;

NS_CCB_BEGIN
CCBNode CCBNode::createWithClassName(const std::string &cls){
	CCBNode node;
	if (cls == "CCNode" || cls == "CCLayoutBox") {
		node.type = Node;
	}else if(cls.compare("CCSprite") == 0){
		node.type = Sprite;
	}else if(cls == "CCScale9Sprite" || cls == "CCSprite9Slice"){
		node.type = Scale9Sprite;
	}else if(cls.compare("CCLabelTTF") == 0){
		node.type = LabelTTF;
	}else if(cls.compare("CCLabelBMFont") == 0){
		node.type = LabelBMFont;
	}else if(cls.compare("CCLayer") == 0){
		node.type = Layer;
	}else if(cls == "CCLayerColor" || cls == "CCNodeColor"){
		node.type = LayerColor;
	}else if(cls == "CCLayerGradient" || cls == "CCNodeGradient"){
		node.type = LayerGradient;
	}else if(cls.compare("CCScrollView") == 0){
		node.type = ScrollView;
	}else if(cls.compare("CCButton") == 0){
		node.type = Button;
	}else if(cls == "CCSlider"){
		node.type = Slider;
	}else if(cls.compare("CCMenu") == 0){
		node.type = Menu;
	}else if(cls.compare("CCMenuItem") == 0){
		node.type = MenuItem;
	}else if(cls.compare("CCBFile") == 0){
		node.type = File;
	}else if(cls == "CCParticleSystemQuid" || cls == "CCParticleSystem"){
		node.type = Particle;
	}else if(cls == "CCTextField"){
		node.type = TextField;
	}
	return std::move(node);
}

CCBNode::CCBNode() : type(Unknow), autoPlaySequenceId(-1){
}
bool CCBNode::hasAnimatedProp(const std::string &name) const {
	return animatedProps.find(name) != animatedProps.end();
}

void CCBNode::setAniValue(const std::string &name, const Value &v){
	aniValues[name] = v;
}

const Value * CCBNode::getValue(const std::string &name) const {
	auto it = values.find(name);
	return it == values.end()? nullptr : &it->second;
}
double CCBNode::getValueNumber(const std::string &name) const {
	auto v = getValue(name);
	return v && v->numbers.size() >= 1? v->number(0) : 0;
}
std::string CCBNode::getValueString(const std::string &name) const {
	auto v = getValue(name);
	return v? v->string : "";
}
cocos2d::Point CCBNode::getValuePoint(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 2;
	return b? Point(v->number(0), v->number(1)) : Point::ZERO;
}
cocos2d::Size CCBNode::getValueSize(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 2;
	return b? cocos2d::Size(v->number(0), v->number(1)) : cocos2d::Size::ZERO;
}
cocos2d::Color3B CCBNode::getValueColor3B(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 3;
	return b? Color3B(v->number(0) * 255, v->number(1) * 255, v->number(2) * 255) : Color3B::BLACK;
}
cocos2d::Color4B CCBNode::getValueColor4B(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 4;
	return b? Color4B(v->number(0) * 255, v->number(1) * 255, v->number(2) * 255, v->number(3) * 255) : Color4B::BLACK;
}
cocos2d::Color4F CCBNode::getValueColor4F(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 4;
	return b? Color4F(v->number(0), v->number(1), v->number(2), v->number(3)) : Color4F::BLACK;
}
cocos2d::BlendFunc CCBNode::getValueBlendFunc(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 2;
	if (b) {
		BlendFunc r = {(unsigned int)v->number(0), (unsigned int)v->number(1)};
		return std::move(r);
	}
	return BlendFunc::DISABLE;
}
Position::Type CCBNode::getValuePositionType(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 3;
	return Position::Type((Position::ReferenceCorner)(b? v->number(0) : 0), (Position::Unit)(b? v->number(1) : 0), (Position::Unit)(b? v->number(2) : 0));
}
Size::Type CCBNode::getValueSizeType(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 2;
	return Size::Type((Size::Unit)(b? v->number(0) : 0), (Size::Unit)(b? v->number(1) : 0));
}
Vec2 CCBNode::getValueVec2(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 2;
	return b? Vec2(v->number(0), v->number(1)) : Vec2::ZERO;
}
Vec3 CCBNode::getValueVec3(const std::string &name) const {
	auto v = getValue(name);
	auto b = v && v->numbers.size() >= 3;
	return b? Vec3(v->number(0), v->number(1), v->number(2)) : Vec3::ZERO;
}
void CCBNode::setValue(const std::string &name, const Value &v){
	values[name] = v;
}
void CCBNode::setValue(const std::string &name, CCBNode &v){
}

cocos2d::Node * CCBNode::toNode(cocos2d::Node *parent){
	Creator creator;
	creator.init();
	return creator.create(*this, parent);
}

std::string CCBNode::toString(){
	const char *clsName;
	switch (type) {
		case Node:			clsName = "Node";	break;
		case LabelBMFont:	clsName = "LabelBMFont";	break;
		case LabelTTF:		clsName = "LabelTTF";	break;
		case Sprite:		clsName = "Sprite";	break;
		case Scale9Sprite:	clsName = "Scale9Sprite";	break;
		case Layer:			clsName = "Layer";	break;
		case LayerColor:	clsName = "LayerColor";	break;
		case LayerGradient:	clsName = "LayerGradient";	break;
		case ScrollView:	clsName = "ScrollView";	break;
		case Button:		clsName = "Button";	break;
		case Slider:		clsName = "Slider";	break;
		case Menu:			clsName = "Menu";	break;
		case MenuItem:		clsName = "MenuItem";	break;
		case File:			clsName = "File";	break;
		case Particle:		clsName = "Particle";	break;
		case TextField:		clsName = "TextField";	break;
		default:			clsName = "?";	break;
	}
	char buf[100];
	sprintf(buf, "%s Props{\n", clsName);
	std::string r(buf);
	for (auto &it : values) {
		if (it.second.empty()) {
			continue;
		}
		r.append("\t\t").append(it.first).append("=").append(it.second.toString()).append("\n");
	}
	r.append("\t} Animation values{\n");
	for (auto &it : values) {
		if (it.second.empty()) {
			continue;
		}
		r.append("\t\t").append(it.first).append("=").append(it.second.toString()).append("\n");
	}
	if (sequences.size() > 0) {
		sprintf(buf, "\t} Sequences(%d){\n", (int)sequences.size());
		r.append(buf);
		for (auto &it : sequences) {
			r.append("\t\t").append(it.second.toString()).append("\n");
		}
	}
	if (children.size() > 0) {
		sprintf(buf, "\t} Children(%d){\n", (int)children.size());
		r.append(buf);
		for (auto &sub : children) {
			r.append(sub.toString()).append("\n");
		}
	}
	r.append("}");
	return std::move(r);
}
NS_CCB_END
