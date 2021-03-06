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
#include "CCBAnimationManager.h"
#include "CCBMemberVariableAssigner.h"

NS_CCB_BEGIN
USING_NS_CC;

//class File : public CCBNode {
//	CREATE_FUNC(File);
//	CC_SYNTHESIZE_RETAIN(CCBNode *, _ccbFile, CCBFile);
//};
/**
 The CCBReader loads node graphs created by SpriteBuilder (or other editors using the same format). If you are using SpriteBuilder it's strongly recommended that you set up the CCFileUtils using the configureCCFileUtils method or use the Xcode project file created by SpriteBuilder.
 
 Load your SpriteBuilder files using the load: method, if you are loading a scene use the loadAsScene: method, which will wrap the loaded node graph in a CCScene object.
 
 You can optionally pass an owner object to the CCBReader. If you have assigned member variables in your ccb-files to the owner, they will be set on the owner object when the file is loaded.
 
 When all loading is complete, the didLoadFromCCB method will be called on all loaded nodes (if it has been implemented).
 
 If you are using animations a CCBAnimationManager will be assigned to all ccb-file root node's userObject properties. The top CCBAnimationManager is also assigned to the CCBReader's animationManager property.
 */
class Reader {
protected:
//		typedef std::map<Node *, AnimationManager *> ActionManagers;

	Data _data;
	unsigned char *_bytes;
	int _currentByte;
	int _currentBit;

	std::vector<std::string> _stringCache;
	std::set<void *> _loadedSpriteSheets;

	Ref *_owner;

//		ActionManagers _actionManagers;
//		std::set<std::string> _animatedProps;
	
	MemberVariableAssigner *_assigner;
public:
	Reader();
	~Reader();
	/**
	 *  Once a ccb-file has been loaded, the animationManager property will be set to contain the top level CCBAnimationManager
	 */
//		CC_SYNTHESIZE_RETAIN(AnimationManager *, _animationManager, AnimationManager);
	/// -----------------------------------------------------------------------
	/// @name Setup
	/// -----------------------------------------------------------------------

	/**
	 *  Call this method to configure the CCFileUtils to work correctly with SpriteBuilder. It will setup search paths for the resources to use with the current device and resolution. It assumes that the SpriteBuilder resources has been published to a directory named Published-iOS that has been added as a blue folder in Xcode.
	 */
	//static void configureFileUtils();

	/// -----------------------------------------------------------------------
	/// @name Instantiation
	/// -----------------------------------------------------------------------

	/**
	 *  Creates a new CCBReader.
	 *  @return A new CCBReader.
	 */
//		CREATE_FUNC(Reader);

	/// -----------------------------------------------------------------------
	/// @name Loading Files
	/// -----------------------------------------------------------------------

	/**
	 *  Loads a ccbi-file with the specified name. Using the extension is optional, e.g. both MyNodeGraph and MyNodeGraph.ccbi will work.
	 *
	 *  @param file Name of the file to load.
	 *
	 *  @return The loaded node graph.
	 */
	static CCBNode load(const std::string &file);

	/**
	 *  Loads a ccbi-file with the specified name and owner. Using the extension is optional, e.g. both MyNodeGraph and MyNodeGraph.ccbi will work.
	 *
	 *  @param file Name of the file to load.
	 *  @param owner The owner object used to load the file.
	 *
	 *  @return The loaded node graph.
	 */
	static CCBNode load(const std::string &file, Ref *owner);
	
	static CCBNode load(const std::string &file, Ref *owner, const cocos2d::Size &parentSize);

	/**
	 *  Loads a ccbi-file from the provided NSData object. This method is useful if you load ccbi-files from the internet. If you are not using the owner variable, pass NULL.
	 *
	 *  @param data       Data object to load the ccbi-file from.
	 *  @param owner      The owner object used to load the file, or NULL if not used.
	 *
	 *  @return The loaded node graph.
	 */
	CCBNode loadWithData(Data &d, Ref *owner/*, const cocos2d::Size &parentSize = cocos2d::Size::ZERO*/);
protected:
	unsigned char readByte();
	bool readBool();
	std::string readUTF8();
	void alignBits();
	int readIntWithSign(bool sign);
	float readFloat();
	std::string readCachedString();
	
	void readPropertyForNode(CCBNode &node/*, CCBNode &parent*/, bool isExtraProp);
	Keyframe readKeyframeOfType(const int type);
	CCBNode readNode(/*bool cleanUp, ActionManagers &am*/);
	CCBNode readNodeGraphParent(CCBNode &parent);
	bool readCallbackKeyframesForSeq(Sequence &seq);
	bool readSoundKeyframesForSeq(Sequence &seq);
	SequenceList readSequences(CCBNode &root);
	bool readStringCache();
	bool readHeader();
//		void cleanUpNodeGraph(Node *node);
	
//		static void callDidLoadFromCCBForNodeGraph(Node *nodeGraph);
	
	CCBNode nodeGraphFromFile(const std::string &file, Ref *owner, const cocos2d::Size &parentSize);
	
	static void setResourcePath(const std::string &searchPath);
};
NS_CCB_END
