#ifndef _MXYO_
#define _MXYO_

#include<mx.h>
#include<menu.h>
#include<mxserver.h>
#include<mxl.h>
#include<vector>
#include<mxwidget_type.h>
#include<mxf.h>
#include<mxfunction.h>
#include"mxassert.h"
#include<boost/python.hpp>
#include<boost/scoped_ptr.hpp>


namespace masterx {

	using namespace mxui;
	using namespace mx;
	using mxui::Dimension;
	using namespace boost::python;


    
	class Console : public Window {

	public:
		Console(mxWidget *parent = 0);
		virtual ~Console();
		virtual int renderEvent();
		virtual void on_close();
		const string printLastMessage() const;
		void printf(const char *src, ...);
		void concat(string  text);
		void flush(string &buf);
        
        
		int PrintText(struct SDL_Surface *surf, struct SDL_Font *fnt, int x, int y, Uint32 color, const char *src);
        void scroll_window();
        string  returnHome();
		virtual int keyDown(int key, void *data);
		void user_inputed(string);
		string insertChar(Point &p, int index, char  c, string &str);
		string removeChar(unsigned int index, string &str);
		string removeCharTwo(int &index, string &str);
		void LexArgument(string s);
		void pyExpr(string);
		void inputPyExpr(string);
		string last_input;
		void setfont(string f) { font_name = f; }
		void setcolor(int r, int g, int b) {   font_color = Color(r,g,b); }
		string hash_pw(string val);
		object trans_module;
		object dict;
		string user_ident;
		void setLoginName(string name);
		void setToEnd();
	protected:

		string mid(string s, size_t start, size_t stop);
		string bottle;
		Rect cordz;
		string con_text,total;
		Point cursor;
		Point console_cursor;
		std::fstream file;
		int number_one_offset;
		Color font_color;
		int input_start;
		int start_length;
		string message;
		string font_name;

		class consoleTokenizer {
		public:
			consoleTokenizer(string data);
			string& operator[](int i) {   return items[i]; }
			size_t size() { return items.size(); }
			void clear() { items.clear(); }
			string total;
		protected:
			std::vector<string> items;

		};

		void listFiles();
		void listFileList();
		string restoreHome();
		string path;

	};

	class consoleDim : public Dimension {

	public:

		consoleDim(mxWnd *wnd, Size  si);
		virtual ~consoleDim() { }
		Console *con;
	};

}


namespace System {
		extern masterx::Console *console;
}









#endif



