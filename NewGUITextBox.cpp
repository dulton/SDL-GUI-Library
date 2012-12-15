

#include "NewGUITextBox.h"
#include "NewGUIImageView.h"
#include "NewGUIApp.h"

#include "GameDisplay.h"
#include "GUIWin_Ctrl.h"
#include "GUIView.h"
#include "GUIImage.h"

#include "GUILetter.h"

#include <iostream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <tr1/functional>
using std::cout; using std::endl;
using std::string;
using std::clock;
using std::vector;
using std::for_each;
using std::tr1::bind;
using namespace std::tr1::placeholders;


static char SDL_to_a(SDLKey key);

const SDL_Color bg_color_c = {255,255,255,0};
const SDL_Color default_color_c = {0,0,0,0};


NewGUITextView::NewGUITextView(int w_, int h_)
:NewGUIView(w_,h_),
text_size(30), color(default_color_c)
{
    set_clear_color(bg_color_c);
            
    update();
    
}

void NewGUITextView::update(){
	
    GUIImage bg = GUIImage::create_blank(get_w(), get_h());
    SDL_FillRect(bg, 0, SDL_MapRGB(bg->format, bg_color_c.r, bg_color_c.g, bg_color_c.b));
    draw_onto_self(bg, DispPoint());
    
	for (int i = 0; i < letters.size(); i++){
		letters[i].set_pos(pos_at_index(i));
	}
    
    for (int i = 0; i < letters.size(); i++){
        letters[i].drawself(this);
	}
    
	mark_changed();
}
void NewGUITextView::clear(){
	letters.clear();
}



void NewGUITextView::set_text(const string& text) {
    clear();
    
    for (int i = 0; i < text.length(); ++i) {
        add_letter(text[i], i);
    };
}
string NewGUITextView::get_text() const {
	string text;
	for (letters_ctr_t::const_iterator it = letters.begin(); it != letters.end(); ++it){
		text += it->get_ltr();
	}
	return text;	
}


void NewGUITextView::add_letter(char ltr, int index){
    
    cout << "ADDING LETTER: '" << ltr << "'" << endl;
	
	try{
		letters.insert(letters.begin()+index, NewLetter_Disp_Obj(ltr, text_size, pos_at_index(index), color));
        cout << "letter: '" << letters[index].get_ltr() << "'\n";
        cout << "width: " << letters[index].get_width() << "\n";
	}
	catch(const Error& e){
		cout << e.msg << endl;
	}
	update();
}
void NewGUITextView::remove_letter(int index){
	if (index < 0) return;
	if (index > letters.size()) return;
	
	letters.erase(letters.begin()+index);
	update();
}



DispPoint NewGUITextView::pos_at_index(size_t i){
	
	DispPoint position;
	if (i == 0){
		position = DispPoint(0,0);
	}
	else {
		const NewLetter_Disp_Obj& letter = letters[i-1];
		int width = letter.get_width();
		position = DispPoint(letter.get_pos().x + width + 1, 
							 letter.get_pos().y);
		if (position.x + width >= get_w()){
			position.x = 0;
            //			position.y += NewLetter_Disp_Obj::get_line_height();
			position.y += text_size;
		}
		
	}
	return position;
}
int NewGUITextView::index_at_pos(DispPoint pos_){
	if (letters.empty()){
		return 0;
	}
	if (pos_.x < letters[0].get_pos().x || pos_.y < letters[0].get_pos().y){
		return 0;
	}
	int i;
	for (i = 0; i < letters.size(); i++) {
		if ((pos_.x >= letters[i].get_pos().x 
			 && pos_.x < letters[i].get_pos().x + letters[i].get_width()/2) 
			&& (pos_.y >= letters[i].get_pos().y 
				&& pos_.y < letters[i].get_pos().y + NewLetter_Disp_Obj::get_line_height())) {
				
				break;
			}		
		
		else if (i > 0 && ((pos_.x >= letters[i-1].get_pos().x + letters[i-1].get_width()/2
							&& pos_.x < letters[i-1].get_pos().x + letters[i-1].get_width()) 
                           && (pos_.y >= letters[i-1].get_pos().y 
                               && pos_.y < letters[i-1].get_pos().y + NewLetter_Disp_Obj::get_line_height()))) 
		{
            
			break;
		}		
	}
	return i;
}



NewGUITextBox::NewGUITextBox(int w_, int h_)
:NewGUITextView(w_,h_), cursor(this),
key_is_held(false)
{        
    attach_subview(&cursor, DispPoint());
    
    NewGUIApp::get()->repeat_on_timer(bind(&NewGUITextBox::handle_key_held, this), 0.05);
}

#include "NewGUIApp.h"
bool NewGUITextBox::handle_key_down(SDL_keysym key_in) {
    
    cout << "text box key down" << endl;
        
    key_is_held = true;
    key_held = key_in.sym;
    time_key_held.reset();

    handle_key();

    return true;
}

bool NewGUITextBox::handle_key_up(SDL_keysym key_in) {
    
    cout << "text box key up" << endl;

    key_is_held = false;
    time_key_held.stop();

    return false;
}
void NewGUITextBox::handle_key_held() {
    
    if (!key_is_held) return;
    if (!(time_key_held.get_time() > 500)) return; 
    
    handle_key();
}

void NewGUITextBox::handle_key() {
 	
    SDLKey key = key_held;

    switch (key) {
		case SDLK_LEFT:
			cursor.move_left();
			break;
		case SDLK_RIGHT:
			cursor.move_right();
			break;
		case SDLK_UP:
			cursor.move_up();
			break;
		case SDLK_DOWN:
			cursor.move_down();
			break;
			
		case SDLK_BACKSPACE:
			remove_letter(cursor.get_index()-1);
			cursor.move_left();
			break;
			
		default:
			handle_alpha_num(SDL_to_a(key));
			break;
	}
}



void NewGUITextBox::handle_alpha_num(char ltr){
		
	int index = cursor.get_index();
	add_letter(ltr, index);
	
	cursor.move_right();
}

bool NewGUITextBox::handle_mouse_down(DispPoint pos_){
	
    if (!rel_point_is_on_me(pos_)) {
        lose_focus();
       
        return false;
    }
    
	cursor.move_to(index_at_pos(pos_));

    capture_focus();
    
    return true;
}

#include <cctype>

static char SDL_to_a(SDLKey key){
	
	if (key >= 0xFF) {
		return 0;
	}
	return toupper(static_cast<char>(key));
}

void NewGUITextBox::lost_focus() {
    cout << "LOSTTTTT FOCUSSSSS" << endl;
    move_subview(&cursor, DispPoint(-100,-100));
    update();
}

void NewGUITextBox::Cursor::move_right(){
	if (index == text_box_ptr->get_letters().size()) return;
	move_to(index + 1);
}
void NewGUITextBox::Cursor::move_left(){
	if (index == 0) return;
	move_to(index - 1);
}
void NewGUITextBox::Cursor::move_up(){
	
	DispPoint new_pos = position;
	new_pos.y -= NewLetter_Disp_Obj::get_line_height();
	
	int new_index = text_box_ptr->index_at_pos(new_pos);
	
	move_to(new_index);
}
void NewGUITextBox::Cursor::move_down(){
	DispPoint new_pos = position;
	new_pos.y += NewLetter_Disp_Obj::get_line_height();
	
	int new_index = text_box_ptr->index_at_pos(new_pos);
	
	move_to(new_index);
}

void NewGUITextBox::Cursor::move_to(int index_) {
	
	if (index_ < 0 || index_ > text_box_ptr->get_letters().size()) return;
	
	index = index_;
	
	position = text_box_ptr->pos_at_index(index);

    text_box_ptr->move_subview(this, position);
}

void NewGUITextBox::Cursor::display(int text_size) {
	if (std::clock() % 300000 >= 150000){
		flicker = 0;
	}
	else flicker = 1;
	
	SDL_Rect box = {0,0,1,text_size};
	
	GUIImage* image = GUIImage::get_image( string("images/cursor") + (flicker ? "1" : "0") + string(".bmp"));
	draw_onto_self(*image, position);
    //	ShowBMP("images/fireball.bmp", screen, position.x, position.y, 0);
}






int NewLetter_Disp_Obj::line_height = 0;


NewLetter_Disp_Obj::NewLetter_Disp_Obj(char ltr, int size, DispPoint pos, SDL_Color color)
:position(pos), ltr(ltr), height(size)
{
	letter = GUILetter::get_letter(ltr, size, color);
	if (!letter) {
		string error_msg = string("Couldn't load image for letter") + ltr + string(": ")+ SDL_GetError() +"\n";
		cout << error_msg << endl;
		throw Error(error_msg.c_str());
	}
	line_height = letter->get_height()+2; // for a line spacing
}

int NewLetter_Disp_Obj::get_width() const{
	return letter->get_width();
}

void NewLetter_Disp_Obj::drawself(NewGUIView *dest) const{
	
	letter->drawself(dest, position.x, position.y + height-letter->get_height());
}
