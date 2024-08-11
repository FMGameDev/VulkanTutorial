//
//  ObjC-interface.h
//  VulkanTutorial
//
//  Created by Francisco Diaz on 11/08/2024.
//

#ifndef ObjC_interface_h
#define ObjC_interface_h

#ifdef __cplusplus
extern "C" {
#endif

    void* getViewFromNSWindowPointer(void* window);
    void makeViewMetalCompatible(void* handle);
    CAMetalLayer* getMetalLayerFromView(id viewHandle);

#ifdef __cplusplus
}
#endif

#endif /* ObjC_interface_h */
