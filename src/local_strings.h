#pragma once

#define LOCAL_STRINGS \
LOCAL_STR(ITEM_NAME_APPLE) \
LOCAL_STR(ITEM_NAME_SWORD) \
LOCAL_STR(ITEM_NAME_SCROLL) \
LOCAL_STR(DIALOG_HELLO) \


enum {
	STR_NULL,
#define LOCAL_STR(x) STR_##x,
LOCAL_STRINGS
#undef LOCAL_STR
	MAX_STR_INDEX,
};

extern const char* local_strings[MAX_STR_INDEX];

using StrHandle = GlobalHandle<const char*, local_strings, false>;
