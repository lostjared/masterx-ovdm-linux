/*   MasterX Console - (C) 1999-2008 (GPL)  Jared Bruni
 *
 * includes embedded Python interpreter
 * use to manipulate the user environment
 * the shell works similar to idle
 * except you can call scripts to be run in the MasterX environment
 * for imported Python scripts to work PYTHONPATH must be set
 * working on automating that task
 * use Bourne shell script 'RunMasterX' to automate exporting path for Python
 */


#ifdef HAVE_CONFIG_H
#include<config.h>
#endif

#include"mxassert.h"
#include"console.h"
#include<mx.h>
#include<stdarg.h>
#include<keyfilter.h>
#include"mxuser.h"
#include<mx_tok.h>
#include<masc++.h>
#include<dirent.h>
#include<unistd.h>
#ifndef _WIN32
#include<sys/types.h>
#include<pwd.h>
#include<unistd.h>
#include<sys/wait.h>
#endif

#include<sstream>
#include"masterx.h"
#include"mxdimension.h"
#include<mxpng.h>
#include<mxinterface.h>
#include<sstream>
#include<boost/scoped_ptr.hpp>
#include<boost/thread.hpp>
#include"mxwidget_type.h"
#include<mx_font.h>


namespace System {
		masterx::Console *console = 0;
}


namespace masterx {
	using namespace mx;
	using namespace mxui;
	using namespace boost::python;
	using System::console;

	boost::mutex mlock;

	void bail() {
		mX::server->quit();
	}

	void Console::user_inputed(string data)
	{
		try {

			last_input = data;
			std::cout << data << "\n";
			Console::printf("\n");
			pyExpr(data.c_str());
		}
		catch   (...) {
			Console::printf(" a error has occoured\n");
		}

		pyExpr("sys.stdout.done()");
	}
	void Console::pyExpr(string total) {
		PyRun_SimpleString(total.c_str());
	}
	void Console::inputPyExpr(string total) {}
	void window_callback(string name, Window *window, mxSurface &surface)   {
		std::ostringstream ss;
		ss << name << "." << "render()";
		System::console->pyExpr(ss.str().c_str());
	}
	class  mxout {
	public:
		class bufferedWriter {
		public:
			bufferedWriter(string str) {
				data = str;
			}
			void operator()()  {
				boost::mutex::scoped_lock lock(mlock);
				string left;
				while ( data.length() > 0 ) {
					int d = data.find("\n");
					if(d == -1 && data.length() < 100) {
						console->printf("%s", data.c_str());
						break;
					}
					if(d > 100) d = 100;
					left = data.substr(0, d+1);
					data = data.substr(d+1, data.length()-d);
					console->printf("%s", left.c_str());
					SDL_Delay(75);
				}
				console->setToEnd();
			}
		protected:
			string data;
		};
		mxout() {};
		void write(string zs) {
			__assert__( console != 0 );
			bufferedWriter bw(zs);
			boost::thread tr( bw );
		}
		void done() {
			//console->printf(">>");
			console->pyExpr("sys.stdout.write( \">>\" )");
		}
		void  setfont(string f) {
			__assert__( console != 0 );
			if( console->font(f) )
				console->setfont(f);
		}
		void setcolor(int r, int g, int b) {
			__assert__ ( console != 0 );
			console->setcolor(r,g,b);
		}
	};
	class mxDim;
	class dynDim : public Dimension {
	public:
		dynDim() : Dimension() { destroyed = false; }
		virtual ~dynDim() {
			if(destroyed == false)
			masterX_server::server->eraseDim(this->getID());
		}
		friend class mxDim;
		bool  destroyed;
	};
	class windowWidget {
		public:
			mxWidget *ptr;
	};
	/* C++ classes  to be exported to Python */
	class mxSurf {
	public:
		mxSurf()
		{
		}
		mxSurf(string str) {
			load(str);
		}
		void load(string str)
		{
			SDL_Surface *_surface = SDL_LoadBMP(str.c_str());
			if(_surface == 0) throw mx::mxException<std::string>("Error could not load graphic: " + str);
			surface = _surface;
		}
		void create(int w, int h) {
			surface.createSurface(w, h);
			mx::mxPainter p(surface);
			p.fillRect(0, Color(0,0,0));
		}
		void loadpng(string str)
		{
			SDL_Surface *surf = 0;
			mx::mxPng p;
			if(p.pngOpen(str.c_str())) {
				surf = p.LoadPNG();
				p.pngClose();
			}
			if(surf == 0) throw mx::mxException<std::string>("Error could not load png graphic: " + str);
			surface = surf;
		}
		void release() {
			surface.cleanSurface();
		}

		int width() { return surface.size().width; }
		int height() { return surface.size().height; }

		mx::mxSurface surface;
	};


	void windowEvents(string name, Window *window, widgetAction &we) {
		std::ostringstream stream;
		stream << name << ".";
		switch(we.actionCode) {
		case MX_KEYDOWN:
			stream << "keydown(" << we.key << ")";
			break;
		case MX_KEYUP:
			stream << "keyup(" << we.key << ")";
			break;
		case MX_MOUSEDOWN:
			stream << "mousedown(" << we.cursor_x << "," << we.cursor_y << "," << we.button << ")";
			break;
		case MX_MOUSEUP:
			stream << "mouseup(" << we.cursor_x << "," << we.cursor_y << "," << we.button << ")";
			break;
		case MX_MOUSEMOVE:
			stream << "mousemove(" << we.cursor_x << "," << we.cursor_y << "," << we.button << ")";
			break;
		default:
			break;
		}
		console->pyExpr(stream.str());
	}
	class mxWin {
		mx::mxPainter p;
		public:

			mx::mxRegularFont *font;

			~mxWin() {
				delete font;
			}

			mxWin()
			{
				window  = new Window();

				__assert__ ( window != 0 );
				font = new mxRegularFont();
				font->loadFont("fonts/system.mxf");
			}
			mxWin(string title, int x,  int y, int w, int h)
			{
				window = new Window();
				__assert__( window != 0 );
				create(title,x,y,w,h);
				font =  new mxRegularFont();
				font->loadFont("fonts/system.mxf");
			}
			void create(string title, int  x, int y, int w, int h) {
				window->createWindow(Color(254,254,254), title, Point(x, y), Size(w, h), Color(255,255,255), Color(0,0,0), Color(0,0,255));
			}
			void settitle(string t) {
				window->setCaption(t);
			}
			void setpos(int x, int y) {
				window->setPos(Point(x,y));
			}
			void setsize(int w, int h) {
				window->resizeWindow(w,h);
			}
			void  setcallback(string name) {
				window->setName(name);
				window->setCallback( window_callback );
				window->setEvent_cb( windowEvents );
			}

			// default callbacks
			void keydown(int key) {}
			void keyup(int key) {}
			void mousemove(int x, int y, int button) {}
			void mousedown(int x, int y, int button) {}
			void mouseup(int x, int y, int button) {}
			void render() {}


			void copysurf(mxSurf &surf, int x,  int y, int w, int  h)
			{
				SDL_Rect rc = { x,y,w,h };
				window->Surface().copySurface(surf.surface, 0, &rc);
			}
			void copyresizesurf(mxSurf &surf, int x, int y, int w, int h)
			{
				SDL_Rect rc = { x,y,w,h };
				SDL_Rect src = { 0, 0, surf.surface.size().width, surf.surface.size().height };
				window->Surface().copyResizeSurface(surf.surface, &src, &rc);
			}
			void fillrect(int  x, int y, int  w, int h, int r, int g,  int   b)  {
				mx::mxPainter p(window->Surface());
				p.fillRect(Rect(x,y,w,h), mx::Color(r,g,b));
			}
			void lock() {
				p.setSurface(window->Surface());
				p.lock();
			}

			void unlock() {
				p.unlock();
			}
			void  setpixel(int   x, int y, int r, int g,   int  b) {
				p.setpixel(x,y, mx::Color(r,g,b));
			}
			void useskin(string  sk) {
				try {
					window->useSkin(sk);
				}
				catch( ... ) {
					console->printf("\ninvalid skin\n");
				}
			}
			void bail() {
				mX::server->quit();
			}
			void addwidget(windowWidget &w) {
				window->addWidget(w.ptr);
			}
			void printtext(int x, int y, int r, int g, int b, string text)
			{
				font->printText(window->Surface(), Point(x,y), Color(r,g,b), text.c_str());
			}
			Window *window;
			Window *getWindow() { return window; }

		};

	void click_callback(string name) {
		std::ostringstream stream;
		stream << name << "." << "button_clicked()";
		System::console->pyExpr(stream.str().c_str());
	}
	class _Button {
	public:
		_Button() {}

		_Button(const _Button &b)  { }
		_Button(mxWin &w) :   bt ( new Button (w.window) ) {
			__assert__ ( bt != 0 );
			w.window->addWidget(bt);
		}
		void  createbutton(int  x,  int y, int w,  int  h, string name) {
			bt->createButton(Rect(x,y,w,h), name, Color(150,150,150), Color(200,200,200), Color(0,0,0));
			settextcolor(255,0,0);
		}
		void settext(std::string text) {
			bt->setTitle(text);
		}
		void settextcolor(int r, int g, int b) {
			bt->SetTextColor(r,g,b);
		}
		void setcallback(std::string name) {
			bt->setname(name);
			bt->setCallback(click_callback);
		}
		void render() {
			bt->renderEvent();
		}
	protected:
		Button  *bt;
		std::string obj_name;
	};
	class mxDim {
		public:

			~mxDim() {

				//  deoncsturct
			}

			mxDim() :  mx_dim( new dynDim() ) {
				__assert__( mx_dim != 0 );
				mx_dim->setBackgroundColor( Color(0,0,0) );
			}
			mxDim(const mxDim &d) {}
			mxDim(string background) : mx_dim ( new dynDim() ) {
				mx_dim->setBackground(background);
			}

			void settitle(string name) {
				mx_dim->setName(name);
			}

			void sizetoscreen()
			{
				mx_dim->background.createSurface(mX::server->size());
			}
			int width() {
				return mx_dim->background.size().width;
			}
			int height() {
				return mx_dim->background.size().height;
			}
			void setbackground(string bg) {
				mx_dim->setBackground(bg);
			}
			void setbgcolor(int r, int g, int b) {
				mx_dim->setBackgroundColor( Color(r,g,b) );
			}
			void add() {
				masterX_server::server->addDimension(mx_dim.get());
			}
			void addwindow(mxWin &win)  {
				mx_dim->addWindow(win.window);
			}

			void setfocus() {
				mX::server->setFocus(mx_dim->getID());
			}

			void setfocushome() {
				mX::server->setCurDim(1);
			}

			void exit() {
				mX::server->eraseDim(mx_dim->getID());
				mx_dim->destroyed = true;
				setfocushome();
			}
			int id;
			mx::mxPainter  paint;
			boost::scoped_ptr<dynDim> mx_dim;
	};
	/* export to Python */
	BOOST_PYTHON_MODULE(masterx)
	{
		class_<mxout>("mxout")
		.def("write", &mxout::write)
		.def("setfont", &mxout::setfont)
		.def("setcolor", &mxout::setcolor)
		.def("done", &mxout::done)
		;
		class_<mxSurf>("mxSurf")
		.def(init<std::string>())
		.def("release", &mxSurf::release)
		.def("load", &mxSurf::load)
		.def("loadpng", &mxSurf::loadpng)
		.def("create", &mxSurf::create)
		.def("w", &mxSurf::width)
		.def("h",&mxSurf::height)
		;
		class_<mxWin>("mxWin")
		.def(init<std::string,int,int,int,int>())
		.def("create", &mxWin::create)
		.def("settitle", &mxWin::settitle)
		.def("setpos", &mxWin::setpos)
		.def("setsize", &mxWin::setsize)
		.def("useskin", &mxWin::useskin)
		.def("setcallback", &mxWin::setcallback)
		.def("fillrect",  &mxWin::fillrect)
		.def("setpixel",   &mxWin::setpixel)
		.def("lock", &mxWin::lock)
		.def("unlock", &mxWin::unlock)
		.def("copysurf", &mxWin::copysurf)
		.def("copyresizesurf", &mxWin::copyresizesurf)
		.def("printtext", &mxWin::printtext)
		.def("keydown", &mxWin::keydown)
		.def("keyup", &mxWin::keyup)
		.def("mousemove", &mxWin::mousemove)
		.def("mousedown", &mxWin::mousedown)
		.def("mouseup", &mxWin::mouseup)
		.def("render", &mxWin::render)
		;
		def("bail", bail);
		class_<_Button>("Button")
		.def(init<mxWin&>())
		.def("settext", &_Button::settext)
		.def("render", &_Button::render)
		.def("createbutton", &_Button::createbutton)
		.def("setcallback", &_Button::setcallback)
		;
		class_<mxDim>("mxDim")
		.def(init<std::string>())
		.def("setbgcolor", &mxDim::setbgcolor)
		.def("add", &mxDim::add)
		.def("setbackground", &mxDim::setbackground)
		.def("addwindow", &mxDim::addwindow)
		.def("width", &mxDim::width)
		.def("height", &mxDim::height)
		.def("sizetoscreen", &mxDim::sizetoscreen)
		.def("settitle", &mxDim::settitle)
		.def("setfocus", &mxDim::setfocus)
		.def("setfocushome",&mxDim::setfocushome)
		.def("exit", &mxDim::exit)
		;
	}
	Console::Console(mxWidget *parent):  Window(parent)
	{
		console = this;
		path = restoreHome();
		file.open(".console.log", std::ios::out);
		number_one_offset = 0;
		cursor.x = 0, cursor.y = 0;
		font_color = Color(0x0,0xff,0x0);
		Console::printf("Welcome to MAsterX-ovDM  Shell\nThis shell is designed for this environment  and uses a Python interpreter.\n>>");
		input_start = con_text.length();
		cursor.x = input_start+1;
		start_length = input_start;
		font_name = "verdana";
		canClose(false);
		canMax(true);
		Py_Initialize();
		object module(handle<>(borrowed(PyImport_AddModule("__main__"))));
		this->trans_module = module;
		dict = module.attr("__dict__");
		initmasterx();
	   //object result = dictionary["result"];
	   //int result_value = extract<int>(result);
	   std::ostringstream  s;
	   s << "import sys; import masterx; from masterx import bail; sys.stdout = masterx.mxout(); sys.stderr = masterx.mxout(); quit = bail;";

	   pyExpr(s.str());
        
	}
	Console::~Console() {
		Py_Finalize();
	}
	string Console::restoreHome() {
#ifndef _WIN32
		uid_t  id;
		struct passwd *pw;
		id = getuid();
		pw = getpwuid(id);
		return pw->pw_dir;
#else
		return "C:\\";
#endif
	}
	void Console::setLoginName(string name) {
		dict["sys.stdout.login"] = name;
		user_ident = name;
	}
	int Console::keyDown(int key, void *data)
	{
		if(key == SDLK_ESCAPE) return  MSG_IGNORED;
		if(key == SDLK_RETURN) {
			static string message_total = "";
			static string prev_message = "";
			message =  con_text.substr( start_length, con_text.length()-1);
			int pos =    message.find(":");
			if(pos != -1)  {
				printf("\n");
				message_total += "\n";
			}
			else if( message.length() == 0) {
				        printf("\n");
						user_inputed(message_total);
						message_total = "";
			}
			 else {
				 prev_message =  message;
				if(prev_message.length()>1)
					printf("\n");
					message_total += "\n";
			}
			message_total += message;
			setToEnd();
			return MSG_PROC;
		}
		if(key == SDLK_TAB) {
			Console::printf("\t");
			return MSG_PROC;
		}

		if(key == SDLK_DOWN) {
			return MSG_PROC;
		}
		if(key == SDLK_BACKSPACE) {
			if(cursor.x > 0 && cursor.x >= start_length+1) {
				con_text = removeChar(cursor.x-1, con_text);
				cursor.x--;
			}
			return MSG_PROC;
		}

		if(key == SDLK_DELETE) {
			if(cursor.x >= 0 && cursor.x >= start_length && static_cast<unsigned int>(cursor.x) < con_text.length() && !(static_cast<unsigned int>(cursor.x) > con_text.length())) {
				con_text = removeCharTwo(cursor.x, con_text);
			}
			return MSG_PROC;
		}

		if(key == SDLK_UP) {
			Console::printf("%s", last_input.c_str());
			return MSG_PROC;
		}
		if(key == SDLK_LEFT) {
			if(cursor.x > start_length)
				cursor.x--;
			std::cout << cursor.x << ":" << start_length << "\n";
			return MSG_PROC;
		}
		if(key == SDLK_RIGHT) {
			if(static_cast<unsigned int>(cursor.x) < con_text.length())
				cursor.x++;
			return MSG_PROC;
		}


		if(key < 0 || key > 127)  return MSG_IGNORED;


		unsigned char *keys = SDL_GetKeyState(0);
		int k =  mxKeyFilter(data);
		if(k != 0 && cursor.x >= start_length) {
			Point p;
			if(char(k)  == '-' && (keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT])) {
				k = '_';
			}
			//con_text += char(k);
			if(cursor.x>0)
			con_text = insertChar(p,cursor.x, char(k), con_text);
			total  += (char)(k);
			//if(char(k) == ':') {
				//next_line = true;
			//}
			cursor.x++;
			cursor.y++;
		}
		return MSG_PROC;
	}
	void Console::setToEnd() {
		int total_length = con_text.length();
		start_length = total_length;
		cursor.x = start_length;
	}
	const std::string Console::printLastMessage() const
	{
		return message;
	}
	void Console::on_close() {
		file.close();
	}
	void Console::concat(string  s) {
		con_text += s;
		total += s;
		cursor.x += s.length();
		cursor.y += s.length();
	}
	// Console::printf -
	void Console::printf(const char *str, ...) {
		 va_list list;
		 string cat=str;
		 // truncate
		 if(cat.length()>4998) {
			 cat = cat.substr(0, 4998);
		 }
	     char string[5000];
	     va_start(list, str);
		 vsprintf((char*)string,cat.c_str(),list);
		 va_end(list);
		 concat(string);
	}
	int  Console::renderEvent() {
		try {

		Window::renderEvent();

		string temp  = con_text;
		Point  pointerbear;
		static unsigned int cursor_flash_counter = 0;
		if((++cursor_flash_counter)%4==0)
		temp = insertChar(pointerbear, cursor.x, '_', temp);
		else
		temp = insertChar(pointerbear, cursor.x, 124, temp);

		SDL_Font *ver = font(font_name)->getFont();
		if(ver == 0) {
			font_name = "verdana";
			ver = font(font_name)->getFont();
		}
        int rt_val = PrintText(Surface().getSurface(), ver, 13, 45, mx::Color::mapRGB(Surface(), font_color), temp.c_str());
        if(rt_val == 1)
        {
                string buf = mid(con_text, number_one_offset+message.length(), con_text.length());
                flush(buf);
                con_text = buf;
            
            
                scroll_window();
            
            
        }

		} catch (...)   {
			Console::printf("error has occoured.\n");
		}
		return MSG_IGNORED;
	}
	void Console::scroll_window()
	{
		cursor.x = con_text.length();
		start_length = con_text.length();

	}
	std::string Console::mid(std::string s, size_t start, size_t stop)
	{
	        std::string temp;
	        size_t i;
	        for(i = start; i < stop; i++)
	                temp += s[i];
	        return temp;
	}
	int Console::PrintText(struct SDL_Surface *surf, struct SDL_Font *fnt, int x, int y, Uint32 color, const char *src) {
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
	                	if(offset_x >= surf->w-35) goto overflow;
	                	continue;
					}
	                if(src[p] == 124) { width=7;  goto skip_to_here; }
	                for(i = 0; i < fnt->mx; i++) {
	                        for(z = 0; z < fnt->my; z++) {
	                                if(fnt->letters[(int)src[p]].fnt_ptr[i][z] != fnt->tcolor) {
	                                        if(offset_y+z > 0 && offset_x+i > 0 && offset_y+z < surf->h && offset_x+i < surf->w)
	                                        setpixel(pbuf, (Uint32)offset_x+i, (Uint32)offset_y+z, color, surf->format->BitsPerPixel, surf->pitch);
	                                        width=i;
	                                        if(height < z)
	                                                height=z;
	                                }
	                        }
	                }

	                skip_to_here:
	                offset_x += width + 3;
	                if(offset_x+width >= surf->w-25) {
	                overflow:
	                        offset_x = prev_x;
	                        offset_y += height;
	                        if(number_one_offset == 0)
	                        {
	                                number_one_offset = p;
	                        }

	                }
	                if(offset_y+height > surf->h-10)
	                                return 1;

	        }
	        unlock(surf);
	        return 0;
	}
	void Console::flush(string  &buf)
	{
		file << buf;
	}
	string Console::insertChar(Point &cursor, int index, char  c, string &str) {
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
	string Console::removeChar(unsigned int index, string &str) {
		string temp;
		size_t i = 0;
		for(i = 0; i < str.length(); i++) {

			if(i != index)
			temp += str[i];
		}
		return temp;
	}



	string Console::removeCharTwo(int &index, string &str) {

		int i;
		std::string temp;

		for(i =  0; i < static_cast<int>(str.length()); i++) {
			if(i == index) {
					str = removeChar(index, str);
					return str;
			}
		}
		return temp;
	}
	void Console::listFiles()
	{
		const char *src;
		src = path.c_str();
		DIR *d = opendir(src);
		if(!d) throw mx::mxException<string>("[ mx ] error on directory listing in console directory  not found: " + string(src)  + "\n");
		struct dirent *file;
		Console::printf("\nDirectory listing for %s\n", path.c_str());
		int count = 0;
		while ((file = readdir(d))) {
			printf(" %s ", file->d_name);
			count++;
		}
		closedir(d);
		Console::printf("\nFinshed listing %d Files/Directorys\n", count);
	}
	void Console::listFileList()
	{
		std::string p=path;
		DIR *d = opendir(p.c_str());
		if(!d) throw mx::mxException<string>("[ mx ] error on directory listing in console directory    [ " + path + "]  not found\n");
		struct dirent *file;
		Console::printf("\nDirectory listing for %s\n", path.c_str());
		int count = 0;
		while ((file = readdir(d)) != 0) {
			Console::printf(" %s\n ", file->d_name);
			count++;
		}
		closedir(d);
		Console::printf("\nFinshed listing %d Files/Directorys\n", count);
	}
	consoleDim::consoleDim( mxWnd *wnd, Size  si ) : Dimension("System Python Interpreter", wnd, si)
	{
		setBackground("img/dimcon.png");
		//setBackgroundColor(Color(20,20,20));
		con = new  Console();
		__assert__ ( con != 0 );
		con->createWindow(Color(254,254,254), "Python Interpreter", Point(100, 70), Size(640, 480), Color(0xff,0xff,0xff), Color(0,0,0), Color(0,0,255));
		con->useSkin("img/skins/mx/console");
		con->setPos ( Point (100,100) );
		con->setAlphaBlend(this, true);
		con->setAlpha(0.5f);
		addWindow(con);
	}

	Console::consoleTokenizer::consoleTokenizer(string data) {
		mascToken token;
		mascInit(&token);
		int rt_val;
		token.source_length = data.length();
		while (  ( rt_val = mascLex(&token, data.c_str()) ) != MID_NULL) {
		  if(rt_val != MID_SKIP) items.push_back(token.token);
		  mascTokenReset(&token);
		}
		free(token.token);
		total = data;
	}
	string Console::hash_pw(string val) {
			std::ostringstream stream;
			stream << "import hashlib; h = hashlib.md5(\"" << val << "\"); hash_value = h.hexdigest();";
			pyExpr(stream.str());
			object result = this->dict["hash_value"];
			char const *hashed_value = extract<char const *>(result);
			string hval = hashed_value;
			return hval;
	}
}


