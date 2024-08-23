#pragma once

void *getViewFromNSWindowPointer(void *window);
void makeViewMetalCompatible(void *handle);
void *getMetalLayerFromView(void *viewHandle);
