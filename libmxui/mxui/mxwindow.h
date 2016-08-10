#ifndef _MX_WINDOW_UI_X_
#define _MX_WINDOW_UI_X_

#include "mxwidget.h"
#include "mxwidget_type.h"
#include <mxpng.h>
#include "mxreg.h"

namespace mxui {

	class Dimension;

	interface window_events {
		void *event_handler;

		window_events() {}
		virtual ~window_events() { }


		virtual void on_create() {}
		virtual void on_destory() {}
		virtual void on_minimize() {}
		virtual void on_maximize() {}
		virtual void on_drag(int , int , int) {}
		virtual void on_close() {}
		virtual void on_dimswitch() {}
		//more
	};

	class Window : public mxWidget , public virtual mxFontDisplay, public window_events {

	protected:
		void (*render_cb)(std::string, Window *, mxSurface &);
		void (*event_cb )(std::string, Window *, widgetAction &we);


	public:

		std::string  cb_name;

		Window(mxWidget *parent = 0);
		virtual ~Window();

		int window_identifier;

		void setEvent_cb( void (*e)(std::string, Window *wnd, widgetAction &we ) ) { event_cb = e; }
		void setName(std::string name) { cb_name = name; }
		void setAlphaBlend(Dimension *d, bool flag);

		void createWindow(Color textcolor, string title, Point pos, Size size, Color titlebar, Color fillcolor,  Color backcolor);
		virtual int addWidget(mxWidget *widget);
		virtual int renderEvent();
		int renderAlpha();
		void renderChildren();
		virtual int procEvent(widgetAction &we);

		void setCallback( void(*render)(std::string id, Window *, mxSurface &) ) { render_cb = render; }
		void minimizeWindow();
		void maximizeWindow();

		const int getID() { return window_identifier; }
		void setID(const int x) { window_identifier = x; }

		virtual void on_minimize();
		virtual void on_maximize();
		virtual void on_close();
		virtual void on_destroy();
		virtual int  destroy();

		mxWidget *operator[](int ID);
		mxWidget *findID(int ID);
		mxWidget *operator()(int id);
		mxWidget *widgets(int id)  {
			mxWidget *w = findID(id);
			if(w == 0) throw mx::mxException<string>("[ mxui ]: error widget id invalid, possibly widget destoryed or using wrong id?\n");
			return w;
		}

		void removeTitleBar() { title_bar_z = true; }
		const Size& size() const { return dimensions.rsize; }

		const string &caption() const { return title; }
		void setCaption(string text);

		virtual int mouseMove(int x, int y, int button);
		virtual int mouseDown(int x, int y, int button);
		virtual int mouseUp(int x, int y, int button);
		virtual int keyUp(int key);
		virtual int keyDown(int key, void *data);

		const WINDOW_STATE state() const { return window_state; }
		void setWindowState(const WINDOW_STATE s) {

			if(s == MINI) {
				window_state = MINIMIZING;
				return;
			}
			window_state = s;
			change_state(s);

		}
		void setCaptionColor(Color col) { textcolor = col; }
		void useSkin(string path);
		void resizeWindow(int w, int h);
		void resizeWindow( Size s );
		void setMax(int x, int y) { max_x = x, max_y = y; }
		void canClose(bool var) { _close = var; }
		void canMax(bool var) { _max = var; }
		const bool moving() const { return move_window; }
		void setMovement(bool b) { move_window = b; }
		void setFocus(mxWidget *w) {
			focus = w;
			save_focus = focus;
		}
		void clrFocus() { focus->setFocus(false); focus = 0; }
		void setKeyInput(bool input) { keyInput = input; }
		/* movement data */

		Rect  movement_rect;
		bool moved_window;
		void setAlpha(float a) { alpha = a; }
		void setTitleColor( Color col ) { text_color = col; }
		virtual void copyWidgetSurface(mxSurface &surface);
	protected:
		mxWidget *focus, *save_focus;

		void change_state(const WINDOW_STATE &s);

		string title;
		Color textcolor, titlebar, fillcolor,  backcolor;
		uiList_ptr<mxWidget *> children;

		int checkItemDown(int x, int y, int button);
		Color text_color;
	public:
		const bool gone() const { return goner; }
		void clr() { goner = false; }
		void gonerOn() { goner = true; }
		static int  identifer;
	private:
		bool keyInput;
		bool goner;
		bool _close;
		bool _max;
		bool exit_win;
		int exit_msg;
		int max_x, max_y;
		widgetAction we;
		bool render_alpha;
		mx::mxSurface alpha_surf;
		mx::mxSurface pre_render;
		Dimension *dim;
		float alpha;
		bool drawn_already;

		friend class Dimension;

		void redraw_surface();

public:	bool move_window;

private:
		bool title_bar_z;
		WINDOW_STATE window_state;

		/* skin variables */

		string skin_path;

		mxSurface title_bar;
		mxSurface windowbg;

		mxSurface wbar;

		/* skin functions */

		bool skin_loaded;

		class windowState {
		public:

			windowState() {
				memset(this, 0, sizeof(windowState));
			}

			windowState( Rect rc, Point pos) {
				this->rc = rc;
				this->pos = pos;
				this->restore_set = 0;
			}
			Rect rc;
			Point pos;
			int restore_set;
		protected:

		};

		windowState savestate;
		bool mini_me;
		int mini_max;


	};


}



#endif
