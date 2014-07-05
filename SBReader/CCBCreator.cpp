//
//  CCBCreator.cpp
//
//  Created by Shawn Clovie on 7/2/14.
//
//

#include "CCBCreator.h"

USING_NS_CC;

NS_CCB_BEGIN
void Creator::add(CCBNode::Type type, Handler handler){
	if (type != CCBNode::Unknow && handler != nullptr) {
		handlers[type] = handler;
	}
}

Creator::Handler Creator::getHandler(CCBNode::Type type){
	auto it = handlers.find(type);
	return it == handlers.end()? nullptr : it->second;
}

cocos2d::Node * Creator::create(CCBNode &node, cocos2d::Node *parent){
	auto handler = getHandler(node.type);
	if(handler == nullptr){
		return nullptr;
	}
	auto r = handler(node, nullptr, parent);
	if (parent) {
		parent->addChild(r);
	}
	if (!node.children.empty()) {
		for (auto nodeSub : node.children) {
			create(nodeSub, r);
		}
	}
	return r;
}

bool Creator::init(){
	add(CCBNode::Type::Node, [this](CCBNode &ccb, Node *node, Node *parent){
		if (!node) {
			node = Node::create();
		}
		auto sizeParent = parent? parent->getContentSize() : Director::getInstance()->getWinSize();
		auto posType = ccb.getValuePositionType("positionType");
		auto point = ccb.getValuePoint("position");
		if (parent) {
			if(posType.xUnit == Position::Unit::NORMALLIZED){
				point.x *= sizeParent.width;
			}
			if (posType.yUnit == Position::Unit::NORMALLIZED) {
				point.y *= sizeParent.height;
			}
		}
		node->setPosition(point);
		if (ccb.getValue("contentSize")) {
			auto sizeType = ccb.getValueSizeType("contentSizeType");
			auto size = ccb.getValueSize("contentSize");
			if (parent) {
				if (sizeType.widthUnit == Size::Unit::NORMALIZED) {
					size.width *= sizeParent.width;
				}
				if (sizeType.heightUnit == Size::Unit::NORMALIZED) {
					size.height *= sizeParent.height;
				}
			}
			node->setContentSize(size);
		}
		if (ccb.getValue("anchorPoint")) {
			node->setAnchorPoint(ccb.getValuePoint("anchorPoint"));
		}
		if (ccb.getValue("scale")) {
			auto sizeType = ccb.getValueSizeType("scaleType");
			auto size = ccb.getValueSize("scale");
			node->setScale(size.width, size.height);
		}
		if (ccb.getValue("color")) {
			node->setColor(ccb.getValueColor3B("color"));
		}
		if (ccb.getValue("opacity")) {
			node->setOpacity(ccb.getValueNumber("opacity") * 255);
		}
		if (!ccb.rootSequences.empty()) {
			// TODO create animation manager.
			
		}
		return node;
	});
	add(CCBNode::Type::LabelTTF, [this](CCBNode &ccb, Node *node, Node *parent){
		auto label = node? dynamic_cast<Label *>(node) : Label::createWithSystemFont("", "", 0);
		if (label) {
			handlers[CCBNode::Type::Node](ccb, label, parent);
			label->setString(ccb.getValueString("string"));
			label->setSystemFontName(ccb.getValueString("fontName"));
			label->setSystemFontSize(ccb.getValueNumber("fontSize"));
		}
		return label;
	});
	add(CCBNode::Type::LabelBMFont, [this](CCBNode &ccb, Node *node, Node *parent){
		auto label = node? dynamic_cast<Label *>(node) : Label::create();
		if (label) {
			handlers[CCBNode::Type::Node](ccb, label, parent);
			label->setString(ccb.getValueString("string"));
			auto fnt = ccb.getValueString("fntFile");
			// Maybe fntFile did formated "path/to/file.fnt/file.fnt"
			auto pos = fnt.rfind(".fnt/");
			if (pos != std::string::npos) {
				fnt = fnt.substr(0, pos + 4);
			}
			label->setBMFontFilePath(fnt);
			auto size = ccb.getValueSize("dimensions");
			label->setDimensions(size.width, size.height);
		}
		return label;
	});
	add(CCBNode::Type::Sprite, [this](CCBNode &ccb, Node *node, Node *parent){
		auto sp = node? dynamic_cast<Sprite *>(node) : Sprite::create();
		if (sp) {
			auto FU = FileUtils::getInstance();
			auto frame = FU->fullPathForFilename(ccb.getValueString("spriteFrame"));
			if (FU->isFileExist(frame)) {
				sp->setTexture(frame);
			}
			handlers[CCBNode::Type::Node](ccb, sp, parent);
		}
		return sp;
	});
	add(CCBNode::Type::Layer, [this](CCBNode &ccb, Node *node, Node *parent){
		auto layer = node? dynamic_cast<Layer *>(node) : Layer::create();
		if (layer) {
			
			handlers[CCBNode::Type::Node](ccb, layer, parent);
		}
		return layer;
	});
	add(CCBNode::Type::LayerColor, [this](CCBNode &ccb, Node *node, Node *parent){
		auto layer = node? dynamic_cast<LayerColor *>(node) : LayerColor::create();
		if (layer) {
			handlers[CCBNode::Type::Layer](ccb, layer, parent);
		}
		return layer;
	});
	add(CCBNode::Type::LayerGradient, [this](CCBNode &ccb, Node *node, Node *parent){
		auto layer = node? dynamic_cast<LayerGradient *>(node) : LayerGradient::create();
		if (layer) {
			layer->setVector(ccb.getValueVec2("vector"));
			layer->setStartColor(ccb.getValueColor3B("startColor"));
			layer->setEndColor(ccb.getValueColor3B("endColor"));
			handlers[CCBNode::Type::LayerColor](ccb, layer, parent);
		}
		return layer;
	});
	return true;
}
NS_CCB_END
