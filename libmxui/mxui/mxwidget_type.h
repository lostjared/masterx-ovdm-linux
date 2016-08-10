#ifndef MX_WIDGET_TYPE_X
#define MX_WIDGET_TYPE_X

#include"mxwidget.h"
#include<string>
#include"mx_font.h"
#include<map>
#include<cassert>
#include<unistd.h>
#include<dirent.h>
#include<mxl.h>
#include<vector>
#include<string>
#include<mxinterface.h>
#include<string.h>

namespace mxui {

	using std::string;
	using mx::mxRegularFont;

	/*
	 *
	 *
	 * font display will be inherited by different subclasses of type widget
	 * initalize there own fonts, than call the default load function to load
	 * the system font or call the constructor.
	 *
	 */
	class mxFontDisplay {

	public:

	static int ref_count;

		mxFontDisplay();
		virtual ~mxFontDisplay();

		static void loadDefaults(string path)
		{
			DIR *d = opendir(path.c_str());

			if(d == 0)
				throw mx::mxException<std::string>(" error mxf directory is missing,\n be sure that when you installed this application /usr/local/mxf was created.\n");


				std::cout << "[ mxui ]: loading fonts from path: " << path << "\n";

				struct dirent *dr;
				while ((dr = readdir(d))) {
                    std::string name = dr->d_name;
					if(strstr(name.c_str(), "mxf") != 0)
					{
						string fullpath = path+"/"+name;

						std::string name = fullpath.substr(fullpath.rfind("/")+1, fullpath.length());
						std::string font_name = name.substr(0, name.rfind(".mxf"));

						mxRegularFont *font = 0;
					try {

						font = new mxRegularFont;
						std::cout << "[ mxui ] loading font: " << font_name;
					    assert(font->loadFont(fullpath) == true);
						fonts[font_name] = font;
						std::cout << " successful \n";
					}
					catch(...)
					{
						delete font;

					}
				}

			}
		}

		bool isFontLoaded(string name);
		void setFont(string name);

		// load default font
		virtual bool loadFont(string fullpath);
		mxRegularFont *font() { return fonts[cur_font]; }
		mxRegularFont *font(string name) {
			if(isFontLoaded(name))
			return fonts[name];
			return 0;
		}

		static bool loaded;
		static mxRegularFont system;
		static std::map<std::string, mxRegularFont *> fonts;
	protected:
		std::string cur_font;

	};

	template<typename Class>
	class Action_Connector {
	public:

		Action_Connector() { cls = 0; action_type = 0; }

		Action_Connector(Class *cls, int (Class::*action)(widgetAction &we)) {
			this->cls = cls;
			action_type = action;
		}

		int operator()(widgetAction &we) {
			return (*cls.*action_type)(we);
		}

		int call(widgetAction &we) {
			return (*cls.action_type)(we);
		}
	protected:
		Class *cls;
		int (Class::*action_type)(widgetAction &we);


	};









	class Button : public mxWidget, public virtual mxFontDisplay   {


	public:
		Button(mxWidget *parent = 0);
		Button(mxWidget *parent, Rect dimensions, string text);
		void createButton(Rect dimensions, string text, Color backcolor, Color forecolor, Color textcolor);

		Color backcolor, forecolor;
		Color textcolor;

		virtual void buttonClicked();

		virtual int renderEvent();
		virtual int mouseMove(int x, int y, int button);
		virtual int mouseDown(int x, int y, int button);
		virtual int mouseUp(int x, int y, int button);
		virtual int clicked();
		virtual int mouse_clicked();
		void  setname(std::string name);
		void  setCallback( void (*cbx)(string) );

		string text;

		void  setTitle(string   text) { this->text = text; }
		void  SetTextColor(int  r, int g, int  b) {  textcolor = Color(r,g,b); }



	protected:
		int mode;
		int mode_clicked;
		std::string obj_name;
		void (*cb)(std::string);


	};

	template<typename Type>
	class ActionButton : public Button {

	public:
		ActionButton(mxWidget *parent = 0) : Button(parent)
		{

		}


		Action_Connector<Type> connector;
		void setConnector(Type *type, int (Type::*action)(widgetAction &we) )
		{
			connector = Action_Connector<Type>(type, action);

		}

		virtual void buttonClicked()
		{
			widgetAction we;
			memset(&we, 0, sizeof(we));

			we.actionCode = MX_BUTTON_CLICKED;
			we.extra = (void*) this;
			we.parent_id = parent->ident();
			connector(we);

		}

	protected:

	};



	class Label : public mxWidget, public mxFontDisplay {

	public:
		Label(mxWidget *parent, string text, Point pos);
		void setText(Color textcolor, string str);
		string getText();

		void setDrawSurface( mxSurface &draw_surf );
		virtual int renderEvent();

	protected:
		std::string text;
		mxSurface *drawToSurface;
		Color textcolor;

	};

	class Image : public mxWidget, public virtual mxFontDisplay {
	public:
		Image(mxWidget *parent = 0);
		Image(mxWidget *parent, string filename, Point pos);

		bool loadImage(string filename);
		void stretchImage(Size new_size);
		virtual int  renderEvent();
	protected:
		mxSurface render_surface;
	};

	class LineEdit : public mxWidget,public virtual mxFontDisplay {
	public:
		LineEdit(mxWidget *parent, Point pos, int width);
		void setText(string text);
		void setColor(Color col);
		void setCursorPos(Point cur);
		virtual int keyDown(int key, void *data);
		virtual int renderEvent();
		void clearData();
		virtual void  enterPressed();
		const string getText() const { return data; }
		const int getLength() const { return data.length(); }
		void setFocus(bool set);
		virtual int mouseDown(int  x, int y, int button);
		void passwordOn();
		void setWidth(const int width) { wid = width; }
		Point cursor;
		int offset;
		bool pw;

	protected:
		string data;
		Color tcolor;
		bool has_focus;
		string insertChar(int index, char  c, string &str);
		string removeChar(int index, string &str);
	private:
		int wid;
		int rt_val;
		bool scroll;

	};

	template<typename Type>
	class ActionLine : public LineEdit {
	public:
		ActionLine(mxWidget *parent, Point pos, int width) : LineEdit(parent, pos, width) {

		}
		Action_Connector<Type> connector;
		void setConnector(Type *type, int (Type::*action)(widgetAction &we))  {
			connector = Action_Connector<Type>(type, action);
		}
		virtual void enterPressed() {
			widgetAction we;
			memset(&we, 0, sizeof(we));
			we.actionCode = MX_EDIT_ENTER;
			we.extra = (void*)  this;
			we.parent_id  = parent->ident();
			connector(we);
		}
	};

	class ScrollBar : public mxWidget, public virtual mxFontDisplay {

	public:
		ScrollBar(mxWidget  *parent, Size s) {
			setParent(parent);
			createWidget(Rect(Point(0,0), s));
		}

		void setScrollMax(int max);
		void setScrollPos(int pos);

		virtual int  renderEvent();

	protected:
		int maximum;
		int position;
	};

	class Link : public mxWidget, public virtual mxFontDisplay {

	public:
		Link(mxWidget *parent, std::string text);
		void setDrawSurface(mxSurface &surf);
		void setText(Color hover, Color normal, std::string text);
		void setFont(std::string cfont);
		virtual int mouseDown(int x, int y, int button);
		virtual int mouseMove(int x, int y, int button);
		virtual int renderEvent();
		virtual void clicked();

	protected:
		mxSurface *draw_surface;
		std::string text, cfont;
		Color hover, normal;
		Color current;
		int depth;
		bool over;

	};

	class CheckBox : public mxWidget,  public virtual mxFontDisplay {
	public:
		CheckBox(mxWidget *parent, Size s)   {
			setParent(parent);
			createWidget(Rect(Point(0,0), s));
			id =  0;
		}

		void setRenderSurface(mx::mxSurface *s) { surf = s; }
		void setCheck(int value) { if(value > 1) id =  150; }
		const bool getCheck() const { if(id == 150) return  true;  else return false; }

		virtual int  renderEvent() {
			mx::mxPainter p(*surf);

			p.fillRect(dimensions, Color(125,125,125));
			Rect r = dimensions;
			r.point.x   += 5 ;
			r.point.y   += 5;
			r.rsize.width  -= 10;
			r.rsize.height -= 10;
			p.fillRect(r, Color(175,175,175));



		if(id ==  150)  {
			font()->printText(*surf, dimensions.point.x+10,dimensions.point.y+3 ,mx::Color::mapRGB(*surf, 255,  0, 0),"X");
		}
			return MSG_PROC;
		}

		virtual int mouseUp(int x, int y, int button) {

			if(button == SDL_BUTTON_LEFT) {
				Point p1 = getPos(), p2 = parent->getPos();
				Size sz = getSize();
				Rect rc(p1.x+p2.x, p1.y+p2.y, sz.width, sz.height);
				if(rc.pointInRect(Point(x,y))) {
						if(parent != 0) {

							if(id == 150)  id = 0;
							else id = 150;

							return MSG_PROC;
						}
				}

			}

			return MSG_IGNORED;
		}

	protected:

		int id;
		mxSurface *surf;
	};

	class DisplayArea : public mxWidget, public virtual mxFontDisplay {

	public:

		explicit DisplayArea(mxWidget *w)
		{

			parent = w;
			fnt = font()->getFont();
			number_one_offset = 0;
			text_color = Color(255,255,255);
			dimensions.point.x = 0;
			dimensions.point.y = 0;
		}

		void setColor(Color col) { text_color = col; }
		virtual int renderEvent();
		int printText(SDL_Surface *surf, int  x,  int y, int w, int h, Uint32 color, const char *src);
		void  setDrawingSurface(mxSurface &surf)  { this->surf = &surf; }
		void  concat(string  d) {
			message = d;
			data += message;
		}
	protected:
		string data;
		SDL_Font *fnt;
		mxSurface *surf;
		int number_one_offset;
		Color text_color;
		string message;
	};

}


#endif

