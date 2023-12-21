// Copyright (C) 2023 owoDra

#pragma once

#include "Logging/LogMacros.h"

GIEXT_API DECLARE_LOG_CATEGORY_EXTERN(LogGIE, Log, All);

#if !UE_BUILD_SHIPPING

#define GIELOG(FormattedText, ...) UE_LOG(LogGIE, Log, FormattedText, __VA_ARGS__)

#define GIEENSURE(InExpression) ensure(InExpression)
#define GIEENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GIEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GIECHECK(InExpression) check(InExpression)
#define GIECHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GIELOG(FormattedText, ...)

#define GIEENSURE(InExpression) InExpression
#define GIEENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GIEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GIECHECK(InExpression) InExpression
#define GIECHECK_MSG(InExpression, InFormat, ...) InExpression

#endif