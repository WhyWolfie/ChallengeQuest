/////////////////////////////////////////////////////////////
// cxr_MMatchBuff.h
//
// This file was generated by CXR, the literal string encryptor.
// CXR, Copyright 2002, Smaller Animals Software, Inc., all rights reserved.
//
// Please do not edit this file. Any changes here will be overwritten on the next compile.
// If you wish to make changes to a string, please edit:
//     MMatchBuff.cxr
//

/////////////////////////////////////////////////////////////

#pragma once
#include "cxr_inc.h"

/*
	== XML 기술 설명 ==
	
	+ zBuff.xml 설명	
	+ attribute 설명
		BuffID			: 버프 아이디						(int)
		BuffName		: 버프 이름							(string[128])
		BuffIconName	: 버프 아이콘 이름					(string[128])
		
		BuffType		: 버프 타입			(MMatchBuffDesc.h내의 MMatchBuffType 참고)
		BuffPeriodType	: 버프 기간 타입	(MMatchBuffDesc.h내의 MMatchBuffPeriodType 참고)
		BuffPeirod		: 버프 기간			(초 단위)
		
		HP	:
		AP	:
		XP	:		
*/

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_BUFF	_CXR("\x8e\x81\x8f\x83\x8c\x8c\x8d\x81\x83\x8f")
#else
#define MICTOK_BUFF														_CXR("BUFF")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_ID	_CXR("\x85\x8d\x87\x80\x86\x87")
#else
#define MICTOK_ATTRNAME_BUFF_ID											_CXR("ID")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_ICON_NAME	_CXR("\x85\x8c\x8d\x88\x87\x82\x84\x81\x88\x84\x82\x8e\x8b\x83\x87\x81\x89\x84")
#else
#define MICTOK_ATTRNAME_BUFF_ICON_NAME									_CXR("IconName")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_PERIOD_TYPE	_CXR("\x89\x89\x8d\x8f\x8f\x8c\x83\x86\x85\x87\x8f\x8c\x89\x88\x8b\x8a\x89\x82\x84\x8e\x89\x81")
#else
#define MICTOK_ATTRNAME_BUFF_PERIOD_TYPE								_CXR("PeriodType")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_PERIOD	_CXR("\x88\x8a\x86\x88\x84\x8b\x89\x80\x88\x83\x81\x87\x8d\x84")
#else
#define MICTOK_ATTRNAME_BUFF_PERIOD										_CXR("Period")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_EFFECT_TYPE	_CXR("\x8c\x8f\x84\x8f\x84\x84\x89\x8e\x86\x83\x80\x82\x85\x81\x8e\x82\x80\x80\x87\x8e\x86\x8e")
#else
#define MICTOK_ATTRNAME_BUFF_EFFECT_TYPE								_CXR("EffectType")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_HP_VALUE	_CXR("\x88\x85\x82\x86\x8f\x8b")
#else
#define MICTOK_ATTRNAME_BUFF_HP_VALUE									_CXR("HP")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_AP_VALUE	_CXR("\x86\x81\x86\x86\x83\x80")
#else
#define MICTOK_ATTRNAME_BUFF_AP_VALUE									_CXR("AP")
#endif


///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRNAME_BUFF_RESPAWN_DECREASE_TIME	_CXR("\x82\x82\x8b\x81\x8e\x80\x88\x80\x88\x83\x87\x85\x88\x89\x89\x85")
#else
#define MICTOK_ATTRNAME_BUFF_RESPAWN_DECREASE_TIME						_CXR("Respawn")
#endif


///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRVAL_PERIOD_TYPE_SHORT	_CXR("\x8f\x80\x84\x80\x8b\x83\x86\x86\x82\x88\x8d\x8b")
#else
#define MICTOK_ATTRVAL_PERIOD_TYPE_SHORT								_CXR("short")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRVAL_PERIOD_TYPE_LONG	_CXR("\x82\x85\x89\x84\x85\x80\x8b\x85\x87\x80")
#else
#define MICTOK_ATTRVAL_PERIOD_TYPE_LONG									_CXR("long")
#endif


///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRVAL_BUFF_EFFECT_TYPE_NORMAL	_CXR("\x83\x8d\x8a\x8d\x84\x8c\x85\x80\x82\x85\x8c\x83\x8d\x88")
#else
#define MICTOK_ATTRVAL_BUFF_EFFECT_TYPE_NORMAL							_CXR("normal")
#endif

///////////////////////////
#ifdef _USING_CXR
#define MICTOK_ATTRVAL_BUFF_EFFECT_TYPE_DOTE	_CXR("\x81\x8d\x81\x81\x81\x84\x8e\x89\x80\x80")
#else
#define MICTOK_ATTRVAL_BUFF_EFFECT_TYPE_DOTE							_CXR("dote")
#endif

