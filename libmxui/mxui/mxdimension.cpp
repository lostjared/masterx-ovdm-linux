#include "mxdimension.h"
#include<functional>
#include<cassert>
#include<mxserver.h>
#include<mxpng.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/dir.h>
#include<vector>


namespace mxui {

	using mx::mxPainter;
	using mx::mxPng;

	unsigned int Dimension::dim_offset = rand()%100;

		Dimension::Dimension()
		{
			my_offset = dim_offset++;
			wnd = (mxWnd*)mX::server;
			Size size =  wnd->size();

			ident_offset=1;
			background.createSurface(size);
			background_image.createSurface(size);
			setBackgroundColor( Color(150,150,150) );
			background_image_on = false;
			name = "Demo";
			focus_window = 0;
			first_run = 0;
			movement_window = 0;
			destroy = false;
		}



		Dimension::Dimension(string n, mxWnd *p_wnd, Size size) : wnd(p_wnd)
		{
			ident_offset=1;
			my_offset = dim_offset++;
			background.createSurface(size);
			background_image.createSurface(size);
			setBackgroundColor( Color(150,150,150) );
			background_image_on = false;
			name = n;
			focus_window = 0;
			first_run = 0;
			movement_window = 0;
			windows.list_id=n;
			destroy = false;

		}


		Window *Dimension::window(int id)   {
			Window *w = findWindow(id);
			if(w == 0) throw mx::mxException<string>("[ mxui ]: error window id is not valid, possible typeo or object destoryed?\n");
			return w;
		}


		Dimension::~Dimension()
		{
			clearDimension();
		}

		void Dimension::clearDimension()
		{

		}


		// Dimension is repsonsible for Windows passed to it
		// do not free windows passed to Dimensions
	    int Dimension::addWindow(Window *wndx)
	    {
	    	wndx->setID(newID());
	    	wndx->setMax(wnd->size().width, wnd->size().height);
	    	clearMove(wndx);
	       	windows.append(wndx);
	       	focus_window = wndx;
	       	return wndx->getID();
	    }



	    void Dimension::shade_window()
	    {


	    }

	    void Dimension::restoreAll()
	    {

	    	if(windows.count() == 1) {
	    		windows.rootptr()->item->setWindowState(NORMAL);
	    		return;
	    	}

	    	listItem<Window *> *type = windows.rootptr(), **typeptr = &type;

	    	while (  (type = *typeptr)  != 0) {

	    		if(type->item->caption() != "")
	    			type->item->setWindowState(NORMAL);
	    		typeptr = &type->next;
	    	}

	    	if(windows.count() > 0)
	    	setFocus(windows.tailptr()->item->getID() );
	    }

		void Dimension::minimizeAll()
		{

			listItem<Window *> *item = windows.rootptr(), **ptr = &item;

			while( (item = *ptr) != 0 ) {
				Window *w = item->item;
				assert( w != 0);
				if(w->caption() != "")
				w->setWindowState(MINI);
			    ptr = &item->next;
			}
		}

		void Dimension::drawScreen(mxSurface &drawScreen)
		{
			drawScreen.copySurface(background, 0, 0);
		}

		void Dimension::render()
		{
			logic();

			mxPainter painter(background);
			painter.fillRect(Rect(0,0, background.width(), background.height()), backcolor);

			if(background_image_on == true) {
				background.copySurface(background_image, 0, 0);
			}

			listItem<Window *> *item = windows.rootptr(), **ptr = &item;

			while(( item = *ptr ) != 0)
			{
				Window *w = item->item;

				if(w->state() != MINI && w->Visible() == true) {
					w->renderEvent();
					w->copyWidgetSurface(background);
				}

				if(w->moving() == true) {

					painter.lock();
					painter.drawSquare(w->movement_rect);
					painter.unlock();
				}
				ptr = &item->next;
			}

			after_render();
		}


		void Dimension::eventProc(widgetAction &we)
		{



			listItem<Window *> *item = windows.tailptr(), **ptr = &item;

			while ( (item = *ptr) != 0)  {

				Window *w = item->item;

				int ac = w->procEvent(we);

				if(ac == MSG_IGNORED) continue;
				if(ac == MSG_PROC) break;
				if(ac == MSG_REMOVE)
				{
				//	windows.erase(i);
					continue;
				}

				ptr = &item->prev;
			}


		}

		void Dimension::keyDown(int key, void *data)
		{
			if(windows.count() < 1) return;


			unsigned char *keys = SDL_GetKeyState(0);

			if(keys[SDLK_LALT] && keys[SDLK_LCTRL] && keys[SDLK_UP]) {
				restoreAll();
				return;
			}

			if(keys[SDLK_LALT] && keys[SDLK_LCTRL]  && keys[SDLK_DOWN])  {
				minimizeAll();
				return;
			}


			listItem<Window *> *item = windows.tailptr(), **ptr = &item;

			while ( (item = *ptr) != 0 ) {
				Window *w = item->item;
				if( w->keyDown(key,data) == MSG_PROC ) return;

				ptr = &item->prev;

			}
		}

		void Dimension::keyUp(int key)
		{

			/** Shift Window Focus with Left Alt and Left Shift
			 *  Dimension default calls  uiList_ptr shift_right
			 **/

			if(windows.count() < 1) return;


			unsigned char *keys = SDL_GetKeyState(0);

			if( ( keys[SDLK_LALT] ) && key == SDLK_LSHIFT) {
				if(windows.count() > 1)
						windows.shift_right();

						SDL_Delay(100);

				focus_window = windows.tailptr()->item;

				return;
			}



			listItem<Window *> *item = windows.tailptr(), **ptr = &item;

			while ( (item = *ptr) != 0)  {
				Window *w = item->item;
				if( w->keyUp(key) == MSG_PROC ) return;
				ptr = &item->prev;
			}

		}

		void Dimension::mouseMove(int x, int y, int button)
		{

			if(windows.count() < 1) return;


			listItem<Window *> *item = windows.tailptr(), **ptr = &item;

			while( ( item = *ptr ) != 0 ) {
				Window *w = item->item;

				if(w->moveable() == true && w->moving() == true) {

						Rect rcrt = Rect(Point(0,0),  wnd->size());
                    
                      
						if(w->movement_rect.point.x+w->movement_rect.rsize.width < wnd->size().width && w->movement_rect.point.y+w->movement_rect.rsize.height < wnd->size().height) {

							if(w->movement_rect.point.y < 30)  w->movement_rect.point.y = 25;


						if(x+w->movement_rect.rsize.width  < wnd->size().width-10 && x > 10) w->movement_rect.point.x = x;
						if(y+w->movement_rect.rsize.height < wnd->size().height-10 && y > 25) w->movement_rect.point.y = y;
						w->moved_window = true;
						movement_window =  w;

						return;
						}


					}

				if( w->mouseMove(x,y,button) == MSG_PROC ) return;
				ptr = &item->prev;
			}
		}

		int Dimension::mouseDown(int x, int y, int button)
		{
			if(windows.count() == 0) return MSG_IGNORED;


			listItem<Window *> *item = windows.tailptr(), **ptr = &item;

			while ( (item = *ptr) != 0)  {

					Window *w = item->item;

					Rect rc = w->rect();


					if(w->caption() != "" && w->state() != MINI && w->Visible() == true && rc.pointInRect(Point(x,y)) == true) {
						w->mouseDown(x,y,button);
						setFocus(w->getID());
						return MSG_PROC;
					}


					 if( w->mouseDown(x,y,button) == MSG_PROC) return MSG_PROC;
					//if(rt_val == MSG_REMOVE) {
					//windows.remove(item->item);




				ptr = &item->prev;
			}

			return MSG_IGNORED;
		}


		void Dimension::mouseUp(int x, int y, int button)
		{

			if(windows.count()  < 1) return;

			Window *w = movement_window;

			 if(w != 0 && w->moving() == true && w->moveable() == true &&  w->moved_window == true   && movement_window !=  0) {

				if(w->movement_rect.point.x < 0) w->movement_rect.point.x = 0;
				if(w->movement_rect.point.y < 0) w->movement_rect.point.y = 0;
				if(w->movement_rect.point.x > wnd->size().width) w->movement_rect.point.x = wnd->size().width-w->size().width;
				if(w->movement_rect.point.y > wnd->size().height) w->movement_rect.point.y = wnd->size().height-w->size().height;
				if(w->movement_rect.point.x+w->movement_rect.rsize.width+10 > wnd->size().width) w->movement_rect.point.x-=10;
				w->setPos(Point(w->movement_rect.point.x-10, w->movement_rect.point.y-20));
				w->setMovement(false);
			    w->moved_window = false;
			    w->drawn_already = false;
			    return;
			}



			listItem<Window *> *item = windows.tailptr(), **ptr = &item;

			while ( (item = *ptr ) != 0) {

				Window *w = item->item;
				assert( w != 0 );

				if( w->mouseUp(x,y,button) == MSG_PROC ) return;

				ptr = &item->prev;
			}

		}

		Window *Dimension::findWindow(string name)
		{
			if(windows.count() == 0)  return 0;

			listItem<Window *> *item = windows.tailptr(), **ptr = &item;
			while( (item = *ptr) != 0)
			{
				Window *w = item->item;

				if(w->caption() == name) return w;

				ptr = &item->prev;
			}
			return 0;
		}


		Window *Dimension::operator[](string name)
		{
			return findWindow(name);
		}

		int Dimension::newID()
		{
			ident_offset += 1;
			return ident_offset;
		}

		Window *Dimension::findWindow(int id)
		{
			if(windows.count() == 0)  return 0;

			listItem<Window *> *item = windows.rootptr(), **ptr = &item;

			while ( (item = *ptr) != 0 )  {

				Window *w = item->item;
				if(w->getID() == id) return w;

				ptr = &item->next;
			}

			return 0;
		}

		Window *Dimension::operator[](int id)
		{
			return findWindow(id);
		}

		void Dimension::sendMessage(Window *window, widgetAction &we)
		{
			window->procEvent(we);
		}

		void Dimension::setBackground( std::string image )
		{
			mxSurface bg;
			mx::mxPng p;

			if(p.pngOpen(image)) {
				bg = p.LoadPNG();
				p.pngClose();
			}
			background_image.copyResizeSurface(bg);
			background_image_on = true;
		}

		void Dimension::setFocus(int id)
		{


			if(focus_window && id == focus_window->getID()) return;

			while ( id != focus_window->getID()) {

				windows.shift_right();
				focus_window = windows.tailptr()->item;
			}
		}

		DimensionDock::DimensionDock(Dimension *super, uiList_ptr<Window *> *wptr, int w, int h, int extra_h, mxSurface &surface) : windows( wptr ), surf(surface), parent(super)
		{
			dimensions.rsize = Size(w,h);
			dimensions.point = Point(0,extra_h-h);
			show();
			desktop  = Size(w,h);
			mxPng p;
			if(p.pngOpen("img/left.png")) {
				left = p.LoadPNG();
				p.pngClose();
			}

			if(p.pngOpen("img/menubar.png")) {
				bar = p.LoadPNG();
				p.pngClose();
			}

			menu_light = false;
			draw_menu = false;

			pos_s = -1;
		}

		int DimensionDock::mouseMove(int x, int y, int button)
		{
			Rect menuRc(surf.size().width-100, dimensions.point.y+5, 96, 20 );

			if( menuRc.pointInRect(Point(x,y)) ) {
				menu_light = true;
				return MSG_PROC;
			} else menu_light = false;


			std::vector<std::string>::reverse_iterator i;
			int pos = dimensions.point.y-375;
			int pos_i = 0;
			for(i = dir_vec.rbegin(); i != dir_vec.rend(); i++) {
				 Rect rcZ( surf.size().width-165, pos-3, 158, 20);

				 if(rcZ.pointInRect(Point(x,y))) {
					 pos_s = pos_i;
					 return MSG_PROC;
				 }
				pos += 25;
				pos_i ++;
			}
			pos_s = -1;
			return MSG_IGNORED;
		}


		int DimensionDock::mouseDown(int x, int y, int button) {

			listItem<Window *>  *ptr = windows->tailptr(), **z = &ptr;

			int pos = 5;


			int counter = 0,  counter_max = 3;

			while ( (ptr = *z) != 0) {

				Rect rc( Point(pos, dimensions.point.y+5), Size(200,20));

				if(ptr->item->caption() == "" || ptr->item->Visible() == false) {
					z = &ptr->prev;
					continue;
				}
				pos += 205;

				if(rc.pointInRect(Point(x,y))) {
					if(ptr->item->state() == MINI) {
						ptr->item->setWindowState(NORMAL);
						parent->setFocus(ptr->item->getID());
					} else if(ptr->item->state() == NORMAL) {
						ptr->item->setWindowState(MINI);
					}
					return MSG_PROC;
				}

				counter++;
				if(counter > counter_max) break;
				z = &ptr->prev;
			}

			Rect rc_left(pos-2, dimensions.point.y+3, 36, 26);

			if(rc_left.pointInRect( Point(x,y) ) ) {
				parent->windows.shift_right();
				if(parent->windows.tailptr()->item->Visible() == true && parent->windows.tailptr()->item->state() == NORMAL)
				{
					parent->focus_window = parent->windows.tailptr()->item;

				}

				return MSG_PROC;
			}

			pos += 40;

			Rect menuRc(surf.size().width-100, dimensions.point.y+5, 96, 24 );

			if ( menuRc.pointInRect(Point(x,y)) ) {
				if( draw_menu == false ) draw_menu = true;
				else draw_menu = false;

			} else if(draw_menu == true) draw_menu = false;


			std::vector<std::string>::reverse_iterator i;
			int pos_one = dimensions.point.y-375;
			int pos_i = 0;
			if(pos_s != -1)
			for(i = dir_vec.rbegin(); i != dir_vec.rend(); i++) {
				Rect rcZ( surf.size().width-165, pos_one-3, 158, 20);
				if(rcZ.pointInRect(Point(x,y))) {
					std::vector<std::string>::reverse_iterator i = dir_vec.rbegin() + pos_s;
					std::cout << "[ MX System Run Python Script: " << *i << " ]\n";
					run(*i);
					return MSG_PROC;
				 }
				pos_one += 25;
				pos_i ++;

				if(pos_one > dimensions.point.y-50) break;

			}

			return MSG_IGNORED;;
		}

		int DimensionDock::renderEvent() {


			mxPainter paint(surf);

			paint.fillRect(dimensions, Color(0x99, 0x99, 0x99));
			Rect rc  = Rect(dimensions);
			rc.point.x += 4;
			rc.point.y += 4;
			rc.rsize.width -= 4;
			rc.rsize.height -=4;
			paint.fillRect(rc, Color(0x77, 0x77, 0x77));

			if(windows->count() == 0) return MSG_PROC;

			int pos = 5;


			listItem<Window *> *ptr = windows->tailptr(), **rest = &ptr;


			int counter = 0, count_max = 3;

			while ( (ptr = *rest ) != 0) {

				const string cap = ptr->item->caption();
				if(cap == "" || ptr->item->Visible() == false) {
					rest = &ptr->prev;
					continue;
				}

				Rect rc( Point(pos, dimensions.point.y+5), Size(200,20));
				Color col = Color(200, 205, 203);

				if(ptr->item->state() == MINI) col = Color(0,0,00);

				paint.fillRect(rc, col);
				Color textcolor;
				textcolor = Color(255,0,0);

				string str = ptr->item->caption();
				if(str.length()>23) {
					str = str.substr(0, 20);
					str += "...";
				}
				font()->printText(surf, Point(pos+2, dimensions.point.y+7), textcolor, str );

			   	pos += 205;
				counter++;
				if(counter > count_max) break;

				rest = &ptr->prev;
			}

			paint.fillRect( Rect(pos-2, dimensions.point.y+3, 36, 32), Color(0,0,0));
			SDL_Rect rc1 =  { pos, dimensions.point.y+5, 36, 32 };
			pos += 40;
			SDL_Rect srcRc = { 0,0,32,23 };
			surf.copySurface(left, &srcRc, &rc1);


			/* draw menu */

			Rect menuRc(surf.size().width-100, dimensions.point.y+5, 96, 24 );

			if(menu_light == false)
			{
				paint.fillRect( menuRc, Color(50, 50, 50) );
			}
			else paint.fillRect( menuRc, Color(200,200,200) );


			font("terminal")->printText(surf, Point(surf.size().width-90, dimensions.point.y+11), Color(0xff,0x0,0x0), "Python ");

			if(draw_menu == true)
			{

				parent->setFocus( this->getID() );
				Rect menuRect( surf.size().width-200, dimensions.point.y-400, 196, 400 );
				paint.fillRect( menuRect, Color(200,200,200) );
				SDL_Rect rcX = menuRect;
				rcX.w = 25;
				surf.copySurface(bar, 0, &rcX );


				if(dir_vec.size()==0) {

					DIR *d = opendir("scripts/");
					struct dirent *entry = 0;

					while ( (entry = readdir(d)) != 0 ) {

						if(strstr(entry->d_name, ".py") != 0 && strstr(entry->d_name, ".pyc") == 0) {

							std::string manip = entry->d_name;
							dir_vec.push_back(manip.substr(0, manip.rfind(".")));
						}
					}

					closedir(d);
				}

				std::vector<std::string>::reverse_iterator i;
				int pos = dimensions.point.y-375;
				int pos_i = 0;
				for(i = dir_vec.rbegin(); i != dir_vec.rend(); i++) {

					if(pos_i == pos_s) {
						paint.fillRect( Rect( surf.size().width-165, pos-3, 158, 20), Color(50, 50, 50));
					}

					font("terminal")->printText(surf, Point(surf.size().width-160, pos), Color(0xff, 0x0, 0x0), *i);
					pos += 25;
					pos_i ++;

					if(pos > dimensions.point.y-50) break;

				}
			}

			return MSG_PROC;

		}
}

