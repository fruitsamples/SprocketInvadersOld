/*	File:		main.c	Contains:	xxx put contents here xxx	Version:	xxx put version here xxx	Copyright:	� 1998-1999 by Apple Computer, Inc., all rights reserved.	File Ownership:		DRI:				xxx put dri here xxx		Other Contact:		xxx put other contact here xxx		Technology:			xxx put technology here xxx	Writers:		(cjd)	Chris De Salvo		(sjb)	Steve Bollinger		(BWS)	Brent Schorsch	Change History (most recent first):	  <SP17>	  3/3/99	cjd		Added calls to register app as an Appearance client	  <SP16>	 1/29/99	cjd		Replaced old RedbookHandler code with new CD_Utils code.	  <SP15>	 1/21/99	cjd		Removing 68K build code		<14>	 6/18/98	sjb		InputSprocket.h comes from <> place		<13>	 6/12/98	BWS		Now uses InputSprocket 68k*///�	------------------------------------------------------------------------------------------	�//�//�	Copyright � 1996 Apple Computer, Inc., All Rights Reserved//�//�//�		You may incorporate this sample code into your applications without//�		restriction, though the sample code has been provided "AS IS" and the//�		responsibility for its operation is 100% yours.  However, what you are//�		not permitted to do is to redistribute the source as "DSC Sample Code"//�		after having made changes. If you're going to re-distribute the source,//�		we require that you make it clear in the source that the code was//�		descended from Apple Sample Code, but that you've made changes.//�//�		Authors://�			Chris De Salvo//�			Jamie Osborne//�//�	------------------------------------------------------------------------------------------	�//�	------------------------------	Includes#include <Appearance.h>#include <Fonts.h>#include <Movies.h>#include <StandardFile.h>#include <DrawSprocket.h>#include <InputSprocket.h>#include "ErrorHandler.h"#include "EventHandler.h"#include "Graphics.h"#include "MenuHandler.h"#include "MoviePlayback.h"#include "CD_Utils.h"#include "SoundHandler.h"#include "NetSprocketSupport.h"//�	------------------------------	Private Definitions//�	------------------------------	Private Types//�	------------------------------	Private Variablesstatic StandardFileReply	gTheReply;//�	------------------------------	Private Functionsvoid main(void);static void ToolboxInit(void);//�	------------------------------	Public VariablesBoolean	gCDAudio = true;//�	--------------------	mainvoidmain(void){//�	When we start up we record the state of the modifier keys on the keyboard//�	and then pass those values to the various initialization routines in the game.SInt16	startupModifiers;	ToolboxInit();	RegisterAppearanceClient();	//�	Make sure that the DrawSprocket and InputSprocket libraries are present	if (DSpStartup == nil)		FatalError("This game requires DrawSprocket.");	if (ISpInit == nil)		FatalError("This game requires InputSprocket.");	//�	Reseed the random number generator	qd.randSeed =  TickCount();	startupModifiers = EventInit();	gNetSprocketPresent = InitNetworking();	MenuInit();	GraphicsInit(startupModifiers);	SoundHandlerInit();	CD_Open();		//�	Run the game	EventLoop();	ShutdownMoviePlayback();	//�	Shut down the handlers and quit	SoundHandlerReset();	GraphicsReset();	if (gNetSprocketPresent)		ShutdownNetworking();		ISpStop();	UnregisterAppearanceClient();	FlushEvents(everyEvent, 0);	ExitToShell();}//�	--------------------	mainstatic voidToolboxInit(void){	MaxApplZone();	InitGraf(&qd.thePort);	InitFonts();	InitWindows();	InitMenus();	TEInit();	InitDialogs(nil);	InitCursor();	//�	Initialize the Quicktime Movie Toolbox	EnterMovies();	//�	Flush out mouse and keyboard events.  This prevents some things like clicking through	//�	to the Finder while the app is launching, etc.	FlushEvents(mDownMask | keyDownMask | autoKeyMask, 0);}