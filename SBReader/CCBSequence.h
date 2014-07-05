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
#include "CCBEnum.h"

NS_CCB_BEGIN
struct Value {
	std::string string;
	std::vector<double> numbers;
	void *any;
	cocos2d::Ref *ref;
	Value();
	Value(const std::string &v);
	Value(const double number);
	Value(const double number[], const size_t count);
	Value(cocos2d::Ref *v);
	void init();
	double number(const size_t index = 0, const double def = 0) const;
	double operator=(double v);
	bool empty() const;
	std::string toString();
};
struct Keyframe {
	Value value;
	float time;
	int easingType;
	float easingOpt;
	std::string toString();
};

class Sequence {
public:
	class Property {
	public:
		std::string name;
		std::vector<Keyframe> keyframes;
		int type;
		bool empty();
		std::string toString();
	};
	typedef std::map<std::string, Property> Properties;
	float duration;
	std::string name;
	int id;
	int chainedId;
	bool autoPlay;
	Properties properties;
	
	// Channels
	Property callbackChannel;
	Property soundChannel;
	
	Sequence();
	std::string toString();
};
typedef std::map<int, Sequence> SequenceList;
typedef std::map<std::string, Value> Values;
NS_CCB_END
