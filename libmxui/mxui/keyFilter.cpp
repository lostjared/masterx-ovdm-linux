
#include "SDL.h"
#include "SDL_keysym.h"
#include<masc++.h>




	int mxKeyFilter(void *ptr)
	{
		
		    SDL_Event *e = (SDL_Event*) ptr;
	        Uint16 key = e->key.keysym.sym;
	        Uint8 *buf = SDL_GetKeyState(0);
	
	        if(key == SDLK_LSHIFT || key == SDLK_RSHIFT || key == SDLK_CAPSLOCK || key  == SDLK_LALT || key == SDLK_RALT || key == SDLK_LCTRL || key == SDLK_RCTRL || key >= SDLK_F1 && key <= SDLK_F12) return 0;
	
	        if(((e->key.keysym.mod == KMOD_LSHIFT || e->key.keysym.mod == KMOD_RSHIFT || e->key.keysym.mod == KMOD_CAPS) || (buf[SDLK_LSHIFT] || buf[SDLK_RSHIFT])) && key >= 'a' && key <= 'z')
	        { 
	                return (toupper(key));
	        }
	        else if(e->key.keysym.mod == KMOD_NONE && key >= 'a' && key <= 'z')
	        {
	                return key;
	        }
	        else if(((e->key.keysym.mod == KMOD_LSHIFT || e->key.keysym.mod == KMOD_RSHIFT) || (buf[SDLK_LSHIFT] || buf[SDLK_RSHIFT])) && 
	key >= '0' && key <= '9')
	        {
	                unsigned char ctext = 0;
	                bool ok = true;
	                switch(key)
	                {
	                case '1': ctext  = '!'; break;
	                case '2': ctext  = '@'; break;
	                case '3': ctext  = '#'; break;
	                case '4': ctext  = '$'; break;
	                case '5': ctext  = '%'; break;
	                case '6': ctext  = '^'; break;
	                case '7': ctext  = '&'; break;
	                case '8': ctext  = '*'; break;
	                case '9': ctext  = '('; break;
	                case '0': ctext  = ')'; break;
	             
	                default: ok = false;
	                }
	
	                if(ok == true) {
	                        return ctext;
	                        
	                }
	        } 
	                	                
	        if(((e->key.keysym.mod == KMOD_LSHIFT || e->key.keysym.mod == KMOD_RSHIFT) || (buf[SDLK_LSHIFT] || buf[SDLK_RSHIFT]))) 
	        {
	        
	                unsigned char ctext = key;
	                switch(key)
	                {
	                case ';':  ctext = ':';  break;
	                case '\'': ctext = '"';  break;
	                case '[':  ctext = '{';  break;
	                case ']':  ctext = '}';  break;
	                case '\\': ctext = '|';  break;
	                case ',':  ctext = '<';  break;
	                case '.':  ctext = '>';  break;
	                case '/':  ctext = '?';  break;
	                case '-':  ctext = '_';  break;
	                case '=':  ctext = '+';  break;
	                }
	                return ctext;
	        }
	
	         return key;
	        
		}

