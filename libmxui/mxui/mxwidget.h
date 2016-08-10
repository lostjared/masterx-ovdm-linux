#ifndef _MX_WIDGET_H_X
#define _MX_WIDGET_H_X

// comment out for system fonts
#define LOCAL_FONTS

#include<mx.h>
#include"wcode.h"
#include<boost/checked_delete.hpp>
#ifndef LOCAL_FONTS
#define FONT_PATH "/usr/local/mxf"
#else
#define FONT_PATH "fonts"
#endif





namespace mxui {

	using mx::Rect;
	using mx::Size;
	using mx::Point;
	using mx::mxSurface;
	using mx::Color;

	template<typename Item>
	class listItem {
	public:
		listItem() { item = 0; next = 0; prev = 0; release = true; }
		listItem(const Item &item) { this->item = item; next = 0; prev = 0; release = true; }
		~listItem() {


		}

		bool release;
		Item item;
		listItem *next;
		listItem *prev;
	};

	template<typename Type>
	class uiList_ptr {
	public:

		std::string list_id;

		uiList_ptr()
		{
			counter = 0;
			root = 0;
			tail = 0;
			list_id="";

		}

		~uiList_ptr()
		{

			clear();

		}

		void setNoRemove() {
			listItem<Type> *rt = this->rootptr(), **ret = &rt;
			while( (rt = *ret)   != 0 ) {
				rt->release = false;
				ret = &rt->next;
			}
		}

		void setRemove() {
			listItem<Type> *rt = this->rootptr(), **ret = &rt;
			while( (rt = *ret)   != 0 ) {
				rt->release = true;
				ret = &rt->next;
			}
		}

		void clear() {
			if(root == 0) return;

			std::cout << "[ mxui ]: clearing list cache for: " << list_id << "\n";

				listItem<Type> *pz = rootptr(), **ptr = &pz;
				while( (pz = *ptr) != 0 ) {
				ptr = &pz->next;
				if(pz->item && pz->release == true) boost::checked_delete(pz->item);//delete pz->item;
				if(pz != 0) boost::checked_delete(pz);//delete pz;
			}

			root = 0;
			counter = 0;
		}

		void append(const Type &type)
		{
			listItem<Type> *t = root, **tt = &t, *var = 0;

			if(root == 0) { root = new listItem<Type>(type); root->prev = 0; tail = root; counter++; return; }

			while ( (t = *tt ) != 0 ) {
				tt = &t->next;
				var = t;
			}
			t = new listItem<Type>(type);
			*tt = t;
			t->prev = var;
			counter++;
			tail = *tt;
		}

		int remove(Type &t) {

			/* implement later since controls are tied to window objects */


			return 0;

		}

		void shift_right() {

			if(count() == 0  || count() == 1) return;

			listItem<Type> *it = rootptr(), **ptr_it = &it;

			std::vector<Type> vec;

			while ( (it = *ptr_it) != 0 ) {
				vec.push_back( it->item );
				ptr_it = &it->next;
			}

			it = rootptr(), ptr_it = &it;

			size_t offset = 0;

			it->item = vec[ vec.size() - 1 ];
			ptr_it = &it->next;

			while ( (it = *ptr_it) != 0) {
				it->item = vec[offset++];
				ptr_it = &it->next;
			}
		}

		listItem<Type> *rootptr() { return root; }
		listItem<Type> *tailptr() { return tail; }
		const int count() const { return counter; }
		const int active_count()  {

			listItem<Type> *ptr = this->rootptr() , **table = &ptr;
			int count = 0;

			while ( (ptr = *table) != 0) {

				if(ptr->item->Visible() == true) count++;
				table =  &ptr->next;
			}
			return count;
		}

	protected:
		listItem<Type> *root, *tail;
		unsigned int counter;
	};

	struct widgetAction {

		ACTIONCODE actionCode;
		Point cursor;
		int key;
		void *extra;
		int parent_id;
		int cursor_x, cursor_y, button;
	};

	interface mxWidgetActions {

		virtual ~mxWidgetActions() { }
		int keyDown(int key, void *data);
		int keyUp(int) { return MSG_IGNORED; }
		int mouseMove(int , int , int) { return MSG_IGNORED; }
		int mouseDown(int , int , int ) { return MSG_IGNORED; }
		int mouseUp(int , int , int ) { return MSG_IGNORED; }
		int renderEvent() { return MSG_IGNORED; }
		int procEvent(widgetAction) { return MSG_IGNORED; }
		int destroy() { return MSG_IGNORED;  }
		int mouse_clicked() { return MSG_IGNORED; }
	};


	enum { TYPE_EDIT , TYPE_OTHER };

	class mxWidget : public mxWidgetActions {

	public:
		mxWidget( mxWidget *parent = 0 );
		virtual ~mxWidget();

		void createWidget(Point spot, Size dim);
		void createWidget(Rect r);


		void setParent(mxWidget *parent);
		mxWidget *getParent();

		void setSize( Size s );
		void setPos( Point p );
		void setRect( Rect r);

		const Point &getPos() const;
		const Size &getSize() const;

		const bool Visible() const { return visible; }

		void show();
		void hide();
		Rect  &rect() { return dimensions; }
		Point &point() { return dimensions.point; }
		Size  &size() { return dimensions.rsize; }
		Rect  sizeRect() { Rect rc; rc.point = Point(0, 0); rc.rsize = size(); return rc; }

		inline mxSurface &drawSurface()  { return draw_surface; }
		void setIdentifier(int id);
		virtual void copyWidgetSurface(mxSurface &surface);
		virtual int renderEvent();
		virtual int mouseMove(int,int,int) { return MSG_IGNORED; }
		virtual int mouseDown(int,int,int) { return MSG_IGNORED; }
		virtual int mouseUp(int,int,int) { return MSG_IGNORED; }
		mxSurface &Surface() { return draw_surface; }
		virtual int destroy() { draw_surface.cleanSurface(); return MSG_PROC; }
		virtual int procEvent(widgetAction &) { return MSG_IGNORED; }
		virtual int keyDown(int , void *) { return MSG_IGNORED; }
		virtual int keyUp(int) { return MSG_IGNORED; }
		virtual int mouse_clicked() { return MSG_IGNORED; }
		const int ident() const { return this_id; }
		const int identifier() const { return  widget_id; }
		virtual void setFocus(bool) { }
		const bool moveable() const {

			if(visible == true)
				return true;

				return false;
		}

		const int &getType() const { return type; }
		int action;
		Rect dimensions;
	protected:
		int type;
		mxWidget *parent;
		mxSurface draw_surface;
		bool visible;

		static int widget_id;
		int this_id;


	};



}



#endif

