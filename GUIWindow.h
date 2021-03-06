//
//  GUIWindow.h
//  Deep
//
//  Created by Nathan Daly on 9/18/12.
//  Copyright (c) 2012 Lions Entertainment. All rights reserved.
//

#ifndef GUIWINDOW_H
#define GUIWINDOW_H


#include <string>

#include "GUIUtility.h"
#include "Compatibility.h"
		 			 

#include SDL_SDLVIDEO_H
#include SDL_SDLKEYBOARD_H

class SDL_Surface;

namespace GUI {

struct Unhandled_Click {
    
    Unhandled_Click(DispPoint coord_) : coord(coord_) { }
    DispPoint coord;
};
struct Unhandled_Key {
    
    Unhandled_Key(SDL_keysym key_) : key(key_) { }
    SDL_keysym key;
};


class View;

// This class maintains information about the Window. 
// (i.e name, size, etc.) Has one View instance that shows what is displayed in
// the window.
class Window {
public:

    Window(int w, int h, const std::string& name_ = "",
                 int numColors = 256, const Uint32 flags = SDL_SWSURFACE);
    
    void resize(int w, int h);
    void stretch(int w, int h);
    
    void rename(const std::string& name_);
    
    View* get_main_view() { return main_view; }

    DispPoint get_dim();
        
    // Handle subviews to main_view:
    void attach_subview(View* view, DispPoint pos);
    void move_subview(View* view, DispPoint pos);
    void remove_subview(View* view);
    View* remove_last_subview(); // Remove subview last added

    
    // If main_view has changed at all, re-display it.
    void refresh();
    
private:
    SDL_Surface* window;
    View* main_view;
    std::string name;
    
    int num_colors;
    Uint32 flags;
    
    static bool WINDOW_ALREADY_CREATED;
};

} // namespace GUI

#endif /* GUIWINDOW_H */
