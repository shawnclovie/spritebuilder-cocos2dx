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

#include "CCBSequence.h"

NS_CCB_BEGIN
Value::Value(){
	init();
}
Value::Value(const std::string &v){
	init();
	string = v;
}
Value::Value(const double number){
	init();
	numbers.push_back(number);
}
Value::Value(const double number[], const size_t count){
	init();
	for (size_t i = 0; i < count; ++ i) {
		numbers.push_back(number[i]);
	}
}
Value::Value(cocos2d::Ref *v){
	init();
	ref = v;
}
void Value::init(){
	any = nullptr;
	ref = nullptr;
}

double Value::number(const size_t index, const double def) const {
	return numbers.size() > index? numbers[index] : def;
}

double Value::operator=(double v){
	numbers.clear();
	numbers.push_back(v);
	return v;
}

bool Value::empty() const{
	return string.empty() && numbers.empty() && !any && !ref;
}

std::string Value::toString(){
	std::string r;
	char buf[100] = {'\0'};
	if (!string.empty()) {
		sprintf(buf, "(%d)\"", (int)string.size());
		r.append(buf).append(string).append("\"");
	}
	if (!numbers.empty()) {
		if (buf[0] != '\0') {	r.append(" ");}
		sprintf(buf, "%d", (int)numbers.size());
		r.append("(").append(buf).append(")[");
		bool _n1st = false;
		for (auto &n : numbers) {
			if (_n1st) {	r.append(",");}
			else{			_n1st = true;}
			bool isInt = n == (int)n;
			if (isInt) {	sprintf(buf, "%d", (int)n);}
			else{			sprintf(buf, "%f", n);}
			r.append(buf);
		}
		r.append("]");
	}
	if (ref) {
		if (buf[0] != '\0') {	r.append(" ");}
		sprintf(buf, "Ref:%p", ref);
		r.append(buf);
	}
	if (any) {
		if (buf[0] != '\0') {	r.append(" ");}
		sprintf(buf, "Any:%p", any);
		r.append(buf);
	}
	return std::move(r);
}

std::string Keyframe::toString(){
	std::string r("KeyF{Time=");
	char buf[100];
	sprintf(buf, "%f EType=%d, EOpt=%f}", time, easingType, easingOpt);
	r.append(value.toString()).append(buf);
	return std::move(r);
}

bool Sequence::Property::empty(){
	return name.empty() || keyframes.empty();
}

Sequence::Sequence() : autoPlay(false){
}

std::string Sequence::Property::toString(){
	char buf[100];
	sprintf(buf, "Prop(T%d,%s)KFs(%d){", type, name.c_str(), (int)keyframes.size());
	std::string r(buf);
	bool _n1st = false;
	for (auto &kf : keyframes) {
		if (_n1st) {	r.append(",");}
		else{			_n1st = true;}
		r.append(kf.toString());
	}
	r.append("}");
	return std::move(r);
}

std::string Sequence::toString(){
	char buf[100];
	sprintf(buf, "Seq(%d,name=%s,dur=%f,chain=%d)Props={\n", id, name.c_str(), duration, chainedId);
	std::string r(buf);
	bool _n1st = false;
	for (auto &it : properties) {
		if (_n1st) {	r.append(",");}
		else{			_n1st = true;}
		r.append(it.first).append("=").append(it.second.toString()).append("\n");
	}
	r.append("}");
	return std::move(r);
}
NS_CCB_END
