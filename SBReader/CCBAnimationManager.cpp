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

#include "CCBAnimationManager.h"
#include "CCBReader.h"

NS_CCB_BEGIN
bool AnimationManager::init() {
	static unsigned int ccbAnimationManagerID = 0;
	_id = ccbAnimationManagerID;
	++ ccbAnimationManagerID;
	return true;
}

cocos2d::Size AnimationManager::getContainerSize(Node *node) const {
	return node? node->getContentSize() : rootContainerSize;
}

void AnimationManager::addNode(Node *node, const SequenceList &seq) {
	_nodeSequences[node] = seq;
}

void AnimationManager::moveAnimationsFromNode(Node *fromNode, Node *toNode) {
	{
		// Move base values
		auto it = _baseValues.find(fromNode);
		if (it != _baseValues.end()) {
			_baseValues[toNode] = it->second;
			_baseValues.erase(it);
		}
	}
	// Move keyframes
	auto it = _nodeSequences.find(fromNode);
	if (it != _nodeSequences.end()) {
		_nodeSequences[toNode] = it->second;
		_nodeSequences.erase(it);
	}
}

void AnimationManager::setBaseValue(const Value &value, Node *forNode, const std::string &propName){
	auto it = _baseValues.find(forNode);
	if (it == _baseValues.end()){
		Values props;
		props[propName] = value;
		_baseValues[forNode] = std::move(props);
	}else{
		it->second[propName] = value;
	}
}

Value AnimationManager::getBaseValue(Node *node, const std::string &propName) const {
	auto it = _baseValues.find(node);
	if (it != _baseValues.end()) {
		auto itProp = it->second.find(propName);
		if (itProp != it->second.end()) {
			return itProp->second;
		}
	}
	return Value();
}

int AnimationManager::getSequenceIdForName(const std::string &name) const {
	for (auto seq : sequences){
		if (seq.name.compare(name) == 0){
			return seq.id;
		}
	}
	return -1;
}

Sequence * AnimationManager::getSequence(const int seqId) {
	for (auto &seq : sequences) {
		if (seq.id == seqId){
			return &seq;
		}
	}
	return nullptr;
}

ActionInterval * AnimationManager::createActionFromKeyframes(Keyframe *kf0, Keyframe *kf1, const std::string &name, Node *node){
	float duration = kf1->time - kf0->time;
	if (name.compare("rotation") == 0){
		return RotateTo::create(duration, atof(kf1->value.string.c_str()));
	}else if (name.compare("rotationalSkewX") == 0){
		return RotateXTo::create(duration, atof(kf1->value.string.c_str()));
	}else if (name.compare("rotationalSkewY") == 0){
		return RotateYTo::create(duration, atof(kf1->value.string.c_str()));
	}else if (name.compare("opacity") == 0){
		return FadeTo::create(duration, atoi(kf1->value.string.c_str()));
	}else if (name.compare("color") == 0){
		return TintTo::create(duration, kf1->value.number(0), kf1->value.number(1), kf1->value.number(2));
	}else if (name.compare("visible") == 0){
		ActionInstant *act;
		if(kf1->value.number(0)){	act = Show::create();}
		else{						act = Hide::create();}
		return cocos2d::Sequence::createWithTwoActions(DelayTime::create(duration), act);
	}else if (name.compare("spriteFrame") == 0){
		return cocos2d::Sequence::createWithTwoActions(DelayTime::create(duration), SetSpriteFrame::create(dynamic_cast<cocos2d::SpriteFrame *>(kf1->value.ref)));
	}else if (name.compare("position") == 0){
		// Get position type
		//int type = [[[self baseValueForNode:node propertyName:name] objectAtIndex:2] intValue];
		// Get relative position
		float x = kf1->value.number(0);
		float y = kf1->value.number(1);
		
		//CGSize containerSize = [self containerSize:node.parent];
		
		//CGPoint absPos = [node absolutePositionFromRelative:ccp(x,y) type:type parentSize:containerSize propertyName:name];
		
		return MoveTo::create(duration, cocos2d::Point(x,y));
	}
	else if (name.compare("scale") == 0){
		// Get position type
		//int type = [[[self baseValueForNode:node propertyName:name] objectAtIndex:2] intValue];
		// Get relative scale
		float x = kf1->value.number(0);
		float y = kf1->value.number(1);
		
		/*
		 if (type == kCCBScaleTypeMultiplyResolution)
		 {
		 float resolutionScale = [node resolutionScale];
		 x *= resolutionScale;
		 y *= resolutionScale;
		 }*/
		
		return ScaleTo::create(duration, x, y);
	}else if (name.compare("skew") == 0){
		float x = kf1->value.number(0);
		float y = kf1->value.number(1);
		return SkewTo::create(duration, x, y);
	}else{
		log("CCBReader: Failed to create animation for property: %s", name.c_str());
	}
	return NULL;
}

void AnimationManager::setAnimatedProperty(const std::string &name, Node *node, const Value &value, const float tweenDuration){
	if (tweenDuration > 0){
		// Create a fake keyframe to generate the action from
		Keyframe kf1;
		kf1.value = value;
		kf1.time = tweenDuration;
		kf1.easingType = Easing::LINEAR;
		// Animate
		auto tweenAction = createActionFromKeyframes(NULL, &kf1, name, node);
		node->runAction(tweenAction);
	}else{
		// Just set the value
		if (name.compare("position") == 0){
			// Get position type
			//int type = [[[self baseValueForNode:node propertyName:name] objectAtIndex:2] intValue];
			// Get relative position
			float x = value.number(0);
			float y = value.number(1);
			node->setPosition(x, y);
		}else if (name.compare("scale") == 0){
			// Get scale type
			//int type = [[[self baseValueForNode:node propertyName:name] objectAtIndex:2] intValue];
			// Get relative scale
			float x = value.number(0);
			float y = value.number(1);
			node->setScale(x, y);
		}else if (name.compare("skew") == 0){
			node->setSkewX(value.number(0));
			node->setSkewY(value.number(1));
		}else{
			// TODO Set other values.
		}
	}
}

void AnimationManager::setFirstFrameForNode(Node *node, const Sequence::Property &seqProp, const float tweenDuration){
	auto &keyframes = seqProp.keyframes;
	if (keyframes.empty()){
		// Use base value (no animation)
		auto baseValue = getBaseValue(node, seqProp.name);
//			NSAssert1(baseValue, @"No baseValue found for property (%@)", seqProp.name);
		setAnimatedProperty(seqProp.name, node, baseValue, tweenDuration);
	}else{
		// Use first keyframe
		auto &keyframe = keyframes[0];
		setAnimatedProperty(seqProp.name, node, keyframe.value, tweenDuration);
	}
}

ActionInterval * AnimationManager::createEaseAction(ActionInterval *action, const int easingType, const float easingOpt){
	if (dynamic_cast<cocos2d::Sequence *>(action) || easingType == Easing::LINEAR) {
		return action;
	}
	if (easingType == Easing::INSTANT){
		return EaseInstant::create(action);
	}else if (easingType == Easing::CUBIC_IN){
		return EaseIn::create(action, easingOpt);
	}else if (easingType == Easing::CUBIC_OUT){
		return EaseOut::create(action, easingOpt);
	}else if (easingType == Easing::CUBIC_IN_OUT){
		return EaseInOut::create(action, easingOpt);
	}else if (easingType == Easing::BACK_IN){
		return EaseBackIn::create(action);
	}else if (easingType == Easing::BACK_OUT){
		return EaseBackOut::create(action);
	}else if (easingType == Easing::BACK_IN_OUT){
		return EaseBackInOut::create(action);
	}else if (easingType == Easing::BOUNCE_IN){
		return EaseBounceIn::create(action);
	}else if (easingType == Easing::BOUNCE_OUT){
		return EaseBounceOut::create(action);
	}else if (easingType == Easing::BOUNCE_IN_OUT){
		return EaseBounceInOut::create(action);
	}else if (easingType == Easing::ELASTIC_IN){
		return EaseElasticIn::create(action, easingOpt);
	}else if (easingType == Easing::ELASTIC_OUT){
		return EaseElasticOut::create(action, easingOpt);
	}else if (easingType == Easing::ELASTIC_IN_OUT){
		return EaseElasticInOut::create(action, easingOpt);
	}
	log("CCBReader: Unkown easing type %d", easingType);
	return action;
}

void AnimationManager::removeActionsByTag(const int tag, Node *node){
	auto am = Director::getInstance()->getActionManager();
	while (am->getActionByTag(tag, node)){
		am->removeActionByTag(tag, node);
	}
}

void AnimationManager::runActionsForNode(Node *node, const Sequence::Property &seqProp, const float tweenDuration){
	auto &keyframes = seqProp.keyframes;
	int numKeyframes = (int)keyframes.size();
	if (numKeyframes > 1){
		// Make an animation!
		Vector<FiniteTimeAction *> actions;
		auto &keyframeFirst = keyframes[0];
		float timeFirst = keyframeFirst.time + tweenDuration;
		
		if (timeFirst > 0){
			actions.pushBack(DelayTime::create(timeFirst));
		}
		
		for (int i = 0; i < numKeyframes - 1; ++ i){
			auto &kf0 = keyframes[i];
			auto &kf1 = keyframes[i+1];
			auto action = createActionFromKeyframes(const_cast<Keyframe *>(&kf0), const_cast<Keyframe *>(&kf1), seqProp.name, node);
			if (action){
				// Apply easing
				action = createEaseAction(action, kf0.easingType, kf0.easingOpt);
				actions.pushBack(action);
			}
		}
		
		auto seq = cocos2d::Sequence::create(actions);
		seq->setTag(_id);
		node->runAction(seq);
	}
}

Action * AnimationManager::createActionForCallbackChannel(const Sequence::Property &channel){
	float lastKeyframeTime = 0;
	Vector<FiniteTimeAction *> actions;
	for (auto &keyframe : channel.keyframes){
		float timeSinceLastKeyframe = keyframe.time - lastKeyframeTime;
		lastKeyframeTime = keyframe.time;
		if (timeSinceLastKeyframe > 0){
			actions.pushBack(DelayTime::create(timeSinceLastKeyframe));
		}
		auto &selectorName = keyframe.value.string;
		int selectorTarget = keyframe.value.number(1);
		// Callback through obj-c
		Ref *target = NULL;
		if (selectorTarget == TargetType::DOCUMENT_ROOT){
			target = getRootNode();
		}else if (selectorTarget == TargetType::OWNER){
			target = _owner;
		}
		if (target) {
			if (selectorName.empty()) {
				log("AnimationManager::createActionForCallbackChannel() Unexpected empty selector.");
			}else{
				ActionInstant *action = nullptr;
				auto resolver = dynamic_cast<SelectorResolver *>(target);
				if (resolver) {
					action = resolver->onResolveCallFunc(selectorName);
				}
				if (action) {
					actions.pushBack(action);
				}
			}
		}
	}
	return actions.empty()? nullptr : cocos2d::Sequence::create(actions);
}

Action * AnimationManager::createActionForSoundChannel(const Sequence::Property &channel){
	float lastKeyframeTime = 0;
	Vector<FiniteTimeAction *> actions;
	for (auto &keyframe : channel.keyframes){
		float timeSinceLastKeyframe = keyframe.time - lastKeyframeTime;
		lastKeyframeTime = keyframe.time;
		if (timeSinceLastKeyframe > 0){
			actions.pushBack(DelayTime::create(timeSinceLastKeyframe));
		}
		auto &soundFile = keyframe.value.string;
		float pitch = keyframe.value.number(0);
		float pan = keyframe.value.number(1);
		float gain = keyframe.value.number(2);
		actions.pushBack(SoundEffect::createWithSoundFile(soundFile, pitch, pan, gain));
	}
	return actions.empty()? nullptr : cocos2d::Sequence::create(actions);
}

void AnimationManager::runAnimationsForSequenceId(const int seqId, const float tweenDuration){
//		CCAssert(seqId != -1, "Sequence id %d couldn't be found",seqId);
	// Stop actions associated with this animation manager
	removeActionsByTag(_id, getRootNode());
	for (auto it : _nodeSequences){
		auto node = it.first;
		// Stop actions associated with this animation manager
		removeActionsByTag(_id, node);
		
		auto &seqs = it.second;
		auto &seq = seqs[seqId];
		StringVector propNames;
		// Reset nodes that have sequence node properties, and run actions on them
		for (auto &itProp : seq.properties){
			propNames.push_back(itProp.first);
			auto &prop = itProp.second;
			setFirstFrameForNode(node, prop, tweenDuration);
			runActionsForNode(node, prop, tweenDuration);
		}
		// Reset the nodes that may have been changed by other timelines
		auto &nodeBaseValues = _baseValues[node];
		for (auto &itValue : nodeBaseValues){
			auto &propName = itValue.first;
			auto itBegin = propNames.begin();
			auto itEnd = propNames.end();
			if (std::find(itBegin, itEnd, propName) != itEnd){
				auto itBV = nodeBaseValues.find(propName);
				if (itBV != nodeBaseValues.end()) {
					setAnimatedProperty(propName, node, itBV->second, tweenDuration);
				}
			}
		}
	}
	// Make callback at end of sequence
	auto seq = getSequence(seqId);
	if (seq) {
		auto completeAction = cocos2d::Sequence::createWithTwoActions(DelayTime::create(seq->duration + tweenDuration), CallFunc::create([this](){
			onSequenceCompleted();
		}));
		completeAction->setTag(_id);
		getRootNode()->runAction(completeAction);
		
		// Playback callbacks and sounds
		if (!seq->callbackChannel.empty()){
			// Build sound actions for channel
			auto action = createActionForCallbackChannel(seq->callbackChannel);
			if (action){
				action->setTag(_id);
				getRootNode()->runAction(action);
			}
		}
		
		if (!seq->soundChannel.empty()){
			// Build sound actions for channel
			auto action = createActionForSoundChannel(seq->soundChannel);
			if (action){
				action->setTag(_id);
				getRootNode()->runAction(action);
			}
		}
	}
	// Set the running scene
	_runningSequence = seq;
}

void AnimationManager::runAnimationsForSequenceNamed(const std::string &name, const float tweenDuration){
	int seqId = getSequenceIdForName(name);
	runAnimationsForSequenceId(seqId, tweenDuration);
}

void AnimationManager::runAnimationsForSequenceNamed(const std::string &name){
	runAnimationsForSequenceNamed(name, 0);
}

void AnimationManager::onSequenceCompleted(){
	// Save last completed sequence
	if (lastCompletedSequenceName != _runningSequence->name){
		lastCompletedSequenceName = _runningSequence->name;
	}
	// Play next sequence
	int nextSeqId = _runningSequence->chainedId;
	_runningSequence = NULL;
	
	// Callbacks
	if(_delegate){
		_delegate->completedAnimationSequence(lastCompletedSequenceName);
	}
	// Run next sequence if callbacks did not start a new sequence
	if (_runningSequence == NULL && nextSeqId != -1){
		runAnimationsForSequenceId(nextSeqId, 0);
	}
}

const Sequence * AnimationManager::getRunningSequence(){
	return _runningSequence;
}

AnimationManager::~AnimationManager(){
}

void AnimationManager::debug(){
	//NSLog(@"baseValues: %@", baseValues);
	//NSLog(@"nodeSequences: %@", nodeSequences);
}

#pragma mark Custom Actions

SetSpriteFrame * SetSpriteFrame::create(cocos2d::SpriteFrame *sf){
	auto inst = new SetSpriteFrame;
	if (inst && inst->init(sf)) {
		inst->autorelease();
	}else{
		CC_SAFE_RELEASE_NULL(inst);
	}
	return inst;
}

bool SetSpriteFrame::init(cocos2d::SpriteFrame *sf){
	_spriteFrame = sf;
	return true;
}

ActionInstant * SetSpriteFrame::clone() const {
	return create(_spriteFrame);
}

ActionInstant * SetSpriteFrame::reverse() const {
	return clone();
}

void SetSpriteFrame::update(float time){
	auto sp = dynamic_cast<Sprite *>(getTarget());
	if (sp) {
		sp->setSpriteFrame(_spriteFrame);
	}
}

RotateTo * RotateTo::create(float duration, float angle){
	auto inst = new RotateTo;
	if (inst && inst->init(duration, angle)) {
		inst->autorelease();
	}else{
		CC_SAFE_RELEASE_NULL(inst);
	}
	return inst;
}

bool RotateTo::init(float duration, float angle){
	if (ActionInterval::initWithDuration(duration)) {
		_dstAngle = angle;
		return true;
	}
	return false;
}

ActionInterval * RotateTo::clone() const {
	return create(_duration, _dstAngle);
}

ActionInterval * RotateTo::reverse() const {
	return clone();
}

void RotateTo::startWithTarget(Node *node){
	_originalTarget = node;
	_target = node;
	_elapsed = 0;
	_firstTick = true;
	_startAngle = _target->getRotation();
	_diffAngle = _dstAngle - _startAngle;
}

void RotateTo::update(float t){
	_target->setRotation(_startAngle + _diffAngle * t);
}

void RotateXTo::startWithTarget(Node *node){
	RotateTo::startWithTarget(node);
	_startAngle = _target->getRotationSkewX();
	_diffAngle = _dstAngle - _startAngle;
}

void RotateXTo::update(float t){
	_target->setRotationSkewX(_startAngle + _diffAngle * t);
}

void RotateYTo::startWithTarget(Node *node){
	RotateTo::startWithTarget(node);
	_startAngle = _target->getRotationSkewY();
	_diffAngle = _dstAngle - _startAngle;
}

void RotateYTo::update(float t){
	_target->setRotationSkewY(_startAngle + _diffAngle * t);
}

SoundEffect * SoundEffect::createWithSoundFile(const std::string &f, float pi, float pan, float gain){
	auto inst = new SoundEffect;
	if (inst && inst->initWithSoundFile(f, pi, pan, gain)) {
		inst->autorelease();
	}else{
		CC_SAFE_RELEASE_NULL(inst);
	}
	return inst;
}

bool SoundEffect::initWithSoundFile(const std::string &file, float pi, float pa, float ga){
	_soundFile = file;
	_pitch = pi;
	_pan = pa;
	_gain = ga;
	return true;
}

ActionInstant * SoundEffect::clone() const {
	return createWithSoundFile(_soundFile, _pitch, _pan, _gain);
}

ActionInstant * SoundEffect::reverse() const {
	return clone();
}

void SoundEffect::update(float time){
	auto SA = CocosDenshion::SimpleAudioEngine::getInstance();
	SA->setEffectsVolume(_gain);
	SA->playEffect(_soundFile.c_str());
}

EaseInstant * EaseInstant::create(cocos2d::ActionInterval *act){
	auto inst = new EaseInstant;
	if (inst && inst->initWithAction(act)) {
		inst->autorelease();
	}else{
		CC_SAFE_RELEASE_NULL(inst);
	}
	return inst;
}

ActionEase * EaseInstant::clone() const {
	return create(_inner);
}

ActionEase * EaseInstant::reverse() const {
	return clone();
}

void EaseInstant::update(float t){
	_inner->update(t < 0? 0 : 1);
}
NS_CCB_END
