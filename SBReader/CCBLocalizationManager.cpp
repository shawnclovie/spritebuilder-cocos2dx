/*
 * SpriteBuilder: http://www.spritebuilder.org
 *
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

#include "CCBLocalizationManager.h"

USING_NS_CC;

NS_CCB_BEGIN
LocalizationManager * LocalizationManager::getInstance(){
	static LocalizationManager *inst = nullptr;
	if (!inst) {
		inst = new LocalizationManager;
		inst->loadStringsFile("Strings.ccbLang");
	}
	return inst;
}

void LocalizationManager::loadStringsFile(const std::string &file){
	auto FU = FileUtils::getInstance();
	// Load default localization dictionary
	auto path = FU->fullPathForFilename(file);
	
	// Load strings file
	__Dictionary* ser = __Dictionary::createWithContentsOfFile(path.c_str());
	
	// Check that format of file is correct
	{
		auto str = __String::create("SpriteBuilderTranslations");
		CCAssert(str->isEqual(ser->objectForKey("fileType")), "Invalid file format for SpriteBuilder localizations");
	}
	
	// Check that file version is correct
	CCAssert(dynamic_cast<__Integer *>(ser->objectForKey("fileVersion"))->getValue() == 1, "Translation file version is incompatible with this reader");
	
	Ref *o;
	
	// Load available languages
	auto langs = dynamic_cast<__Array *>(ser->objectForKey("activeLanguages"));
	CCARRAY_FOREACH(langs, o){
		auto lang = dynamic_cast<__String *>(o);
		activeLanguage.push_back(lang->getCString());
		if (language.empty()) {
			language = lang->getCString();
		}
	}
	
	// Load translations
	auto trans = dynamic_cast<__Array *>(ser->objectForKey("translations"));
	CCARRAY_FOREACH(trans, o){
		auto _tran = dynamic_cast<__Dictionary *>(o);
		auto key = dynamic_cast<__String *>(_tran->objectForKey("key"));
		auto values = dynamic_cast<__Dictionary *>(_tran->objectForKey("translations"));
		Translation tran;
		DictElement *de;
		CCDICT_FOREACH(values, de){
			tran[de->getStrKey()] = dynamic_cast<__String *>(de->getObject())->getCString();
		}
		translations[key->getCString()] = std::move(tran);
	}
}

const std::string & LocalizationManager::getLocalizedStringForKey(const std::string &key) const {
	auto it = translations.find(key);
	if (it != translations.end()) {
		auto itTran = it->second.find(language);
		if (itTran != it->second.end()) {
			return itTran->second;
		}
	}
	return key;
}
NS_CCB_END
