#pragma once

#include "Helper.h"
#include "Layout.h"

//Serializes values by layout. Returns bytes of serialized data.
char* Serialize(const Layout* layout, Value value);