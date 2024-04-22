#pragma once

#include "Helper.h"
#include "Layout.h"

//Deserializes bytes by layout. Returns constructed value.
Value Deserialize(const Layout *layout, char *buffer);