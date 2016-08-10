#include "mxwidget_type.h"
#include "mxpng.h"
#include<mxinterface.h>
#include<mxf.h>
#include "keyfilter.h"
#include "wcode.h"
#include<cstdlib>
#include<ctime>
#include<mx_types.h>


namespace mxui {

	using mx::mxPainter;
	using mx::Color;
	bool destroy;


	std::map<std::string, mx::mxRegularFont *> mxFontDisplay::fonts;
	bool mxFontDisplay::loaded = false;
	mx::mxRegularFont mxFontDisplay::system;
	int mxFontDisplay::ref_count = 0;


	bool mxFontDisplay::isFontLoaded(string name)
	{
		std::map<std::string, mx::mxRegularFont *>::iterator i;
		i = fonts.find( name );
		if(i != fonts.end()) return true;
		return false;
	}

	mxFontDisplay::mxFontDisplay()
	{
		string path = FONT_PATH; // default path
		cur_font = "system"; // default font

		if(loaded == false) {
			loadDefaults(path);
			loaded = true;
			ref_count = 1;
		} else ref_count++;
	}

	// reference counted destructor
	mxFontDisplay::~mxFontDisplay()
	{
		ref_count--;
		if(ref_count == 0)
		{
			std::map<std::string, mxRegularFont *>::iterator i;
			for(i = fonts.begin(); i != fonts.end(); i++) {
				if(i->second) {
					std::cout << "[ mxui ] relesing font: " << (unsigned long)(i->second) << "\n";
					i->second->freeFont();
					delete i->second;
				}
			}
			fonts.clear();
		}
	}

	void mxFontDisplay::setFont(string name)
	{
		if(isFontLoaded(name) == true)
		{
			cur_font = name;
		}
		else cur_font = "system";
	}

	bool mxFontDisplay::loadFont(string fullpath)
	{
		mxRegularFont *font = 0;
		try
		{
			font = new mxRegularFont;
			bool val = font->loadFont(fullpath);
			if(val == false) { delete font; return false; }
			std::string name = fullpath.substr(fullpath.rfind("/")+1, fullpath.length());
			std::string font_name = name.substr(0, name.rfind(".mxf"));
			fonts[font_name] = font;
		}
		catch (...)
		{
			delete font;
		}
		return true;
	}

	Label::Label(mxWidget *parent, string text, Point pos) : mxFontDisplay()
	{
		setParent(parent);
		setText(Color(0,0,0), text);
		setPos(pos);
	}

	void  Label::setText(Color textcolor, string text) {
		this->text = text;
		this->textcolor =   textcolor;
	}

	void Label::setDrawSurface( mxSurface &draw_surf )
	{
		drawToSurface = &draw_surf;
	}


	string Label::getText()
	{
		return this->text;

	}

	int Label::renderEvent() {

			if(text.length()>0)
			font()->printText(*drawToSurface, dimensions.point.x, dimensions.point.y,  Color::mapRGB(*drawToSurface, textcolor), text.c_str() );
			return MSG_PROC;
	}

	Button::Button(mxWidget *parent)
	{
		setParent(parent);
		cb = 0;
	}
	Button::Button(mxWidget *parent, Rect dimensions, string text)
	{
		setParent(parent);
		createButton(dimensions, text, Color(150,175,200), Color(255,255,255), Color(0,0,0));
		cb = 0;
	}

	void Button::createButton(Rect dimensions, string text, Color backcolor, Color forecolor, Color textcolor)
	{
		createWidget(dimensions);
		this->text = text;
		this->backcolor = backcolor;
		this->forecolor = forecolor;
		this->textcolor = textcolor;
		mode_clicked = 0;
		cb = 0;
	}

	int Button::mouse_clicked() {

		if(cb != 0)
			cb(obj_name);
	}

	void Button::setCallback( void (*mcb)(string) ) {
		cb = mcb;
	}

	void Button::setname(std::string name) {
		obj_name = name;
	}

	int Button::mouseMove(int x, int y, int button)
	{

		Point p1 = getPos(), p2 = parent->getPos();
		Size sz = getSize();

		Rect rc(p1.x+p2.x, p1.y+p2.y, sz.width, sz.height);
		if(rc.pointInRect(Point(x,y))) {
			mode = 1;
			return MSG_PROC;
		} else {
			mode = 2;
		}

		return MSG_IGNORED;
	}

	int Button::clicked()
	{
		widgetAction we;
		memset(&we, 0, sizeof(we));
		we.actionCode = MX_BUTTON_CLICKED;
		we.extra = (void*)this;
		parent->procEvent(we);
		mode_clicked = 0;
		return MSG_PROC;
	}

	int Button::mouseDown(int x, int y, int button)
	{
		if(button == SDL_BUTTON_LEFT) {
			Point p1 = getPos(), p2 = parent->getPos();
			Size sz = getSize();

				Rect rc(p1.x+p2.x, p1.y+p2.y, sz.width, sz.height);
				if(rc.pointInRect(Point(x,y))) {
					if(parent != 0) {
						mode_clicked = 1;
						mode = 0;
						return MSG_CLICKED;
					}
					return MSG_PROC;
				}
		}

		return MSG_IGNORED;

	}

	int Button::mouseUp(int x, int y, int button) {

		if(button == SDL_BUTTON_LEFT) {
			Point p1 = getPos(), p2 = parent->getPos();
			Size sz = getSize();

				Rect rc(p1.x+p2.x, p1.y+p2.y, sz.width, sz.height);
				if(rc.pointInRect(Point(x,y))) {
					if(parent != 0 && mode_clicked == 1) {
						mode = 0;
						mode_clicked = 0;
						buttonClicked();
						clicked();
					}
					return MSG_PROC;
				}
			}


		return MSG_IGNORED;
	}


	void Button::buttonClicked()
	{


	}

	int Button::renderEvent()
	{
		mx::mxPainter p(Surface());

		Rect fill;

		fill.point = Point(0,0);
		fill.rsize = dimensions.rsize;

		p.fillRect(fill, (mode == 1) ? forecolor : backcolor );

		fill.rsize.width -= 3;
		fill.rsize.width -= 3;
		fill.point.x += 3;
		fill.point.y += 3;

		p.fillRect(fill, (mode == 1) ?  backcolor : forecolor );

		Point textpoint = Point(5,8);

		font()->printText(Surface(), textpoint, textcolor, text);
		return MSG_PROC;
	}

	Image::Image(mxWidget *parent)
	{
		setParent(parent);
	}

	Image::Image(mxWidget *parent, string filename, Point pos)
	{
		if(loadImage(filename) == false) throw mx::mxException<std::string>(" mxException: Error could not load image: " + filename + "\n");
	}

	void Image::stretchImage(Size new_size)
	{
		mxSurface *self_c = draw_surface.copySurface();
		draw_surface.cleanSurface();
		draw_surface.createSurface(new_size);
		draw_surface.copyResizeSurface(*self_c);
		delete self_c;
	}

	bool Image::loadImage(string filename)
	{
		string lwr;
		for(unsigned int i = 0; i < filename.length(); i++) lwr += tolower(filename[i]);

		if(lwr.find(".png") != 0) {

			mx::mxPng p;
			if(p.pngOpen(filename)) {

				draw_surface = p.LoadPNG();
				p.pngClose();

			}
			else throw mx::mxException<std::string>(" error could not load png file: " + filename + "\n");

		}
		else if(lwr.find(".bmp") != 0) {

			draw_surface = SDL_LoadBMP(filename.c_str());

		}
		else {

			throw mx::mxException<std::string>(" mxException: only supported image types are png and bmp you provided: " + filename + "\n");
		}


		setSize(draw_surface.size());

		return true;
	}

	int Image::renderEvent() {

		return MSG_PROC;
	}


	LineEdit::LineEdit(mxWidget *parent, Point pos, int width)
	{
		Rect rc(0,0, width,25);
		createWidget(rc);
		wid =   width;
		setPos(pos);
		setParent(parent);
		tcolor = Color(0,0,0);
		cursor = Point(0,0);
		offset = 0;
		setText("");
		scroll = true;
		has_focus = false;
		pw = false;
		type = TYPE_EDIT;

	}

	void LineEdit::setText(string text) {
		data = text;
		cursor.x =  data.length();
		cursor.y = data.length();
	}

	void LineEdit::setColor(Color col) {
		tcolor =  col;
	}

	void LineEdit::setCursorPos(Point cur) {
		cursor = cur;
	}

	void LineEdit::enterPressed() {


	}

	void mid(string &in, string &out,  size_t start, size_t stop) {

		size_t i, offset = 0;
		for(i = start; i < stop; i++) {
			out[offset] = in[offset];
			offset++;
		}
	}

	int LineEdit::keyDown(int key, void *datax) {




		if(key == SDLK_RETURN) {
			enterPressed();
			return MSG_PROC;
		}




		if(key == SDLK_UP || key == SDLK_DOWN || key == SDLK_ESCAPE ||  key ==  SDLK_TAB) return MSG_IGNORED;


		if(data.length() > 0) {


			if(key == SDLK_BACKSPACE) {

				if(data.length() == 1)  {

					data = string();
					cursor.x = cursor.y = 0;
					offset = 0;
					scroll = true;
				}


					string left;
					string right;

						if(static_cast<unsigned int>(offset+cursor.x) == data.length()) {

								data = removeChar(data.length()-1, data);
								cursor.y = data.length();
								cursor.x = data.length();
								return MSG_PROC;
						}

						if(cursor.x > 0) {

							//left = data.substr(0, cursor.x-1);
							//right = data.substr(cursor.x, data.length()-cursor.x);

							data = removeChar(cursor.x, data);
							cursor.x--;
							cursor.y--;
							return MSG_PROC;
						}






					if(offset > 0 && rt_val == 0) offset--;

				return MSG_PROC;
			}
		}

		if(key == SDLK_HOME) {

			cursor.x =  0;
			cursor.y = 0;
			offset = 0;
			scroll = false;
			return MSG_PROC;
		}
		else if (key == SDLK_END) {

			cursor.x = data.length();
			cursor.y = data.length();
			offset = offset-data.length();
			return MSG_PROC;
		} else if(key == SDLK_LEFT) {

			if(cursor.x > 0 && cursor.y > 0) cursor.x--, cursor.y--;
			return MSG_PROC;
		}
		else if(key == SDLK_RIGHT) {

			if(static_cast<unsigned int>(cursor.x) < data.length() && static_cast<unsigned int>(cursor.y) < data.length()) {
				cursor.x ++;
				cursor.y ++;
			}

			return MSG_PROC;
		}

		int ch = mxKeyFilter(datax);


			if(ch != 0  && key != SDLK_BACKSPACE)  {
					string c;
					c += ch;
					string leftX, rightX, finalX;

						if(key < 0 || key > 127) return MSG_IGNORED;
								try {

									data = insertChar(offset+cursor.x, c[0], data);

								}
								catch(std::exception  &e) {

								}

								return MSG_PROC;
						}
						else {

							cursor.x++;
							cursor.y++;
							//data += char(ch);


						}


			//cursor.x = cursor.y = this->data.length();
		return MSG_IGNORED;

	}

	void LineEdit::setFocus(bool set)
	{
		has_focus = set;
	}

	int  LineEdit::renderEvent()  {
		srand(time(0));

		Rect rc(0, 0, dimensions.rsize.width, dimensions.rsize.height);
		mxPainter p(Surface());
		if(has_focus == true)
		p.fillRect(rc, mx::Color::mapRGB(Surface(), 255, rand()%255, rand()%255));
		else
		p.fillRect(rc, mx::Color::mapRGB(Surface(), 200, 200, 200));
		rc.point.x += 4;
		rc.point.y += 4;
		rc.rsize.width -= 8;
		rc.rsize.height -= 8;
		p.fillRect(rc, mx::Color::mapRGB(Surface(), 0xFF, 0xFF, 0xFF));

		/* draw text */

		if(data.length() == 0) return MSG_PROC;
		SDL_Font *print_font = font("verdana")->getFont();
		std::string temp_display;
		temp_display = data.substr(offset, data.length());

		string strX = "";
		if(has_focus)
		strX = "_";

		try {
			temp_display.insert(offset+cursor.x, strX);
		}
		catch(std::exception &e) {

		}

		string pw_temp_display;

		if(pw == true)
		for(size_t i = 0; i <  temp_display.length(); i++)
		{
			if(temp_display[i] == '_') {
				pw_temp_display += "_";
			}
			else {pw_temp_display += "*";}
		}

		if(pw == true) temp_display  = pw_temp_display;


		rt_val = SDL_PrintTextIdle(Surface().getSurface(),  print_font, rc.point.x+3, rc.point.y+2, dimensions.rsize.width, mx::Color::mapRGB(Surface(), tcolor),  temp_display.c_str());


		if(rt_val == 1) {
			// shift
			if(scroll == true) offset++;
		}
		return MSG_PROC;
	}

	void LineEdit::clearData() {
		data = "";
		offset = 0;
	}

	string LineEdit::insertChar(int index, char  c, string &str) {

		size_t i;

		if(str.length() == 0) {

			str += char(c);
			cursor.x = 1;
			cursor.y = 1;
			return str;
		}



		if(static_cast<unsigned int>(index) == str.length() ) {

			str += char(c); cursor.x++, cursor.y++;
			return str;

		}

		if(static_cast<unsigned int>(index) > str.length()) {
			str += char(c); cursor.x++, cursor.y++;
			return str;
		}

		string temp;

		for(i = 0; i < str.length(); i++) {

			if(i == static_cast<unsigned int>(index)) {

				temp += char(c);
				temp += str[i];
				cursor.x++, cursor.y++;
				continue;
			}

			temp += str[i];
		}

		return temp;
	}


	int LineEdit::mouseDown(int x, int y, int button) {

		Point pos = getPos();
		Point par =   parent->getPos();

		Point p(pos.x+par.x-50, pos.y+par.y-50);
		Rect rc (p, Size(size().width+50, size().height+50));

		if(rc.pointInRect(Point(x,y))) {
			setFocus(true);
			return MSG_FOCUS;
		}
		return MSG_IGNORED;
	}

	string LineEdit::removeChar(int index, string &str) {

		string temp;
		size_t i = 0;

		for(i = 0; i < str.length(); i++) {
			if(i != static_cast<unsigned int>((index)))
			temp += str[i];
		}
		return temp;
	}

	void LineEdit::passwordOn() {

		pw = true;

	}

	void ScrollBar::setScrollMax(int max) {
		maximum = max;
	}

	void ScrollBar::setScrollPos(int pos)  {
		position = pos;
	}

	int ScrollBar::renderEvent()   {
		mxPainter paint(Surface());
		paint.fillRect(dimensions, Color::mapRGB(Surface(),150,150,150));
		return MSG_PROC;
	}

	Link::Link(mxWidget *parent, std::string text) {
		this->text = text;
		setParent(parent);
		cfont = "verdana";
		setPos(Point(0,0));
		current = Color(255,255,255);
		hover = Color(255,0,0);
		normal = Color(255,255,255);
		over = false;
		show();
	}

	void Link::setDrawSurface(mxSurface &surf) {
		draw_surface = &surf;
	}

	void Link::setText(Color hover, Color normal, std::string text) {
		this->hover = hover, this->normal = normal;
		this->text = text;
		current = normal;

	}

	void Link::setFont(std::string cfont) {
		this->cfont = cfont;
	}

	void Link::clicked() {


	}

	int Link::mouseDown(int x, int y, int button) {

		Point p = dimensions.point;
		Point p2 = parent->getPos();

		int cx = p.x+p2.x;
		int cy = p.y+p2.y;

		Rect rc(cx,cy,depth,20);

		if (rc.pointInRect( Point(x,y) )) {

			clicked();
			return MSG_CLICKED;
		}
		return MSG_IGNORED;

	}
	int Link::mouseMove(int x, int y, int button) {

		Point p = dimensions.point;
		Point p2 = parent->getPos();

		int cx = p.x + p2.x;
		int cy = p.y + p2.y;

		Rect rc(cx,cy,depth, 20);

		if(rc.pointInRect(Point(x,y))) {
			current = hover;
			over = true;
		} else {
			current = normal;
			over = false;
		}

	}

	int Link::renderEvent() {

		if(visible == false)   return 0;

		Point p = dimensions.point;

		int x = p.x;
		int y = p.y;

		font(cfont)->printTextWidth_64(&depth, *draw_surface, x,y,200, Color::mapRGB(*draw_surface, current), text.c_str());
		if(over  == true) {

			mxPainter p(*draw_surface);
			p.lock();
			p.drawVertical(x, x+depth, y+15, current);
			p.unlock();
		}

		return 0;
	}

	int DisplayArea::printText(SDL_Surface *surf, int  x,  int y, int w, int h, Uint32 color, const char *src)
	{

		int prev_x = x;
		int offset_x = prev_x, offset_y = y;
		int width = 0, height = 0;
		int i,z,p;
		void *pbuf = lock(surf, surf->format->BitsPerPixel);
		number_one_offset = 0;
		for(p = 0; p < (int)strlen(src);  p++) {
				if(src[p] == '\n') {
						offset_x = prev_x;
						offset_y += fnt->my;
						if(number_one_offset == 0) number_one_offset = p;
						continue;
				}
				else if(src[p] == '\r') continue;
				else if(src[p] == '\t') {
					offset_x += 35;
					if(offset_x >= w-35) goto overflow;
					continue;
				}
				if(src[p] == 124) { width=7;  goto skip_to_here; }
				for(i = 0; i < fnt->mx; i++) {
						for(z = 0; z < fnt->my; z++) {
								if(fnt->letters[(int)src[p]].fnt_ptr[i][z] != fnt->tcolor) {
										if(offset_y+z > 0 && offset_x+i > 0 && offset_y+z < h && offset_x+i < w)
										setpixel(pbuf, (Uint32)offset_x+i, (Uint32)offset_y+z, color, surf->format->BitsPerPixel, surf->pitch);
										width=i;
										if(height < z)
												height=z;
								}
						}
				}

				skip_to_here:
				offset_x += width + 3;
				if(offset_x+width >= w-25) {
				overflow:
						offset_x = prev_x;
						offset_y += height;
						if(number_one_offset == 0)
						{
								number_one_offset = p;
						}

				}
				if(offset_y+height > h-10)
								return 1;

		}
		unlock(surf);
		return 0;
	}


	std::string mid(std::string s, size_t start, size_t stop)
	{
	        std::string temp;
	        size_t i;
	        for(i = start; i < stop; i++)
	                temp += s[i];
	        return temp;
	}

	int DisplayArea::renderEvent()
	{
		Point p = getPos();
		Point parent_pos = parent->getPos();
		Point final_point = Point(p.x+parent_pos.x, p.y+parent_pos.y);
		Size s = getSize();

		int rt = printText(surf->getSurface(), final_point.x, final_point.y,parent_pos.x+s.width, parent_pos.x+s.height,text_color.toInteger(),data.c_str());
		if(rt == 1) {

			 string buf = mid(data, number_one_offset+message.length(), data.length());
			 data = buf;
		}

	}

}
