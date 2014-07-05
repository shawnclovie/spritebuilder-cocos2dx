//
//  CCBEnum.h
//
//  Created by Shawn Clovie on 6/29/14.
//
//
#ifndef _CCBEnum_h
#define _CCBEnum_h

#include "cocos2d.h"

#define kCCBVersion 6

#define NS_CCB_BEGIN namespace spritebuilder {
#define NS_CCB_END }

NS_CCB_BEGIN
enum PropType {
	POSITION = 0,
	SIZE,
	POINT,
	POINT_LOCK,
	SCALE_LOCK,
	DEGREES,
	INTEGER,
	FLOAT,
	FLOAT_VAR,
	CHECK,
	SPRITE_FRAME,
	TEXTURE,
	BYTE,
	COLOR3,
	COLOR4F_VAR,
	FLIP,
	BLENDMODE,
	FNT_FILE,
	TEXT,
	FONT_TTF,
	INTEGER_LABELED,
	BLOCK,
	ANIMATION,
	CCB_FILE,
	STRING,
	BLOCK_CONTROL,
	FLOAT_SCALE,
	FLOAT_XY,
	COLOR4,
};
struct CCBFloat{
	enum{
		_0 = 0,
		_1,
		MINUS1,
		_05,
		INTEGER,
		FULL,
	};
};

enum Platform{
	ALL = 0,
	IOS,
	MAC
};

enum TargetType{
	NONE = 0,
	DOCUMENT_ROOT = 1,
	OWNER = 2,
};

enum Easing {
	INSTANT,
	LINEAR,
	
	CUBIC_IN,
	CUBIC_OUT,
	CUBIC_IN_OUT,
	
	ELASTIC_IN,
	ELASTIC_OUT,
	ELASTIC_IN_OUT,
	
	BOUNCE_IN,
	BOUNCE_OUT,
	BOUNCE_IN_OUT,
	
	BACK_IN,
	BACK_OUT,
	BACK_IN_OUT
};

struct Position{
	enum Unit{
		/// Position is set in points (this is the default)
		POINTS,
		/// Position is UI points, on iOS this corresponds to the native point system
		UI_POINTS,
		/// Position is a normalized value multiplied by the content size of the parent's container
		NORMALLIZED,
	};
	enum ReferenceCorner{
		/// Position is relative to the bottom left corner of the parent container (this is the default)
		BOTTOM_LEFT,
		/// Position is relative to the top left corner of the parent container
		TOP_LEFT,
		/// Position is relative to the top right corner of the parent container
		TOP_RIGHT,
		/// Position is relative to the bottom right corner of the parent container
		BOTTOM_RIGHT,
	};
	struct Type{
		Unit xUnit;
		Unit yUnit;
		ReferenceCorner corner;
		Type(ReferenceCorner cor, Unit xu, Unit yu){
			xUnit = xu;
			yUnit = yu;
			corner = cor;
		}
	};
};
struct Size{
	enum Unit{
		/// Content size is set in points (this is the default)
		POINTS,
		/// Position is UI points, on iOS this corresponds to the native point system
		UI_POINTS,
		/// Content size is a normalized value multiplied by the content size of the parent's container
		NORMALIZED,
		/// Content size is the size of the parents container inset by the supplied value
		INSET_POINTS,
		/// Content size is the size of the parents container inset by the supplied value multiplied by the UIScaleFactor (as defined by CCDirector)
		INSET_UI_POINTS,
	};
	struct Type{
		Unit widthUnit;
		Unit heightUnit;
		Type(Unit wu, Unit hu){
			widthUnit = wu;
			heightUnit = hu;
		}
	};
};

typedef std::vector<std::string> StringVector;

NS_CCB_END
#endif
