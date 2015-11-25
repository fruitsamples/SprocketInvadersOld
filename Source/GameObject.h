//�	------------------------------------------------------------------------------------------	�//�//�	Copyright � 1996 Apple Computer, Inc., All Rights Reserved//�//�//�		You may incorporate this sample code into your applications without//�		restriction, though the sample code has been provided "AS IS" and the//�		responsibility for its operation is 100% yours.  However, what you are//�		not permitted to do is to redistribute the source as "DSC Sample Code"//�		after having made changes. If you're going to re-distribute the source,//�		we require that you make it clear in the source that the code was//�		descended from Apple Sample Code, but that you've made changes.//�//�		Authors://�			Chris De Salvo//�//�	------------------------------------------------------------------------------------------	�#ifndef __GAMEOBJECT__#define __GAMEOBJECT__//�	------------------------------	Includes#include "Particles.h"#include "Sprite.h"//�	------------------------------	Public Definitionstypedef enum ObjectKind{	objectEnemy,	objectEnemyShot,	objectGreenPlayer,	objectGreenPlayerShot,	objectRedPlayer,	objectRedPlayerShot,	objectEnemyParticles,	objectPlayerParticles,	objectPlasmaParticles,	objectMissileParticles} ObjectKind;//�	------------------------------	Public Types//�	Forward declaration of this structure so we can build a link list of//�	them within the actual structurestruct GameObject;typedef struct GameObject GameObject, *GameObjectPtr, **GameObjectHandle;typedef void (*ObjectAction)(GameObjectPtr theObject);struct GameObject{	GameObjectPtr	next;	GameObjectPtr	prev;	ObjectKind		kind;	Rect			screenRect;		//�	The area of the screen that the object was last drawn in	Rect			oldScreenRect;	//�	Last rect where the sprite was drawn	SInt16			screenX;		//�	Where the hotspots are in screen space	SInt16			screenY;		SInt16			velocityH;	SInt16			velocityV;		Rect			bounds;			//�	The object must stay within this rectangle.  This is defined in screen space		SInt16			frame;			//�	Which frame of animation is being displayed	UInt32			time;			//�	Local time for this object		union	{		SpritePtr	sprite;			//�	Animation frames and hot-spots for this object		ParticlesPtr particles;		//�	Particle system for particle objects	} objectData;	ObjectAction	action;			//�	This function controls all object actions	UInt32			refCon;			//�	Client-defined 32-bit scratch variable};//�	------------------------------	Public Variablesextern unsigned long	gNumGameObjects;//�	------------------------------	Public Functions#ifdef __cplusplusextern "C" {#endifextern GameObjectPtr GameObjectAllocate(void);extern void GameObjectDispose(GameObjectPtr go);extern void GameObjectAddToList(GameObjectPtr *list, GameObjectPtr go);extern void GameObjectRemoveFromList(GameObjectPtr *list, GameObjectPtr go);extern void GameObjectDisposeList(GameObjectPtr *list);extern void GameObjectSetSprite(GameObjectPtr go, SpritePtr sprite);extern SpritePtr GameObjectGetSprite(GameObjectPtr go);extern void GameObjectSetParticles(GameObjectPtr go, ParticlesPtr particles);extern ParticlesPtr GameObjectGetParticles(GameObjectPtr go);extern void GameObjectSetBounds(GameObjectPtr go, RectPtr r);extern RectPtr GameObjectGetBounds(GameObjectPtr go);extern void GameObjectListAdvance(GameObjectPtr list);extern void GameObjectListDraw(GameObjectPtr list, CGrafPtr dest);extern UInt32 GameObjectListCount(GameObjectPtr list);#ifdef __cplusplus}#endif#endif