#ifndef _MXDB_H_
#define _MXDB_H_
#include<mxdimension.h>
#include<string>
#include<vector>
#include<mxwidget.h>
#include<mxwidget_type.h>
#include<unistd.h>
#ifndef  _WIN32
#include<sys/wait.h>
#include<sys/types.h>
#include<pwd.h>
#endif

#include<mx.h>
#include<mxsurface.h>
#include<list>





namespace masterx {


	using std::string;
	using std::vector;
	using namespace mx;
	using namespace mxui;
	using mx::mxWnd;
	using std::list;
	

	struct fileInfo {
		string name;
		bool dir;
		int offset;
	};
	
	
	enum opMode { MX_DIM=0x1, MX_FILE };
	
	class fileBrowser  : public  mxWidget , virtual public mxFontDisplay {
	
	public:
		
		fileBrowser(mxWidget *parent = 0) : mxWidget(parent)
		{
			offset = 0;
			current_file = 0;
		}
		
		void countFiles();
		void displayFiles();	
		vector<fileInfo> current_directory;
		int offset;
		int current_file;
		int offset_stop;
	protected:
		
		
	};
	
	class FileDisplay;

	class dimBrowser : public mxui::Dimension {
	
	public:
	
		dimBrowser(mx::mxWnd *wnd, mx::Size s);	
		virtual ~dimBrowser() { }
	
		void setPath(opMode mode, string cpath);
	
		const char *getHomePath() {
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
		dimBrowser&  operator=(mxWidget *additive);
		void addTable(mxWnd *wnd);
		void addItem(mxWidget *);
		virtual void keyDown(int key, void *data);
		
		int line_id;
		FileDisplay *display;
protected:
		opMode current;
		string path;
public: 
	vector<fileInfo> *directory; 
	
	Window *win;
	
protected:
		fileBrowser  browser;
		
				
};

	

	class mxListView : public mxWidget, public virtual  mxFontDisplay {
	public:
		
		mxListView(mxWidget *parent)  {
			this_parent = parent;
			setParent(parent);
			bg_color = Color(0,0,0);	
		}
		void createListView(Rect pos, Color color);	
		virtual int renderEvent();
		void setDirectory(std::vector<fileInfo> *);
		void displayFileList(std::vector<fileInfo> *v, int counter_x, int across, int *offset);
		void setBrowser( fileBrowser *b ) { browser = b; }
		
	protected:
		mxWidget *this_parent;
		Color bg_color;
		std::vector<fileInfo> *file_info;
		fileBrowser *browser;
				
	};


	class FileDisplay : public Window {
		
	public:
		FileDisplay() : Window()
		{
			setParent(parent);
		}
		
		int action(widgetAction &ew);
		int button_left_cb(widgetAction &we);
		int button_right_cb(widgetAction &we);
		
		ActionLine<FileDisplay> *line_edit;
		ActionButton<FileDisplay> *button_left;
		ActionButton<FileDisplay> *button_right;
	
		fileBrowser *browser;
		
	protected:
		
	};



}


#endif


