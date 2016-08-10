#include "mxserver.h"
#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include<mx.h>
#include<mx_types.h>
#include<mxinterface.h>
#include<sstream>
#include"mxwidget_type.h"
#include "mxwidget.h"
#include<functional>
#include<algorithm>

namespace mxui {


	using mx::mxPainter;
	using std::ostringstream;


	mX  *mX::server = 0;


	mX::mX(Size s,unsigned int flags) : mxWnd(s.width, s.height, flags)
	{

		cur_dimX = 0;
		if( ! font.loadFont(string(FONT_PATH)+"/verdana.mxf") ) throw mx::mxException<string>("libmx: Error could not load font file: " + string(FONT_PATH) + "/verdana.mxf , possibly wrong font path set?\n");
		close = true;
		key = 0;
		enableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
		server = this;
		locked = false;
		si = s;
		no_Flip = false;
		dim_change  = 0;
		change_dim = false;
		destroy = false;

	}

	int mX::addDimension(Dimension *d)
	{
		dimensions.push_back(d);
		return dimensions.size()-1;
	}

	void mX::eventPassed(SDL_Event &e)
	{

		if(dimensions.size() == 0) return;

		Dimension *cur_dim = dimensions[cur_dimX];

		assert( cur_dim != 0 );


		if(close  ==  true && e.type == SDL_QUIT) { quit(); return; }

		unsigned char *keys = SDL_GetKeyState(0);

		if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE && keys[SDLK_LSHIFT]) {
			quit(); return;
		}


#if (DEBUG_MODE == 1)

		if(e.type == SDL_MOUSEMOTION) {
			cursor_x = e.motion.x;
			cursor_y = e.motion.y;
		}

		if(e.type == SDL_KEYDOWN)
			key = e.key.keysym.sym;
		if(e.type == SDL_KEYUP)
			key = 0;

#endif



		switch(e.type) {
		case SDL_KEYDOWN:
			cur_dim->keyDown(e.key.keysym.sym, (void*)&e);
			keyDown(e.key.keysym.sym, (void*)&e);
			break;
		case SDL_KEYUP:
			cur_dim->keyUp(e.key.keysym.sym);
			keyUp(e.key.keysym.sym);
			break;
		case SDL_MOUSEMOTION:
			cur_dim->mouseMove(e.motion.x, e.motion.y, e.motion.state);
			mouseMotion(e.motion.x, e.motion.y, e.motion.state);
			break;
		case SDL_MOUSEBUTTONDOWN:
			cur_dim->mouseDown(e.button.x, e.button.y, e.button.button);
			mouseDown(e.button.x,e.button.y,e.button.button);
			break;
		case SDL_MOUSEBUTTONUP:
			cur_dim->mouseUp(e.button.x, e.button.y, e.button.button);
			break;
		}

	}

	void mX::eraseDim(unsigned int id) {

		if(cur_dimX != 2)
		setCurDim(1);

		unsigned int pos = 0;

		std::vector<mxui::Dimension *>::iterator i;

		for(i = dimensions.begin(); i != dimensions.end(); i++)  {

			mxui::Dimension *d = (*i);

			if(d->getID()==id) {

				dimensions.erase(i);
				return;
			}

			pos ++;
		}


	}

	void mX::shift_left() {

		if(locked == true) return;


		int  testX = cur_dimX;

		if(dimChangeOk(cur_dimX-1) == false)   {

			while(testX > -1) {

				if(dimChangeOk(testX) == true)
					this->setCurDim(testX);

				testX--;
			}

			if(testX == -1) return;

		}
		else
		if(cur_dimX > 0) setCurDim(cur_dimX-1);

		setTitle( "MAsterX-ovDM - "  + dimensions[cur_dimX]->getname() );

	}
	void mX::shift_right() {

		if(locked == true) return;

		int testX = cur_dimX;

		if(dimChangeOk(cur_dimX+1) == false) {

			while(static_cast<unsigned int>(testX) < dimensions.size()-1) {

				if(dimChangeOk(testX) == true)
					this->setCurDim(testX);

				testX++;
			}

			if(testX == -1) return;
		}
		else
		if(static_cast<unsigned int>(cur_dimX) < dimensions.size()-1) setCurDim(cur_dimX+1);

		setTitle( "MAsterX-ovDM - " + dimensions[cur_dimX]->getname() );


	}

	void mX::renderScreen()
	{
		if(dimensions.size() == 0) return;




		Dimension *cur_dim = dimensions[cur_dimX];
		cur_dim->render();
		cur_dim->drawScreen(front);



		if(no_Flip==true)  front.Flip();
	}

	void mX::lockChange() {
		locked = true;
	}
	void mX::unlockChange() {
		locked = false;

	}

	void mX::removeDimension(int id) {

		rm_dim.push_back(id);

	}


	bool mX::dimChangeOk(int id) {


		if ( std::find(rm_dim.begin(), rm_dim.end(), id)  == rm_dim.end()) return true;
		return false;

	}


	Size &mX::size() { return sizevar; }









}


