#ifndef _MX_DIMENSION_XH_
#define _MX_DIMENSION_XH_
#include "mxwindow.h"
#include<string>
#include"mxplugin.h"
#include<vector>
/*
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
*/
namespace mxui {


	using mx::mxWnd;
	using std::string;

	interface DimensionEvents {

		DimensionEvents() {}
		virtual ~DimensionEvents() {}
		virtual void drawScreen(mxSurface &) {}
		virtual void onClose() { }
		virtual void logic() { }
	};

	class mX;


	class Dimension : public DimensionEvents {

		static unsigned int dim_offset;
	public:

		bool destroy;
		/* Dimensions must have a name, say your 'programs name' */
		Dimension();
		Dimension(string name, mxWnd *p_wnd, Size size);
		virtual ~Dimension();

		virtual void clearDimension();
		virtual int addWindow(Window *wnd);
		virtual void eventProc(widgetAction &we);

		virtual void keyDown(int key, void *data);
		virtual void keyUp(int key);

		virtual void mouseMove(int x, int y, int button);
		virtual int mouseDown(int x, int y, int button);
		virtual void mouseUp(int x, int y, int button);

		Window *findWindow(string name);
		Window *operator[](string window);
		Window *findWindow(int id);
		Window *operator[](int id);
		Window *window(int id);
		virtual void after_render() { }
		void sendMessage(Window *window, widgetAction &we);
		void setFocus(int id);
		// change your dimensions name
		void setName(const string arg) { name = arg; windows.list_id = arg; }
		virtual void drawScreen(mxSurface &drawScreen);
		virtual void  onClose() { }
		void minimizeAll();
		void restoreAll();
		void setBackground(std::string name);
		void setBackgroundColor( Color col ) { backcolor = col; }
		void render();
		virtual void logic() { }
		const string getname() const { return name; }
		const string& dimensionName() const { return name; }
		const int windowCount() { return windows.active_count(); }
		void  clearMove(Window *w) { w->setMovement(false); w->move_window = false;  }
		mx::mxSurface &Surface() { return background; }
		protected:
		public:			uiList_ptr<Window *> windows;
	    protected:		Color backcolor;
		mxSurface background, background_image;
		bool background_image_on;
    public:
	    unsigned int getID() { return my_offset; }

		mxWnd *wnd;
	public:
		Window *focus_window;
		friend class Window;


	private:
		void shade_window();
		int ident_offset;
		int newID();
		string name;
		Window *movement_window;
		int first_run;
		int my_offset;


	};


	class DimensionDock : public Window {

	public:
		DimensionDock(Dimension *parent, uiList_ptr<Window *> *windows, int w, int h, int extra_h, mxSurface &render);
		virtual int renderEvent();
		virtual int mouseDown(int x, int y, int button);
		virtual int mouseMove(int x, int y, int button);
		void initRun(void (*runs)(std::string)) { run = runs; }
	protected:
	    uiList_ptr<Window *> *windows;
	    int width, height;
	    mxSurface &surf;
	    Dimension *parent;
	    Size desktop;
	    mxSurface left,bar;
	    int offset;
	    bool menu_light;
	    bool draw_menu;
	    std::vector<std::string> dir_vec;
	    int pos_s;
	    void (*run)(std::string script);


	};





}




#endif
