#ifdef HAVE_CONFIG_H
#include<config.h>
#endif
#include"masterx.h"
#include"menu.h"
#ifndef _WIN32
#include<sys/wait.h>
#include<sys/types.h>
#include<pwd.h>
#include<unistd.h>
#endif
#include<string>
#include<iostream>
#include<ctime>
#include<cstdlib>
#include<args.h>
#include<boost/scoped_ptr.hpp>
#include<boost/lexical_cast.hpp>

const int SCREEN_WIDTH   = 1280;
const int SCREEN_HEIGHT  = 720;



using masterx::masterX_server;
using std::string;


struct AutomaticFileCloser {
	FILE *fptr;
	AutomaticFileCloser() { fptr = 0; }
	~AutomaticFileCloser() { if(fptr != 0) fclose(fptr); }
};

AutomaticFileCloser af_stdout;
AutomaticFileCloser af_stderr;

//#define MACOSX_APP
class masterX_Driver {

public:
masterX_Driver() {}
~masterX_Driver()  {}


	public:	static int Main(int argc, char **argv) {

			masterX_Driver *driver = new masterX_Driver();

			if(argv[0][0] == '/') {
				std::string path = argv[0];
				path = path.substr(0, path.rfind("/"));
			#ifdef MACOSX_APP
				path += "/../Resources";
			#endif
				if( chdir(path.c_str()) )
				std::cout << "changed directory path to: " << path << "\n";
			}

			try {
				srand(static_cast<unsigned int>(time(0)));
				af_stderr.fptr = freopen("mxerror.log", "w", stderr);

				int width = SCREEN_WIDTH, height = SCREEN_HEIGHT;
				mx::mxArgs argz(argc, argv);

				string w = argz.testForItem("--width");
				string h = argz.testForItem("--height");
				string ilog = argz.testForItem("--log");

				if(w != "" && h != "") {
					int wx = boost::lexical_cast<int>(w);
					int hx = boost::lexical_cast<int>(h);

					if(wx >= SCREEN_WIDTH && hx >= SCREEN_HEIGHT) width = wx, height = hx;
					std::cout << "set screen size: " << width << "/" << height << "\n";
				}

				if(ilog != "" && ilog == "true") {
					af_stdout.fptr = freopen("system.log", "w", stdout);
				}
				string full = argz.testForItem("--fullscreen");

				unsigned int flags = 0;

				#if defined(__APPLE__)
					if(full == "yes")
					flags = SDL_FULLSCREEN;
				#endif


				driver->Xserver.reset(new masterX_server(mx::Size(width,height), flags));
				driver->Xserver->width = width;
				driver->Xserver->height = height;

				/// be sure to set width and height as well as fullscreen flag to work properly
				#if defined(__LINUX__)
					if(full == "yes") SDL_WM_ToggleFullScreen(driver->Xserver->renderSurface());
				#endif
				int rt = driver->Xserver->messageLoop();
                delete driver;
                driver = 0;
                return   rt;

			} catch (mx::mxException<std::string> &e) {

				std::cout << argv[0] << ": a exception has been thrown, program execution halted\ncheck mxerror.log for further details\n";
				e.printError(std::cerr);
			}
			catch(boost::bad_lexical_cast &bc) {

				std::cout << argv[0] << ": wrong operands for width or height\ncheck mxerror.log for further details.\n";
				std::cerr << bc.what() << "\n";
			}
			catch (std::exception &e) {

				std::cout << argv[0] << ": a exception has been thrown, program execution halted\ncheck mxerror.log for further details\n";
				std::cerr << e.what() << "\n";
			}
			catch(...) {
				std::cerr << "Unknown exception\n";
			}
            if(driver != 0) delete driver;
			return  EXIT_FAILURE;
	}

protected:
	boost::scoped_ptr<masterx::masterX_server> Xserver;

};














_m_(masterX_Driver::Main)


