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

#pragma once

#include "CCBNode.h"
#include <functional>
#include "CCBSelectorResolver.h"

USING_NS_CC;

NS_CCB_BEGIN
#pragma mark Action Manager

class AnimationManager : public Ref {
public:
#pragma mark Delegate
	class Delegate {
	public:
		virtual void completedAnimationSequence(std::string name) = 0;
	};
protected:
	unsigned int _id;
	// {Node:{seqId:Sequence,...},...}
	std::map<Node *, SequenceList> _nodeSequences;
	// {Node:{propName:Value,...},...}
	std::map<Node *, Values> _baseValues;
	
	Sequence* _runningSequence;
	
	std::function<void(Ref *)> _callback;
	
	CC_SYNTHESIZE_READONLY(int, _autoPlaySequenceId, AutoPlaySequenceId);
	CC_SYNTHESIZE(Node *, _rootNode, RootNode);
	CC_SYNTHESIZE(Ref *, _owner, Owner);
	CC_SYNTHESIZE(Delegate *, _delegate, Delegate);
public:
	~AnimationManager();
	std::vector<Sequence> sequences;
	int autoPlaySequenceId;
	cocos2d::Size rootContainerSize;
	std::string lastCompletedSequenceName;
	
	CREATE_FUNC(AnimationManager);
	
	virtual bool init();
	cocos2d::Size getContainerSize(Node *node) const;
	
	void addNode(Node *node, const SequenceList &seq);
	void moveAnimationsFromNode(Node *fromNode, Node *toNode);
	
	Value getBaseValue(Node *node, const std::string &propName) const;
	void setBaseValue(const Value &value, Node *forNode, const std::string &propName);
	
	int getSequenceIdForName(const std::string &name) const;
	Sequence * getSequence(const int seqId);
	
	void runAnimationsForSequenceNamed(const std::string &name, float tweenDuration);
	void runAnimationsForSequenceNamed(const std::string &name);
	void runAnimationsForSequenceId(const int seqId, const float tweenDuration);
	
	void setCompletedAnimationCallback(std::function<void(Ref *)> callback);
	
	void debug();
	
	const Sequence * getRunningSequence();
	std::string getLastCompletedSequenceName();
protected:
	ActionInterval * createActionFromKeyframes(Keyframe *kf0, Keyframe *kf1, const std::string &name, Node *node);
	void setAnimatedProperty(const std::string &name, Node *node, const Value &value, const float tweenDuration);
	void setFirstFrameForNode(Node *node, const Sequence::Property &seqProp, const float tweenDuration);
	ActionInterval * createEaseAction(ActionInterval *action, const int easingType, const float easingOpt);
	void removeActionsByTag(const int tag, Node *node);
	void runActionsForNode(Node *node, const Sequence::Property &seqProp, const float tweenDuration);
	Action * createActionForCallbackChannel(const Sequence::Property &channel);
	Action * createActionForSoundChannel(const Sequence::Property &channel);
	void onSequenceCompleted();
};

#pragma mark Custom Animation Actions

class SetSpriteFrame : public ActionInstant {
protected:
	cocos2d::SpriteFrame * _spriteFrame;
public:
	/** creates a Place action with a position */
	static SetSpriteFrame * create(cocos2d::SpriteFrame *frame);
	/** Initializes a Place action with a position */
	bool init(cocos2d::SpriteFrame *frame);
	virtual ActionInstant * clone() const override;
	virtual ActionInstant * reverse() const override;
	virtual void update(float time) override;
};

class RotateTo : public ActionInterval {
protected:
	float _startAngle;
	float _dstAngle;
	float _diffAngle;
public:
	static RotateTo * create(float duration, float angle);
	virtual bool init(float duration, float angle);
	virtual ActionInterval * clone() const override;
	virtual ActionInterval * reverse() const override;
	virtual void startWithTarget(Node *node) override;
	virtual void update(float time) override;
};

class RotateXTo : public RotateTo {
public:
	virtual void startWithTarget(Node *node) override;
	virtual void update(float time) override;
};

class RotateYTo : public RotateTo {
public:
	virtual void startWithTarget(Node *node) override;
	virtual void update(float time) override;
};

class SoundEffect : public ActionInstant {
	std::string _soundFile;
	float _pitch;
	float _pan;
	float _gain;
public:
	static SoundEffect * createWithSoundFile(const std::string &file, float pitch, float pan, float gain);
	bool initWithSoundFile(const std::string &file, float pitch, float pan, float gain);
	virtual ActionInstant * clone() const override;
	virtual ActionInstant * reverse() const override;
	virtual void update(float time);
};
//
// EeseInstant
//
class EaseInstant : public ActionEase{
public:
	static EaseInstant * create(ActionInterval *act);
	virtual ActionEase * clone() const override;
	virtual ActionEase * reverse() const override;
	virtual void update(float time);
};
NS_CCB_END
