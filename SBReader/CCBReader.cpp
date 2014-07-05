/*
 * SpriteBuilder: http://www.spritebuilder.org
 *
 * Copyright (c) 2012 Zynga Inc.
 * Copyright (c) 2013 Apportable Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "CCBReader.h"
#include "CCBLocalizationManager.h"

NS_CCB_BEGIN
#define DIRECTOR_DESIGN_SIZE Director::getInstance()->getOpenGLView()->getDesignResolutionSize()

//void Reader::configureFileUtils(){
//    auto FU = FileUtils::getInstance();
//    
//    // Setup file utils for use with SpriteBuilder
//    FU->setEnableiPhoneResourcesOniPad:NO];
//    
//    FU.directoriesDict =
//    [[NSMutableDictionary alloc] initWithObjectsAndKeys:
//     @"resources-tablet", CCFileUtilsSuffixiPad,
//     @"resources-tablethd", CCFileUtilsSuffixiPadHD,
//     @"resources-phone", CCFileUtilsSuffixiPhone,
//     @"resources-phonehd", CCFileUtilsSuffixiPhoneHD,
//     @"resources-phone", CCFileUtilsSuffixiPhone5,
//     @"resources-phonehd", CCFileUtilsSuffixiPhone5HD,
//     @"", CCFileUtilsSuffixDefault,
//     nil];
//    
//    sharedFileUtils.searchPath =
//    [NSArray arrayWithObjects:
//     [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Published-iOS"],
//     [[NSBundle mainBundle] resourcePath],
//     nil];
//    
//	sharedFileUtils.enableiPhoneResourcesOniPad = YES;
//    sharedFileUtils.searchMode = CCFileUtilsSearchModeDirectory;
//    [sharedFileUtils buildSearchResolutionsOrder];
//    
//    [sharedFileUtils loadFilenameLookupDictionaryFromFile:@"fileLookup.plist"];
//    [[CCSpriteFrameCache sharedSpriteFrameCache] loadSpriteFrameLookupDictionaryFromFile:@"spriteFrameFileList.plist"];
//}
Reader::Reader()
: _bytes(nullptr)
, _currentByte(-1)
, _currentBit(-1)
, _owner(nullptr)
, _assigner(nullptr){
	// Setup action manager
//		_animationManager = AnimationManager::create();
	// Setup resolution scale and default container size
//		_animationManager->rootContainerSize = DIRECTOR_DESIGN_SIZE;
}

Reader::~Reader() {
	_bytes = NULL;
}

unsigned char Reader::readByte(){
	unsigned char byte = _bytes[_currentByte];
	++ _currentByte;
	return byte;
}

bool Reader::readBool(){
	return (bool)readByte();
}

std::string Reader::readUTF8(){
	std::string ret;
	int b0 = readByte();
	int b1 = readByte();
	int numBytes = b0 << 8 | b1;
	
	char* pStr = (char*)malloc(numBytes+1);
	memcpy(pStr, _bytes + _currentByte, numBytes);
	pStr[numBytes] = '\0';
	ret = pStr;
	free(pStr);
	
	_currentByte += numBytes;
	return std::move(ret);
}

void Reader::alignBits() {
	if (_currentBit) {
		_currentBit = 0;
		++ _currentByte;
	}
}

#define REVERSE_BYTE(b) (unsigned char)(((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16)

int Reader::readIntWithSign(bool sign){
	// Good luck groking this!
	// The basic idea is to do as little bit reading as possible and use everything in a byte contexts and avoid loops; espc ones that iterate 8 * bytes-read
	// Note: this implementation is NOT the same encoding concept as the standard Elias Gamma, instead the real encoding is a byte flipped version of it.
	// In order to optimize to little-endian devices, we have chosen to unflip the bytes before transacting upon them (excluding of course the "leading" zeros.
	unsigned int v = *(unsigned int *)(_bytes + _currentByte);
	int numBits = 32;
	int extraByte = 0;
	v &= -((int)v);
	if (v) numBits--;
	if (v & 0x0000FFFF) numBits -= 16;
	if (v & 0x00FF00FF) numBits -= 8;
	if (v & 0x0F0F0F0F) numBits -= 4;
	if (v & 0x33333333) numBits -= 2;
	if (v & 0x55555555) numBits -= 1;
	
	if ((numBits & 0x00000007) == 0){
		extraByte = 1;
		_currentBit = 0;
		_currentByte += (numBits >> 3);
	}else{
		_currentBit = numBits - (numBits >> 3) * 8;
		_currentByte += (numBits >> 3);
	}
	
	static unsigned char prefixMask[] = {
		0xFF, 0x7F, 0x3F, 0x1F,
		0x0F, 0x07, 0x03, 0x01,
	};
	static unsigned int suffixMask[] = {
		0x00, 0x80, 0xC0, 0xE0,
		0xF0, 0xF8, 0xFC, 0xFE,
		0xFF,
	};
	unsigned char prefix = REVERSE_BYTE(*(_bytes + _currentByte)) & prefixMask[_currentBit];
	long long current = prefix;
	int numBytes = 0;
	int suffixBits = (numBits - (8 - _currentBit) + 1);
	if (numBits >= 8){
		suffixBits %= 8;
		numBytes = (numBits - (8 - (int)(_currentBit)) - suffixBits + 1) / 8;
	}
	if (suffixBits >= 0){
		++ _currentByte;
		for (int i = 0; i < numBytes; ++ i){
			current <<= 8;
			unsigned char byte = REVERSE_BYTE(*(_bytes + _currentByte));
			current += byte;
			++ _currentByte;
		}
		current <<= suffixBits;
		unsigned char suffix = (REVERSE_BYTE(*(_bytes + _currentByte)) & suffixMask[suffixBits]) >> (8 - suffixBits);
		current += suffix;
	}else{
		current >>= -suffixBits;
	}
	_currentByte += extraByte;
	int num;
	if (sign){
		int s = current % 2;
		num = current / 2 * (s? 1 : -1);
	}else{
		num = (int)current - 1;
	}
	alignBits();
	return num;
}

float Reader::readFloat(){
	unsigned char type = readByte();
	if (type == CCBFloat::_0){				return 0;}
	else if (type == CCBFloat::_1){		return 1;}
	else if (type == CCBFloat::MINUS1){	return -1;}
	else if (type == CCBFloat::_05){		return 0.5f;}
	else if (type == CCBFloat::INTEGER){
		return readIntWithSign(true);
	}else{
		volatile union {
			float f;
			int i;
		} t;
		t.i = *(int *)(_bytes + _currentByte);
		_currentByte += 4;
		return t.f;
	}
}

std::string Reader::readCachedString(){
	int n = readIntWithSign(false);
	return _stringCache[n];
}

void Reader::readPropertyForNode(CCBNode &node/*, CCBNode &parent*/, bool isExtraProp){
	// Read type and property name
	int type = readIntWithSign(false);
	auto name = readCachedString();
	// Check if the property can be set for this platform
	bool setProp = true;
	// Forward properties for sub ccb files
//		if (node.type == CCBNode::File) {
//			if (isExtraProp && fileNode->getCCBFile()){
//				node = fileNode->getCCBFile();
//				// Skip properties that doesn't have a value to override
//				auto extraPropsNames = dynamic_cast<StringVector *>(node->getUserObject());
//				setProp &= extraPropsNames->contains(name);
//			}
//		}else if (isExtraProp && node == _animationManager->getRootNode()){
//			auto extraPropNames = static_cast<StringVector *>(node->getUserData());
//			if (!extraPropNames){
//				extraPropNames = new StringVector;
//				node->setUserData(extraPropNames);
//			}
//			extraPropNames->pushBack(name);
//		}
	
	if (type == PropType::POSITION){
		float x = readFloat();
		float y = readFloat();
		int corner = readByte();
		int xUnit = readByte();
		int yUnit = readByte();
		if (setProp){
			double vs[] = {x, y, (double)corner, (double)xUnit, (double)yUnit};
			node.setValue(name, Value(vs, 2));
			node.setValue(std::string(name).append("Type"), Value(vs + 2, 3));
			if (node.hasAnimatedProp(name)){
				node.setAniValue(name, Value(vs, 5));
//				_animationManager->setBaseValue(value, node, name);
			}
		}
	}else if(type == PropType::POINT || type == PropType::POINT_LOCK){
		float x = readFloat();
		float y = readFloat();
		if (setProp){
			double vs[] = {x, y};
			node.setValue(name, Value(vs, 2));
		}
	}else if (type == PropType::SIZE){
		float w = readFloat();
		float h = readFloat();
		int xUnit = readByte();
		int yUnit = readByte();
		if (setProp){
			double vs[] = {w, h, (double)xUnit, (double)yUnit};
			node.setValue(name, Value(vs, 2));
			node.setValue(std::string(name).append("Type"), Value(vs + 2, 2));
		}
	}
	else if (type == PropType::SCALE_LOCK){
		float x = readFloat();
		float y = readFloat();
		int sType = readByte();
		if (setProp){
			double vs[] = {x, y};
			node.setValue(name, Value(vs, 2));
			node.setValue(std::string(name).append("Type"), sType);
			if (node.hasAnimatedProp(name)){
				double vs[] = {x, y, (double)sType};
				node.setAniValue(name, Value(vs, 3));
//				_animationManager->setBaseValue(value, node, name);
			}
		}
	}else if (type == PropType::FLOAT_XY){
		float xFloat = readFloat();
		float yFloat = readFloat();
		if (setProp){
			node.setValue(std::string(name).append("X"), xFloat);
			node.setValue(std::string(name).append("Y"), yFloat);
		}
	}else if (type == PropType::DEGREES || type == PropType::FLOAT){
		float f = readFloat();
		if (setProp){
			node.setValue(name, f);
			if (node.hasAnimatedProp(name)){
				node.setAniValue(name, f);
//				_animationManager->setBaseValue(f, node, name);
			}
		}
	}else if (type == PropType::FLOAT_SCALE){
		float f = readFloat();
		int sType = readIntWithSign(false);
		if (setProp){
			if (sType == 1){
//				f *= Director::getInstance()->get.UIScaleFactor;
			}
			node.setValue(name, f);
		}
	}else if (type == PropType::INTEGER || type == PropType::INTEGER_LABELED){
		int d = readIntWithSign(true);
		if (setProp){
			node.setValue(name, d);
		}
	}else if (type == PropType::FLOAT_VAR){
		float f = readFloat();
		float fVar = readFloat();
		if (setProp){
			node.setValue(name, f);
			node.setValue(std::string(name).append("Var"), fVar);
		}
	}else if (type == PropType::CHECK){
		bool b = readBool();
		if (setProp){
			node.setValue(name, b);
			if (node.hasAnimatedProp(name)){
				node.setAniValue(name, b);
//				_animationManager->setBaseValue(b, node, name);
			}
		}
	}else if (type == PropType::SPRITE_FRAME){
		auto spriteFile = readCachedString();
		if (setProp && !spriteFile.empty()){
//			auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFile);
			node.setValue(name, spriteFile);
			if (node.hasAnimatedProp(name)){
				node.setAniValue(name, spriteFile);
//				_animationManager->setBaseValue(frame, node, name);
			}
		}
	}else if (type == PropType::TEXTURE){
		auto spriteFile = readCachedString();
		if (setProp && !spriteFile.empty()){
//			auto TC = Director::getInstance()->getTextureCache();
//			auto tex = TC->getTextureForKey(spriteFile);
			node.setValue(name, spriteFile);
		}
	}else if (type == PropType::BYTE){
		int byte = readByte();
		if (setProp){
			node.setValue(name, byte);
			if (node.hasAnimatedProp(name)){
				node.setAniValue(name, byte);
//				_animationManager->setBaseValue(byte, node, name);
			}
		}
	}else if (type == PropType::COLOR4 || type == PropType::COLOR3){
		float r = readFloat();
		float g = readFloat();
		float b = readFloat();
		float a = readFloat();
		if (setProp){
			double vs[] = {r, g, b, a};
			Value v(vs, 4);
			node.setValue(name, v);
			if (node.hasAnimatedProp(name)){
				node.setAniValue(name, v);
//				_animationManager->setBaseValue(v, node, name);
			}
		}
	}else if (type == PropType::COLOR4F_VAR){
		float r = readFloat();
		float g = readFloat();
		float b = readFloat();
		float a = readFloat();
		float rVar = readFloat();
		float gVar = readFloat();
		float bVar = readFloat();
		float aVar = readFloat();
		if (setProp){
			{
				double vs[] = {r, g, b, a};
				Value v(vs, 4);
				node.setValue(name, v);
			}
			double vs[] = {rVar, gVar, bVar, aVar};
			Value v(vs, 4);
			node.setValue(std::string(name).append("Var"), v);
		}
	}else if (type == PropType::FLIP){
		bool xFlip = readBool();
		bool yFlip = readBool();
		if (setProp){
			node.setValue(std::string(name).append("X"), xFlip);
			node.setValue(std::string(name).append("Y"), yFlip);
		}
	}else if (type == PropType::BLENDMODE){
		int src = readIntWithSign(false);
		int dst = readIntWithSign(false);
		if (setProp){
			double vs[] = {(double)src, (double)dst};
			node.setValue(name, Value(vs, 2));
		}
	}else if (type == PropType::FNT_FILE){
		auto fntFile = readCachedString();
		node.setValue(name, fntFile);
	}else if (type == PropType::TEXT || type == PropType::STRING){
		auto txt = readCachedString();
		bool localized = readBool();
		if (localized){
//			txt = CCBLocalize(txt);
		}
		if (setProp){
			node.setValue(name, txt);
		}
	}else if (type == PropType::FONT_TTF){
		auto fnt = readCachedString();
		if (setProp){
			//if ([[fnt lowercaseString] hasSuffix:@".ttf"])
			//{
			//    fnt = [[fnt lastPathComponent] stringByDeletingPathExtension];
			//}
			node.setValue(name, fnt);
		}
	}else if (type == PropType::BLOCK){
		auto selectorName = readCachedString();
		int selectorTarget = readIntWithSign(false);
		if (setProp && selectorTarget){
			node.setValue(name, selectorName);
			// Objective C callbacks
//				Ref *target = NULL;
//				if (selectorTarget == TargetType::DocumentRoot){
//					target = _animationManager->getRootNode();
//				}else if (selectorTarget == TargetType::Owner){
//					target = _owner;
//				}
//				if (target){
//					auto resolver = dynamic_cast<SelectorResolver *>(target);
//					if (resolver) {
//						resolver->onResolve(node, selectorName);
//					}
//				}else{
//					log("CCBReader: Failed to find target for selector %s", selectorName.c_str());
//				}
		}
	}else if (type == PropType::CCB_FILE){
		auto ccbFileName = readCachedString();
		// Change path extension to .ccbi
		if (!ccbFileName.empty()){
			if (ccbFileName.rfind(".ccb") == ccbFileName.size() - 4) {
				ccbFileName = ccbFileName.substr(0, ccbFileName.size() - 4);
			}
			ccbFileName.append(".ccbi");
		}
		// Load sub file
		auto FU = FileUtils::getInstance();
		auto path = FU->fullPathForFilename(ccbFileName);
		if (FU->isFileExist(path)) {
			auto d = FU->getDataFromFile(path);
			CCAssert(!d.isNull(), std::string("Failed to find ccb file: ").append(ccbFileName).c_str());
			Reader reader;
//			reader._animationManager->rootContainerSize = parent->getContentSize();
			// Setup byte array & owner
			reader._data = d;
			reader._bytes = (unsigned char*)d.getBytes();
			reader._currentByte = 0;
			reader._currentBit = 0;
			reader._owner = _owner;
//			reader._animationManager->setOwner(_owner);
			auto ccbFile = reader.readNode();
//			if (ccbFile && reader._animationManager->getAutoPlaySequenceId() != -1){
//				// Auto play animations
//				reader._animationManager->runAnimationsForSequenceId(reader._animationManager->getAutoPlaySequenceId(), 0);
//			}
			if (setProp){
				node.setValue(name, ccbFile);
			}
		}
	}else{
		log("CCBReader: Failed to read property type %d", type);
	}
}

Keyframe Reader::readKeyframeOfType(const int type){
	Keyframe kf;
	kf.time = readFloat();
	
	int easingType = readIntWithSign(false);
	float easingOpt = 0;
	Value value;
	
	if (easingType == Easing::CUBIC_IN
		|| easingType == Easing::CUBIC_OUT
		|| easingType == Easing::CUBIC_IN_OUT
		|| easingType == Easing::ELASTIC_IN
		|| easingType == Easing::ELASTIC_OUT
		|| easingType == Easing::ELASTIC_IN_OUT){
		easingOpt = readFloat();
	}
	kf.easingType = easingType;
	kf.easingOpt = easingOpt;
	
	if (type == PropType::CHECK){
		value = readBool();
	}else if (type == PropType::BYTE){
		value = readBool();
	}else if (type == PropType::COLOR3){
		float r = readFloat();
		float g = readFloat();
		float b = readFloat();
		float a = readFloat();
		value.numbers.push_back(r);
		value.numbers.push_back(g);
		value.numbers.push_back(b);
		value.numbers.push_back(a);
	}else if (type == PropType::DEGREES || type == PropType::FLOAT){
		value = readFloat();
	}else if (type == PropType::SCALE_LOCK
			  || type == PropType::POSITION
			  || type == PropType::FLOAT_XY){
		float a = readFloat();
		float b = readFloat();
		value.numbers.push_back(a);
		value.numbers.push_back(b);
	}else if (type == PropType::SPRITE_FRAME){
		auto spriteFile = readCachedString();
//		auto spriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFile);
		value.string = spriteFile;
	}
	kf.value = value;
	return std::move(kf);
}

void didLoadFromCCB(){}

CCBNode Reader::readNodeGraphParent(CCBNode &parent){
	// Read class
	auto className = readCachedString();
	
	// Read assignment type and name
	int memberVarAssignmentType = readIntWithSign(false);
	std::string memberVarAssignmentName;
	if (memberVarAssignmentType){
		memberVarAssignmentName = readCachedString();
	}
	
	auto node = CCBNode::createWithClassName(className);
	CCAssert(node.type != CCBNode::Unknow, std::string("CCBReader: Could not create class of type ").append(className).c_str());
	
	// Set root node
//		if (!_animationManager->getRootNode()){
//			_animationManager->setRootNode(node);
//		}
	
	// Read animated properties
	std::map<int, Sequence> seqs;
	int numSequences = readIntWithSign(false);
	for (int i = 0; i < numSequences; ++ i){
		Sequence seq;
		seq.id = readIntWithSign(false);
		int numProps = readIntWithSign(false);
		for (int j = 0; j < numProps; ++ j){
			Sequence::Property seqProp;
			seqProp.name = readCachedString();
			seqProp.type = readIntWithSign(false);
			node.animatedProps.insert(seqProp.name);
			
			int numKeyframes = readIntWithSign(false);
			for (int k = 0; k < numKeyframes; ++ k){
				seqProp.keyframes.push_back(readKeyframeOfType(seqProp.type));
			}
			seq.properties[seqProp.name] = seqProp;
		}
		seqs[seq.id] = seq;
	}
	if (seqs.size() > 0){
		node.sequences = std::move(seqs);
//		_animationManager->addNode(node, std::move(seqs));
	}
	
	// Read properties
	int numRegularProps = readIntWithSign(false);
	int numExtraProps = readIntWithSign(false);
	int numProps = numRegularProps + numExtraProps;
	
	for (int i = 0; i < numProps; ++ i){
		bool isExtraProp = (i >= numRegularProps);
		readPropertyForNode(node/*, parent*/, isExtraProp);
	}
	
	if (node.type == CCBNode::File){
		// TODO
		// Handle sub ccb files (remove middle node)
//			auto nodeFile = dynamic_cast<File *>(node);
//			auto embeddedNode = nodeFile->getCCBFile();
//			embeddedNode->setPosition(nodeFile->getPosition());
//			embeddedNode->setPositionType(nodeFile->getPositionType());
//			//embeddedNode->setAnchorPoint(nodeFile->getAnchorPoint());
//			embeddedNode->setRotation(nodeFile->getRotation());
//			embeddedNode->setScaleX(nodeFile->getScaleX());
//			embeddedNode->setScaleY(nodeFile->getScaleY());
//			embeddedNode->setName(nodeFile->getName());
//			embeddedNode->setTag(nodeFile->getTag());
//			embeddedNode->setVisible(true);
//			//embeddedNode.ignoreAnchorPointForPosition = ccbFileNode.ignoreAnchorPointForPosition;
//			
////			_animationManager->moveAnimationsFromNode(nodeFile, embeddedNode);
//			nodeFile->setCCBFile(nullptr);
//			node = embeddedNode;
	}
	
	// Assign to variable (if applicable)
	if (memberVarAssignmentType){
//			Ref *target = NULL;
//			if (memberVarAssignmentType == TargetType::DocumentRoot){
//				target = _animationManager->getRootNode();
//			}else if (memberVarAssignmentType == TargetType::Owner){
//				target = _owner;
//			}
//			if (target){
//				bool assigned = false;
//				auto assigner = dynamic_cast<MemberVariableAssigner *>(target);
//				if (assigner) {
//					assigned = assigner->onAssign(memberVarAssignmentName, node);
//				}
//				if (!assigned && _assigner) {
//					assigned = _assigner->onAssign(memberVarAssignmentName, node);
//				}
//			}
	}
	
//		_animatedProps.clear();
	
	// Read physics
	bool hasPhysicsBody = readBool();
	if (hasPhysicsBody){
//#if CC_PLATFORM_IOS == CC_TARGET_PLATFORM
		// Read body shape
		int bodyShape = readIntWithSign(false);
		float cornerRadius = readFloat();
//#endif
		// Read points
		int numPoints = readIntWithSign(false);
		Vec2 *points = (Vec2 *)malloc(sizeof(Vec2) * numPoints);
		for (int i = 0; i < numPoints; i++){
			float x = readFloat();
			float y = readFloat();
			points[i] = Vec2(x, y);
		}
//#if CC_PLATFORM_IOS == CC_TARGET_PLATFORM
		// Create body
		PhysicsBody* body = NULL;
		if (bodyShape == 0){
			body = PhysicsBody::createEdgePolygon(points, numPoints);
//				cornerRadius:cornerRadius;
		}else if (bodyShape == 1){
			if (numPoints > 0){
				body = PhysicsBody::createCircle(cornerRadius);
				body->setPositionOffset(points[0]);
//					bodyWithCircleOfRadius:cornerRadius andCenter:points[0]];
			}
		}
		CCAssert(body, "Reader::readNodeGraphParent() Unknown body shape");
		
		bool dynamic = readBool();
		bool affectedByGravity = readBool();
		bool allowsRotation = readBool();
		body->setDynamic(dynamic);
		
		float density = readFloat();
		float friction = readFloat();
		float elasticity = readFloat();
		
		if (dynamic){
			body->setGravityEnable(affectedByGravity);
			body->setRotationEnable(allowsRotation);
		}
//			body.density = density;
//			body.friction = friction;
//			body.elasticity = elasticity;
//			node->setPhysicsBody(body);
//#endif
		free(points);
	}
	
	// Read and add children
	int numChildren = readIntWithSign(false);
	for (int i = 0; i < numChildren; ++ i){
		node.children.push_back(readNodeGraphParent(node));
	}
	return node;
}

bool Reader::readCallbackKeyframesForSeq(Sequence &seq){
	int numKeyframes = readIntWithSign(false);
	if (!numKeyframes){
		return true;
	}
	for (int i = 0; i < numKeyframes; ++ i){
		Keyframe kf;
		kf.time = readFloat();
		kf.value.string = readCachedString();
		kf.value.numbers.push_back(readIntWithSign(false));
		seq.callbackChannel.keyframes.push_back(kf);
	}
	return true;
}

bool Reader::readSoundKeyframesForSeq(Sequence &seq){
	int numKeyframes = readIntWithSign(false);
	if (!numKeyframes){
		return true;
	}
	for (int i = 0; i < numKeyframes; ++ i){
		float time = readFloat();
		auto soundFile = readCachedString();
		float pitch = readFloat();
		float pan = readFloat();
		float gain = readFloat();
		Keyframe kf;
		kf.time = time;
		kf.value.string = soundFile;
		kf.value.numbers.push_back(pitch);
		kf.value.numbers.push_back(pan);
		kf.value.numbers.push_back(gain);
		seq.soundChannel.keyframes.push_back(kf);
	}
	return true;
}

SequenceList Reader::readSequences(CCBNode &root){
	SequenceList seqList;
	int numSeqs = readIntWithSign(false);
	for (int i = 0; i < numSeqs; ++ i){
		Sequence seq;
		seq.duration = readFloat();
		seq.name = readCachedString();
		seq.id = readIntWithSign(false);
		seq.chainedId = readIntWithSign(true);
		if (!readCallbackKeyframesForSeq(seq) || !readSoundKeyframesForSeq(seq)){
//			return false;
		}
		seqList[seq.id] = std::move(seq);
//		_animationManager->sequences.push_back(std::move(seq));
	}
	root.autoPlaySequenceId = readIntWithSign(true);
	if (root.autoPlaySequenceId >= 0 && seqList.find(root.autoPlaySequenceId) != seqList.end()) {
		seqList[root.autoPlaySequenceId].autoPlay = true;
	}
//	_animationManager->autoPlaySequenceId = root.autoPlaySequenceId;
	return std::move(seqList);
}

bool Reader::readStringCache(){
	int numStrings = readIntWithSign(false);
	_stringCache.assign(numStrings, "");
	for (int i = 0; i < numStrings; i++){
		auto str = readUTF8();
//		log("Reader::readStringCache() %s", str.c_str());
		_stringCache[i] = std::move(str);
	}
	return true;
}

#define CHAR4(c0, c1, c2, c3) (((c0)<<24) | ((c1)<<16) | ((c2)<<8) | (c3))

bool Reader::readHeader(){
	// if no bytes loaded, don't crash about it.
	if(_bytes == nullptr)
		return false;
	// Read magic
	int magic = *((int*)(_bytes + _currentByte));
	_currentByte += 4;
	if (magic != CHAR4('c', 'c', 'b', 'i'))
		return false;
	
	// Read version
	int version = readIntWithSign(false);
	if (version != kCCBVersion){
		log("CCBReader: Incompatible ccbi file version (file: %d reader: %d)", version, kCCBVersion);
		return false;
	}
	return true;
}

//	void Reader::cleanUpNodeGraph(Node *node){
//		node->setUserObject(NULL);
//		for (auto child : node->getChildren()){
//			cleanUpNodeGraph(child);
//		}
//	}

CCBNode Reader::readNode(/*bool cleanUp, ActionManagers &am*/){
	CCBNode nodeRoot;
	if (!readHeader()){			return nodeRoot;}
	if (!readStringCache()){	return nodeRoot;}
	auto seqList = readSequences(nodeRoot);
	
//		_actionManagers = am;
	
	auto node = readNodeGraphParent(nodeRoot);
	node.rootSequences = std::move(seqList);
	node.autoPlaySequenceId = nodeRoot.autoPlaySequenceId;
//		_actionManagers[node] = _animationManager;
//		if (cleanUp){
//			cleanUpNodeGraph(node);
//		}
	return std::move(node);
}

// static
//	void Reader::callDidLoadFromCCBForNodeGraph(Node *nodeGraph){
//		for (auto child : nodeGraph->getChildren()){
//			callDidLoadFromCCBForNodeGraph(child);
//		}
//		auto ccbNode = dynamic_cast<CCBNode *>(nodeGraph);
//		if (ccbNode){
//			ccbNode->didLoadFromCCB();
//		}
//	}

CCBNode Reader::loadWithData(Data &d, Ref *owner/*, const cocos2d::Size &parentSize*/){
	// Setup byte array
	_data = d;
	_bytes = d.getBytes();
	_currentByte = 0;
	_currentBit = 0;
	
	_owner = owner;
	
//		_animationManager->rootContainerSize = parentSize.equals(cocos2d::Size::ZERO)? DIRECTOR_DESIGN_SIZE : parentSize;
//		_animationManager->setOwner(_owner);
	
//		ActionManagers actMGRs;
	auto nodeGraph = readNode();
//		if (nodeGraph && _animationManager->getAutoPlaySequenceId() != -1){
//			// Auto play animations
//			_animationManager->runAnimationsForSequenceId(_animationManager->getAutoPlaySequenceId(), 0);
//		}
//		for (auto it : actMGRs){
//			auto node = it.first;
//			node->setUserObject(it.second);
//		}
	// Call didLoadFromCCB
//		callDidLoadFromCCBForNodeGraph(nodeGraph);
	return std::move(nodeGraph);
}

CCBNode Reader::nodeGraphFromFile(const std::string &file, Ref *owner, const cocos2d::Size &parentSize){
	// Add ccbi suffix
	std::string path(file);
	if (path.rfind(".ccbi") != path.size() - 5){
		path.append(".ccbi");
	}
	auto FU = FileUtils::getInstance();
	path = FU->fullPathForFilename(path);
	auto d = FU->getDataFromFile(path);
	return loadWithData(d, owner);
}

CCBNode Reader::load(const std::string &file, Ref *owner, const cocos2d::Size &parentSize){
	Reader reader;
	return reader.nodeGraphFromFile(file, owner, parentSize);
}

CCBNode Reader::load(const std::string &file, Ref *owner){
	return load(file, owner, DIRECTOR_DESIGN_SIZE);
}

CCBNode Reader::load(const std::string &file){
	return load(file, NULL, DIRECTOR_DESIGN_SIZE);
}

// static
void Reader::setResourcePath(const std::string &searchPath){
	FileUtils::getInstance()->addSearchPath(searchPath);
}
NS_CCB_END
