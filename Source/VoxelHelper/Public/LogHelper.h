#pragma once
#include "Kismet/KismetSystemLibrary.h"
#define LOG_DEFAULT_CATEGORY LogTemp


// dont support   __VA_OPT__
//#define LogWarning(format,...) UE_LOG(LOG_DEFAULT_CATEGORY, Warning, TEXT("[%hs(%d):{%hs}]"#format),__FILE__,__LINE__,__FUNCTION__, __VA_OPT__(,)__VA_ARGS__);


// Logs
#define LogFatal(format,...)		UE_LOG(LOG_DEFAULT_CATEGORY, Fatal,		TEXT("{%hs}\t") TEXT(format) TEXT("\t\t\t\t\t{%hs(%d)}"),__FUNCTION__ ,##__VA_ARGS__,__FILE__,__LINE__);
#define LogError(format,...)		UE_LOG(LOG_DEFAULT_CATEGORY, Error,		TEXT("{%hs}\t") TEXT(format) TEXT("\t\t\t\t\t{%hs(%d)}"),__FUNCTION__ ,##__VA_ARGS__,__FILE__,__LINE__);
#define LogWarning(format,...)		UE_LOG(LOG_DEFAULT_CATEGORY, Warning,	TEXT("{%hs}\t") TEXT(format) TEXT("\t\t\t\t\t{%hs(%d)}"),__FUNCTION__ ,##__VA_ARGS__,__FILE__,__LINE__);
#define LogDisplay(format,...)		UE_LOG(LOG_DEFAULT_CATEGORY, Display,	TEXT("{%hs}\t") TEXT(format) TEXT("\t\t\t\t\t{%hs(%d)}"),__FUNCTION__ ,##__VA_ARGS__,__FILE__,__LINE__);
#define LogDefault(format,...)		UE_LOG(LOG_DEFAULT_CATEGORY, Log,		TEXT("{%hs}\t") TEXT(format) TEXT("\t\t\t\t\t{%hs(%d)}"),__FUNCTION__ ,##__VA_ARGS__,__FILE__,__LINE__);
#define LogVerbose(format,...)		UE_LOG(LOG_DEFAULT_CATEGORY, Verbose,	TEXT("{%hs}\t") TEXT(format) TEXT("\t\t\t\t\t{%hs(%d)}"),__FUNCTION__ ,##__VA_ARGS__,__FILE__,__LINE__);


// Asserts
#define AssertLogCheck(expr,format,...)			checkf(expr,TEXT("{%hs}\t\t\t")TEXT(format),				__FUNCTION__,##__VA_ARGS__);  // now work in shipping build , crash when expr is false
#define AssertLogVerify(expr,format,...)		verifyf(expr,TEXT("{%hs}\t\t\t")TEXT(format),				__FUNCTION__,##__VA_ARGS__);  // used when expr is a func's return value , check() dont exec this func on shipping build
#define AssertLogEnsure(expr,format,...)		ensureMsgf(expr,TEXT("{%hs}\t\t\t")TEXT(format),			__FUNCTION__,##__VA_ARGS__);  // like verify , no crash , only exec once
#define AssertLogEnsureA(expr,format,...)		ensureAlwaysMsgf(expr,TEXT("{%hs}\t\t\t")TEXT(format),	__FUNCTION__,##__VA_ARGS__);  // like verify , no crash ,  exec always

// Screen Logs
#define LogScreenOnly(time,format,...)					UKismetSystemLibrary::PrintString(GetWorld(),FString::Printf(TEXT(format),##__VA_ARGS__),true,false,FLinearColor::Red,time);	// print on screen only
#define LogScreenOnly_Color(time,color,format,...)		UKismetSystemLibrary::PrintString(GetWorld(),FString::Printf(TEXT(format),##__VA_ARGS__),true,false,FLinearColor##color,time);	// print on screen only with color
#define LogScreen(time,format,...)						UKismetSystemLibrary::PrintString(GetWorld(),FString::Printf(TEXT(format),##__VA_ARGS__),true,true,FLinearColor::Red,time);	// print on screen + log
#define LogScreen_Color(time,color,format,...)			UKismetSystemLibrary::PrintString(GetWorld(),FString::Printf(TEXT(format),##__VA_ARGS__),true,true,FLinearColor##color,time);	// print on screen + log with color

