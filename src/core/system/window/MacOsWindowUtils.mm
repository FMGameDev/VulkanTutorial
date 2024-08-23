#include <Cocoa/Cocoa.h>
#include <QuartzCore/CAMetalLayer.h>

void makeViewMetalCompatible(void* handle) {
    // Convert void* to NSView*
    NSView* view = (__bridge NSView*)handle;
    
    // Verify that the handle points to an NSView
    assert([view isKindOfClass:[NSView class]]);
    
    // Check if the layer is not a CAMetalLayer, then create one
    if (![view.layer isKindOfClass:[CAMetalLayer class]]) {
        [view setLayer:[CAMetalLayer layer]];
        [view setWantsLayer:YES];
    }
}

void* getViewFromNSWindowPointer(void* window) {
    // Convert void* to NSWindow*
    NSWindow* ns_window = (__bridge NSWindow*) window;
    
    // Retrieve the content view of the window and make it Metal-compatible
    void* viewHandle = (__bridge void*) [ns_window contentView];
    makeViewMetalCompatible(viewHandle);
    
    return viewHandle;
}

void* getMetalLayerFromView(void* viewHandle) {
    // Convert void* to NSView*
    NSView* view = (__bridge NSView*)viewHandle;
    
    // Return the CAMetalLayer associated with the view
    return (__bridge void*)[view layer];
}
