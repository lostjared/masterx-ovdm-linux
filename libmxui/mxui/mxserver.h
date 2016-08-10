#ifndef _MX_SERVE_H_
#define _MX_SERVE_H_

#include<mx.h>
#include "mxdimension.h"
#include "mxmsgbox.h"



#include<list>

namespace mxui {

extern bool destroy;


	class mX : public mx::mxWnd   {

	public:
		std::string user_id;
		bool trans;

		mX(Size s, unsigned int flags=0);


		virtual ~mX() {
			std::vector<Dimension *>::iterator i, e = dimensions.end();
			for(i = dimensions.begin(); i != e; i++) {
				Dimension *d = *i;
				if(d->destroy == true) {
					std::cout << "[ mx ]: destroying non solid dimension: " << d->getname() << "\n";
					delete d;
					continue;

				}
			}
		}
		int addDimension(Dimension *d);
		virtual void eventPassed(SDL_Event &e);
		virtual void renderScreen();
		virtual int procEvent(widgetAction &we) { return MSG_IGNORED; }
		void setClose(const  bool b) {  close = b; }
		void setCurDim(int id) {

			if(locked == true || dimChangeOk(id) == false) return;
			dim_change = id;
			setTitle("MAsterX-ovDM - " + dimensions[cur_dimX]->getname());
			setDimChange();
		}
		virtual int  mouseDown(int  x, int  y, int button) { return 0; }
		virtual int  mouseMotion(int x,  int y, int state) {   return 0; }
		virtual int  keyDown(int  key, void *data) { return 0; }
		virtual int  keyUp(int key) { return 0; };
		void shift_left();
		void shift_right();
		const int curDim() const { return cur_dimX; }
		void lockChange();
		void unlockChange();
		void removeDimension(int id);
		bool dimChangeOk(int id);
		static mX *server;
		Size& size();
		void eraseDim(unsigned int id);
		void setFlip(bool flip) { no_Flip = flip; }
		void setDimChange() {
			change_dim  = true;
		}

		virtual void  erase(){}

		int  cur_dimX;

		void setFocus(unsigned int dim_id) {
			register unsigned int index = 0, size  = dimensions.size();
			for(index = 0; index < size; index++) {
				if(dimensions[index]->getID() == dim_id) {
					trans = false;
					setCurDim(index);
					;
					return;
				}
			}
		}

	protected:
		std::vector<Dimension *> dimensions;

		int cursor_x, cursor_y;
		int key;
		mx::mxRegularFont font;
		bool close;
		bool locked;
		std::list<int> rm_dim;
		Size si;
		bool no_Flip;
		int dim_change;
		bool change_dim;
		bool no_fade;
	};

}

#endif


