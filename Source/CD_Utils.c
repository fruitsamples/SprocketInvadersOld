/*	File:		CD_Utils.c	Contains:	xxx put contents here xxx	Version:	xxx put version here xxx	Copyright:	© 1999 by Apple Computer, Inc., all rights reserved.	File Ownership:		DRI:				xxx put dri here xxx		Other Contact:		xxx put other contact here xxx		Technology:			xxx put technology here xxx	Writers:		(cjd)	Chris De Salvo	Change History (most recent first):	   <SP1>	 1/29/99	cjd		first checked in*///₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	₯//₯//₯	Copyright © 1997 Apple Computer, Inc., All Rights Reserved//₯//₯//₯		You may incorporate this sample code into your applications without//₯		restriction, though the sample code has been provided "AS IS" and the//₯		responsibility for its operation is 100% yours.  However, what you are//₯		not permitted to do is to redistribute the source as "DSC Sample Code"//₯		after having made changes. If you're going to re-distribute the source,//₯		we require that you make it clear in the source that the code was//₯		descended from Apple Sample Code, but that you've made changes.//₯//₯		Authors://₯			Chris De Salvo		<mailto:desalvo@apple.com>//₯//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	₯//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	Includes#include <Devices.h>#include <Errors.h>#include <MacTypes.h>#include <StringCompare.h>#include <ToolUtils.h>#include <string.h>					//₯	For memset()#include "CD_Utils.h"//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	Private Definitions#define kCDDriverName				"\p.AppleCD"//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	Private Types//₯	CD Driver csCodesenum{	csDriveStatus				=	8,	csReadTOC					=	100,	csReadQ						=	101,	csAudioTrackSearch			=	103,	csPlayAudio					=	104,	csAudioStop					=	106,	csAudioStatus				=	107,	csAudioVolume				=	109,	csGetSpindleSpeed			=	113};//₯	CD addressing typesenum{	addressLBS,	addressMIN_SEC_FRAME,	addressTrack};#pragma options align=mac68ktypedef struct AudioStatusRec{	QElemPtr	qLink;	SInt16		qType;	SInt16		ioTrap;	Ptr			ioCmdAddr;	ProcPtr		ioCompletion;	OSErr		ioResult;	StringPtr	ioNamePtr;	SInt16		ioVRefNum;	SInt16		ioRefNum;	SInt16		csCode;	struct	{		Byte	audioStatus;		Byte	playMode;		Byte	cntlField;		Byte	minutes;		Byte	seconds;		Byte	frames;	} csParam;} AudioStatusRec, *AudioStatusRecPtr;#pragma options align=reset//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	Private Variablesstatic Boolean	gCDInited = false;static SInt16	gCDDriverRef = 0;static SInt16	gCurrentTrack = 0;static ParamBlockRec	gPlayParamBlock;static ParamBlockRec	gStopParamBlock;//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	Private Functionsstatic UInt32 NumToBCD(UInt16 theNum);static void CD_ClearParamBlock(ParamBlockRec *inPB);//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	Public Variables//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_OpenOSErrCD_Open(void){OSErr	theErr;	theErr = OpenDriver(kCDDriverName, &gCDDriverRef);	if (noErr == theErr)		gCDInited = true;	//₯	Zero this out so that the ioResult field is inited	CD_ClearParamBlock(&gPlayParamBlock);			return (theErr);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_GetSpeedSInt16CD_GetSpeed(void){OSErr			theErr;ParamBlockRec	thePB;	if (false == gCDInited)		return (0);	CD_ClearParamBlock(&thePB);	thePB.cntrlParam.ioCRefNum = gCDDriverRef;	thePB.cntrlParam.csCode = csGetSpindleSpeed;		theErr = PBControlSync(&thePB);	if (noErr != theErr)		return (-1);	return (thePB.cntrlParam.csParam[0]);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_PlayAudioTrackOSErrCD_PlayAudioTrack(UInt8 trackNumber, UInt8 playMode, Boolean inAsync){ParamBlockRec	*thePB = &gPlayParamBlock;	if (false == gCDInited)		return (paramErr);	CD_ClearParamBlock(thePB);	gCurrentTrack = trackNumber;	thePB->cntrlParam.ioCRefNum = gCDDriverRef;						//₯	CD driver ref num	thePB->cntrlParam.csCode = csPlayAudio;							//₯	csCode to seek and play audio selection		thePB->cntrlParam.csParam[0] = addressTrack;					//₯	Use track number addressing	thePB->cntrlParam.csParam[1] = 0;								//₯	High word of Track in BCD	thePB->cntrlParam.csParam[2] = LoWord(NumToBCD(trackNumber));	//₯	Low word of Track in BCD	thePB->cntrlParam.csParam[3] = 0;								//₯	This is a start position	thePB->cntrlParam.csParam[4] = playMode;		if (true == inAsync)	{		CD_WaitOnPlayAsyncCalls();		return (PBControlAsync(thePB));	}	return (PBControlSync(thePB));}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_StopAudioTrackOSErrCD_StopAudioTrack(Boolean inAsync){ParamBlockRec	*thePB = &gStopParamBlock;	if (false == gCDInited)		return (paramErr);	CD_ClearParamBlock(thePB);		thePB->cntrlParam.ioCRefNum = gCDDriverRef;						//₯	CD driver ref num	thePB->cntrlParam.csCode = csAudioStop;		if (true == inAsync)	{		CD_WaitOnStopAsyncCalls();		return (PBControlAsync(thePB));	}	return (PBControlSync(thePB));}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	NumToBCDstatic UInt32NumToBCD(UInt16 theNum){UInt32	theBCD;	theBCD = 0x0000;		//₯	Get the digits	theBCD |= theNum % 10;	theNum /= 10;	//₯	Get the tens	theBCD |= (theNum % 10) << 4;	theNum /= 10;		//₯	Get the hundreds	theBCD |= (theNum % 10) << 8;	theNum /= 10;		//₯	Get the thousands	theBCD |= (theNum % 10) << 12;		return (theBCD);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	BCDToNumstatic SInt16BCDToNum(UInt8 bcd){UInt8	place;SInt16	num = 0;		place = bcd & 0x0F;									//₯	Get the first 10 bits	num += place;		place = (bcd >> 4) & 0x0F;	num += (place * 10);		return (num);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_GetCurrentAudioTrackOSErrCD_GetCurrentAudioTrack(SInt16 *theTrack){OSErr			theErr;ParamBlockRec	thePB;	if (false == gCDInited)		return (paramErr);	CD_ClearParamBlock(&thePB);		thePB.cntrlParam.ioCRefNum = gCDDriverRef;				//₯	CD driver ref num	thePB.cntrlParam.csCode = csReadQ;						//₯	Get the position of the optical pickup		theErr = PBControlSync(&thePB);		*theTrack =  BCDToNum(thePB.cntrlParam.csParam[0] & 0xFF);	//₯	Return the current track number	return (theErr);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_IsFileOnCDIsOnCDResultCD_IsFileOnCD(SInt16 fileRef){HParamBlockRec	volRec;DCtlHandle		devHandle;FCBPBRec		fileRec;OSErr			theErr;StringPtr		driverName;StringPtr		cdDriverName = kCDDriverName;	//₯	Get information on this open file	fileRec.ioCompletion = nil;	fileRec.ioFCBIndx = 0;	fileRec.ioVRefNum = 0;	fileRec.ioRefNum = fileRef;	fileRec.ioNamePtr = nil;	theErr = PBGetFCBInfoSync(&fileRec);	if (noErr != theErr)		return (isOnCDErr);	//₯	Get information about the volume of this file	volRec.volumeParam.ioCompletion = nil;	volRec.volumeParam.ioNamePtr = nil;	volRec.volumeParam.ioVRefNum = fileRec.ioFCBVRefNum;	volRec.volumeParam.ioVolIndex = 0;		theErr = PBHGetVInfoSync(&volRec);	if (noErr != theErr)		return (isOnCDErr);	//₯	Get info on this drive's driver	devHandle = GetDCtlEntry(volRec.volumeParam.ioVDRefNum);	if (nil == devHandle)		return (isOnCDErr);			if ((**devHandle).dCtlFlags & (1 << dRAMBased))	{	Handle	driver;			driver = (Handle) (**devHandle).dCtlDriver;		driverName = (StringPtr) (*driver + 18);	}	else	{		driverName = (StringPtr) ((**devHandle).dCtlDriver + 18);	}			if (EqualString(driverName, cdDriverName, false, false) == true)		return (isOnCDTrue);	return (isOnCDFalse);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_IsCDInsertedBooleanCD_IsCDInserted(void){ParamBlockRec	params;OSErr			theErr;		if (false == gCDInited)		return (false);	CD_ClearParamBlock(&params);	//₯	On return from a drive status call the csParam fields have the same info	//₯	as the regular disk driver DrvSts structure.  The fourth byte in that	//₯	structure is true or false depending on whether or not a disk is in the drive.	params.cntrlParam.ioVRefNum = 1;	params.cntrlParam.ioCRefNum = gCDDriverRef;	params.cntrlParam.csCode = csDriveStatus;		theErr = PBStatusSync((ParmBlkPtr) &params);	if (noErr != theErr)		return (false);		return (params.cntrlParam.csParam[1] & 0xFF);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_GetNumTracksSInt16CD_GetNumTracks(void){ParamBlockRec	params;OSErr			theErr;UInt8			lastTrack;		if (false == gCDInited)		return (0);	CD_ClearParamBlock(&params);		params.cntrlParam.ioVRefNum = 1;	params.cntrlParam.ioCRefNum = gCDDriverRef;	params.cntrlParam.csCode = csReadTOC;	params.cntrlParam.csParam[0] = 1;			//₯	Type 1 TOC lookup marker	theErr = PBControlSync(&params);	if (noErr != theErr)		return (0);	lastTrack = params.cntrlParam.csParam[0] & 0xFF;	//₯	Return the number of tracks in decimal instead of BCD form	return (BCDToNum(lastTrack));}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_IsCDPlayingBooleanCD_IsCDPlaying(void){ParamBlockRec	params;AudioStatusRec	*rec;OSErr			theErr;		if (false == gCDInited)		return (false);	params.cntrlParam.ioCompletion = nil;	params.cntrlParam.ioVRefNum = 1;	params.cntrlParam.ioCRefNum = gCDDriverRef;	params.cntrlParam.csCode = csAudioStatus;	theErr = PBControlSync((ParmBlkPtr) &params);	if (noErr != theErr)		return (theErr);	rec = (AudioStatusRec *) &params;	if (rec->csParam.audioStatus == 0)		return (true);	return (false);}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_SetVolumeOSErrCD_SetVolume(UInt8 left, UInt8 right){ParamBlockRec	thePB;UInt16	volume;	if (false == gCDInited)		return (paramErr);	CD_ClearParamBlock(&thePB);		thePB.cntrlParam.ioCRefNum = gCDDriverRef;		//₯	CD driver ref num	thePB.cntrlParam.csCode = csAudioVolume;	volume = (left << 8) | right;	thePB.cntrlParam.csParam[0] = volume;		return (PBControlSync(&thePB));}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_EjectvoidCD_Eject(void){OSErr			theErr;Str255			ioName;HVolumeParam	params;short			driverRef;	memset(&params, 0, sizeof (params));	params.ioNamePtr = ioName;	params.ioVolIndex = 0;	driverRef = gCDDriverRef;	do	{		params.ioVolIndex++;				theErr = PBHGetVInfoSync((HParmBlkPtr) &params);		if (theErr)			return;	} while (params.ioVDRefNum != driverRef);	theErr = PBUnmountVol((ParmBlkPtr) &params);	if (noErr == theErr)	{		params.ioVRefNum = params.ioVDrvInfo;		//₯	get the drive number		theErr = PBEject((ParmBlkPtr) &params);	}}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_ClearParamBlockstatic voidCD_ClearParamBlock(ParamBlockRec *inPB){	memset(inPB, 0, sizeof (ParamBlockRec));}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_WaitOnPlayAsycCallsvoidCD_WaitOnPlayAsyncCalls(void){	while (1 == gPlayParamBlock.cntrlParam.ioResult)	{	}}//₯	ΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡΡ	CD_WaitOnStopAsycCallsvoidCD_WaitOnStopAsyncCalls(void){	while (1 == gStopParamBlock.cntrlParam.ioResult)	{	}}