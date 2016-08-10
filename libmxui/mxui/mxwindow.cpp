#include "mxwindow.h"
#include<mxl.h>
#include "mxreg.h"
#include<cstdlib>
#include<boost/lexical_cast.hpp>
#include"mxdimension.h"

namespace mxui {

	int Window::identifer = rand()%100;


	Window::Window(mxWidget *parent)
	{
		exit_msg = false;
		setParent(parent);
		skin_loaded = false;
		move_window = false;
		moved_window = false;
		text_color = Color(255,0,0);
		mini_me = false;
		_close = true;
		_max = true;
		keyInput = false;
		render_cb = 0;
		event_cb = 0;
		render_alpha = false;
		dim = 0;
		drawn_already = false;

	}

	Window::~Window()
	{
		on_destroy();
	}


	void Window::on_destroy()
	{
		draw_surface.cleanSurface();
		// clean up

		listItem<mxWidget *> *item = children.rootptr(), **ptr = &item;

		while ( (item = *ptr) != 0) {

			item->item->destroy();

			ptr = &item->next;
		}

		std::cout << "[ mxui ] - releasing Window " << caption() <<  "\n";


	}

	int Window::destroy()
	{
		on_destroy();
		return MSG_PROC;
	}

	void Window::createWindow(Color textcolor, string title, Point pos, Size size, Color titlebar, Color fillcolor, Color backcolor)
	{
		exit_msg = false;
		this->textcolor = textcolor;
		this->title = title;
		dimensions.point = pos;
		dimensions.rsize = size;
		this->titlebar = titlebar;
		this->backcolor = backcolor;
		this->fillcolor = fillcolor;

		draw_surface.createSurface(dimensions.rsize);
		on_create();
		window_state = NORMAL;
		show();
		focus = 0;
		children.list_id=title;
	}

	void Window::setAlphaBlend(Dimension *d,bool flag)
	{
		render_alpha = flag;

		if(render_alpha == true) {
			alpha_surf.createSurface( draw_surface.size().width,draw_surface.size().height );
			alpha_surf.copyResizeSurface(windowbg);
			dim = d;
			pre_render.createSurface( draw_surface.size().width, draw_surface.size().height );

		}
		else alpha_surf.cleanSurface();

	}

	void Window::redraw_surface() {


		mx::mxPainter dimbg(dim->background), image_bg(alpha_surf), drawbg(pre_render);
		
		dimbg.lock();
		image_bg.lock();
		drawbg.lock();
		
		mx::Color pix1,pix2,final_pix;
		unsigned int i=0,z=0,total_x=this->point().x,total_y=this->point().y;
			for(i = 0; i < pre_render.size().width; i++) {
				for(z = 0; z < pre_render.size().height; z++) {
					
					image_bg.GetPixel(i,z, pix1);
					dimbg.GetPixel(total_x,total_y,pix2);
					unsigned char r,g,b;
					r = static_cast<unsigned char>( alpha*pix1.color.colors[0] + (1-alpha)*pix2.color.colors[0] );
					g = static_cast<unsigned char>( alpha*pix1.color.colors[1] + (1-alpha)*pix2.color.colors[1] );
					b = static_cast<unsigned char>( alpha*pix1.color.colors[2] + (1-alpha)*pix2.color.colors[2] );
					final_pix=Color(r,g,b);
					drawbg.setpixel(i,z, final_pix);
					total_y++;
				}
				total_x++;
				total_y=this->point().y;
			}
		
			dimbg.unlock();
			image_bg.unlock();
			drawbg.unlock();

			drawn_already = true;
	}

	int Window::renderAlpha()
	{

		if(dim != 0) {
			if(drawn_already == false) redraw_surface();
			draw_surface.copyResizeSurface(pre_render);

		}

		draw_surface.copySurface(title_bar, 0, 0);
		SDL_Rect pos = { title_bar.width()-wbar.width(), 0, wbar.width(), wbar.height() };
		static SDL_Color col = { 0xff, 0xff, 0xff };
		draw_surface.copySurfaceColorKey(wbar, 0, &pos, col);
		font()->printText(Surface(), Point(20, 3), titlebar, title);
		renderChildren();

		return 0;
	}


	int Window::renderEvent()
	{

		if(render_alpha) {
			return renderAlpha();
		}

		if(skin_loaded == false) {

			mx::mxPainter paint(Surface());

			Rect point;
			point.point = Point(0,0);
			point.rsize = dimensions.rsize;
			Rect spoint;
			spoint.point = Point(5,5);
			spoint.rsize = dimensions.rsize;
			spoint.rsize.width -= 10;
			spoint.rsize.height -= 10;
			paint.fillRect(point, fillcolor);
			paint.fillRect(spoint, backcolor);
			Rect pos;
			pos.point = Point(0,0);
			pos.rsize = Size( size().width, 20 );
			paint.fillRect(pos, titlebar);
		} else {

			draw_surface.copyResizeSurface(windowbg);
			draw_surface.copySurface(title_bar, 0, 0);

			SDL_Rect pos = { title_bar.width()-wbar.width(), 0, wbar.width(), wbar.height() };
		    static SDL_Color col = { 0xff, 0xff, 0xff };
		    draw_surface.copySurfaceColorKey(wbar, 0, &pos, col);
		}

		font()->printText(Surface(), Point(20,3), titlebar, title);

		renderChildren();

		if(render_cb !=  0)  {
			render_cb(cb_name, this, Surface());
		}

		return MSG_PROC;

	}

	void Window::copyWidgetSurface(mxSurface &surface) {

		if(state() !=   MINIMIZING) {

			mxWidget::copyWidgetSurface(surface);

			return;
		}


		static int   x=0,   w=0;

		int size_width = dimensions.rsize.width;

		if(w == 0) w =   dimensions.rsize.width;



		w -= 50;
		x -= 50;


		if(x >= size_width || w <= 0) {

			window_state =  MINI;
			change_state(window_state);
			x =0,  w = 0;
			return;
		}


		SDL_Rect rcX  = { x, dimensions.point.y, w, dimensions.point.y+dimensions.rsize.height };
		SDL_Rect rc = { dimensions.x(), dimensions.y(), dimensions.width(), dimensions.height() };


		surface.copySurface(Surface(), &rcX, &rc);


	}

	void Window::renderChildren()
	{

		listItem<mxWidget *> *item = children.rootptr(), **ptr = &item;


		while((item = *ptr) != 0 )
		{

			mxWidget *p = item->item;

			if(p->renderEvent() == MSG_PROC) {

				Rect sizeRect = p->sizeRect();
				Rect dimeRect = p->rect();

				Surface().copySurface(p->Surface(), sizeRect, dimeRect);

			}

			ptr = &item->next;

		}

	}

	int Window::procEvent(widgetAction &we)
	{
		if(exit_msg == true) return exit_msg;

		switch(we.actionCode) {
		case MX_QUIT:
			on_close();
			return MSG_PROC;
			break;
		case MX_MINIMIZE:
			minimizeWindow();
			return MSG_PROC;
			break;
		case MX_MAXIMIZE:
			maximizeWindow();
			return MSG_PROC;
		default:
			return MSG_IGNORED;
			break;
		}

		return MSG_IGNORED;
	}

	void Window::minimizeWindow()
	{
		on_minimize();
	}

	void Window::maximizeWindow()
	{
		on_maximize();
	}

	// all children a window owns
	// so use new to allocate them
	//  on release window will deallocate
	int  Window::addWidget(mxWidget *widget)
	{
		widget->setIdentifier(++identifer);
		children.append(widget);
		setFocus(widget);
		return widget->identifier();
	}

	mxWidget *Window::operator[](int ID)
	{
		return findID(ID);

	}

	mxWidget *Window::findID(int ID) {

		listItem<mxWidget *> *item = children.rootptr(), **ptr = &item;
		while ( (item = *ptr) != 0) {
			if(item->item->identifier() == ID) return item->item;
			ptr = &item->next;
		}
		return 0;

	}

	mxWidget *Window::operator()(int id)  {
		return  findID(id);
	}

	void Window::setCaption(string text)
	{
		this->title = text;
	}

	void Window::on_minimize()
	{
		window_state = MINIMIZING;
	}

	void Window::on_maximize()
	{
		window_state = MAX;
	}

	void Window::on_close()
	{
		exit_win = true;
		exit_msg = MSG_REMOVE;
	}

	int Window::mouseMove(int x, int y, int button)
	{
		if(event_cb != 0) {
			memset(&we, 0, sizeof(we));
			we.actionCode = MX_MOUSEMOVE;
			we.cursor_x = x;
			we.cursor_y = y;
			we.button = button;
			event_cb(cb_name, this, we);
		}


		listItem<mxWidget *> *ptr = children.rootptr(), **pointer = &ptr;

		Point pos = getPos();

		Rect rc (pos.x,pos.y, title_bar.width(), 20);

		if(Visible() == true && rc.pointInRect(Point(x,y))) {

			if(button == 1) {

				move_window = true;
				movement_rect = rect();
			}


			return MSG_IGNORED;

		}
		while (( ptr = *pointer ) != 0 ) {

			mxWidget *ztr = ptr->item;

			if( ztr->mouseMove(x,y,button) == MSG_PROC ) return MSG_PROC;

			pointer = &ptr->next;
		}


		return MSG_IGNORED;
	}

	int Window::mouseDown(int x, int y, int button)
	{
		if(event_cb != 0) {
			memset(&we, 0, sizeof(we));
			we.actionCode = MX_MOUSEDOWN;
			we.cursor_x = x;
			we.cursor_y = y;
			we.button = button;
			event_cb(cb_name, this, we);
		}


		listItem<mxWidget *> *ptr = children.rootptr(), **pointer = &ptr;

		while (( ptr = *pointer ) != 0 ) {
		 int  rt =  ptr->item->mouseDown(x,y,button);

		 if(rt  == MSG_CLICKED)
		 {
			 ptr->item->mouse_clicked();
			 return MSG_PROC;
		 }

		 if(rt == MSG_FOCUS) {

			 clrFocus();
			 setFocus(ptr->item);
			 ptr->item->setFocus(true);
			 return MSG_PROC;
		 }


		 	pointer = &ptr->next;
		}

		int  rt_val = checkItemDown(x,y,button);

		if(rt_val == MSG_PROC ||  MSG_REMOVE) return  rt_val;


		return MSG_IGNORED;

	}
	int Window::mouseUp(int x, int y, int button)
	{

		if(event_cb != 0) {
			memset(&we, 0, sizeof(we));
			we.actionCode = MX_MOUSEUP;
			we.button = button;
			we.cursor_x = x;
			we.cursor_y = y;
			event_cb(cb_name, this, we);
		}

		move_window = false;

		listItem<mxWidget *> *ptr = children.rootptr(), **pointer = &ptr;

		while (( ptr = *pointer ) != 0 ) {
			if( ptr->item->mouseUp(x,y,button) == MSG_PROC ) return MSG_PROC;
			pointer = &ptr->next;
		}

		return MSG_IGNORED;
	}

	int Window::keyDown(int key, void *data) {

		if( focus != 0 && keyInput == true) {

			focus->keyDown(key, data);

			return MSG_PROC;


		} else {

			listItem<mxWidget *> *ptr = children.rootptr(), **pointer = &ptr;

			while (( ptr = *pointer ) != 0 ) {

				if( ptr->item->keyDown(key, data) == MSG_PROC ) return MSG_PROC;

				pointer = &ptr->next;
			}
		}

		if(event_cb != 0) {
			memset(&we, 0, sizeof(we));
			we.actionCode = MX_KEYDOWN;
			we.key = key;
			we.extra = data;
			event_cb(cb_name, this, we);
		}

		return MSG_PROC;
	}

	int Window::keyUp(int key) {

		listItem<mxWidget *> *ptr = children.rootptr(), **pointer = &ptr;
		while (( ptr = *pointer ) != 0 ) {
			if( ptr->item->keyUp(key) == MSG_PROC ) return MSG_PROC;
			pointer = &ptr->next;
		}

		if(event_cb != 0) {
			memset(&we, 0, sizeof(we));
			we.actionCode = MX_KEYUP;
			we.key = key;
			event_cb(cb_name, this, we);
		}

		return MSG_IGNORED;
	}


	void Window::useSkin(string path) {

		skin_path = path;
		Size s = draw_surface.size();

		windowbg.createSurface( s );
		title_bar.createSurface ( draw_surface.width(), 20);

		mx::mxPng p;
		mxSurface temp;

		mxlParser parser;

		if(!parser.parseMXL_file(path + "/skin.mxl")) throw mx::mxException<string>("[ mxl ]: a error processing registry");

		mxReg::app_registry.addMXL(parser);
		string file = path + "/" + mxReg::app_registry.table()["skin"]["background"];


		if(p.pngOpen(file)) {
			temp = p.LoadPNG();
			p.pngClose();
			windowbg.copyResizeSurface(temp);
			temp.cleanSurface();
		} else throw mx::mxException<string>("[ mxui ]: error could not load png file: " + file);


		file = path + "/" + mxReg::app_registry.table()["skin"]["titlebar"];


		if(p.pngOpen(file)) {
			temp = p.LoadPNG();
			p.pngClose();
			title_bar.copyResizeSurface(temp);
			temp.cleanSurface();
		}  else throw mx::mxException<string>("[ mxui ]: error could not load png file: " + file);

		file = path + "/" + mxReg::app_registry.table()["skin"]["bar"];

		if(p.pngOpen(file)) {
			wbar = p.LoadPNG();
			p.pngClose();

		} else throw mx::mxException<string>("[ mxui ]: error could not load png file: " + file);

		skin_loaded = true;
	}


	void Window::resizeWindow ( Size s ) {

		draw_surface.cleanSurface();
		draw_surface.createSurface(s);
		dimensions.rsize = s;

		if(render_alpha) {
			pre_render.cleanSurface();
			alpha_surf.cleanSurface();
			alpha_surf.createSurface(s);
			pre_render.createSurface(s);
			drawn_already = false;
		}

		if(skin_path.length()>0) {
			useSkin(skin_path);
		}
	}

	void Window::resizeWindow( int w, int h) {

		resizeWindow( Size(w, h) );
	}

	int Window::checkItemDown(int x, int y, int button) {


		try {

		if(skin_loaded == false) return MSG_IGNORED;


		if(button == 1) {

			Point cursor(x,y);



				static Point pointer( boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["min_x1"] ), boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["min_y1"] ) );
				static Size  sizeobj( boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["min_x2"] ), boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["min_y2"] ) );

				Point par = getPos();

				Point final = Point(par.x+(title_bar.width()-wbar.width()) , par.y );
				Rect rc(final, sizeobj);

				if(rc.pointInRect(cursor)) {
					setWindowState(MINI);
					return MSG_PROC;
				}

				static Point pointer2( boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["max_x1"] ), boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["max_y1"] ) );
				static Size  sizeobj2( boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["max_x2"] ), boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["max_y2"] ) );

				Rect rc2(final, sizeobj2);

				if(_max == true && rc2.pointInRect(cursor)) {
					setWindowState(MAX);
					return MSG_PROC;
				}

				static Point pointer3( boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["close_x1"] ), boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["close_y1"] ) );
				static Size  sizeobj3( boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["close_x2"] ), boost::lexical_cast<int> ( mxReg::app_registry.table()["barmenu"]["close_y2"] ) );

				Rect rc3(final, sizeobj3);

				if(rc3.pointInRect(cursor)) {

						if(_close == true) hide();

						return MSG_REMOVE;
				}

			}


		}
		catch( boost::bad_lexical_cast &bc ) {

			std::cerr << "[ mxui ]: error parsing Registry window coordinates contain invalid character?\n";
			throw;
		}


		return MSG_IGNORED;
	}


	void Window::change_state(const WINDOW_STATE &s) {


		if(s == MAX) {

			if(savestate.restore_set == 0) {

				savestate.pos = getPos();
				savestate.rc = this->dimensions;
				savestate.restore_set = 1;
				setPos(Point(0,25));
				resizeWindow( max_x, max_y-25);
			} else if( savestate.restore_set == 1 ) {
				setPos( savestate.pos );
				resizeWindow ( savestate.rc.rsize );
				savestate.restore_set = 0;
			}

		}

	}

}
