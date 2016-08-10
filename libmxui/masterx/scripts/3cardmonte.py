
#example

import masterx
import random

class card(object):
    def __init__(self, x,y, select,  speed):
        self.setpos(x,y)
        self.value = select
        self.speed = speed
    def setpos(self, x,y):
        self.x = x
        self.y = y
    def setvalue(self, value):
        self.value = value
    def draw(self):
        if self.value == 1:
            card_window.copysurf(card_window.card, self.x, self.y, card_window.card.w(), card_window.card.h())
        elif self.value == 2:
            card_window.copysurf(card_window.card_select, self.x, self.y, card_window.card_select.w(), card_window.card_select.h())
    def drawBack(self):
        card_window.copysurf(card_window.card, self.x, self.y, card_window.card.w(), card_window.card.h())
        
def drawSuccess():
    for c in card_window.card_list:
        c.draw()
    
    card_window.printtext(50,50, 0x0, 0x0, 0x0, "Success ! Press Escape to quit");
    
def  drawFailure():
    for c in card_window.card_list:
        c.draw();
    card_window.printtext(50,50, 0xff, 0,    0, "Failure you lose, press  Escape to quit")

def draw_game(): 
    for c in  card_window.card_list:
        c.draw()
    card_window.printtext(250, 250+card_window.card_select.h(), 0xff, 0, 0, "Press Enter to Shuffle");
         
def draw_in_place():
    for c in card_window.card_list:
        c.drawBack()         

def  draw_shuffle():
    for  c in card_window.card_list:
            c.x -=c.speed
            c.setpos(c.x, c.y)
            if(c.x < 75 ): 
                c.setpos(700, 200)
                #c.setpos(600+card_wiidow.card_select.w(),200)
            c.drawBack()
    card_window.printtext(250,  250+card_window.card_select.h(),  0xff, 0xff, 0xff, "Press Space to stop shuffle")
    
            
def render():
    #card_window.fillrect(0, 0, masterx_dim.width(), masterx_dim.height(), 0, 0, 0)
    #card_window.copysurf(background.obj,   0, 0, background.obj.w(),  background.obj.h())
    card_window.copysurf(card_window.bg, 0,   0, card_window.bg.w(), card_window.bg.h())
    
    if card_window.render_mode == 1:
       card_window.printtext(50, 50, 0xff,0x0,0x0,"Press Space to Play")
       card_window.printtext(50, 75,0x0,0x0,0xff, "Press Escape to  Quit")
       card_window.copysurf(card_window.card_select,250,250,card_window.card.w(), card_window.card.h())       
    elif card_window.render_mode == 2:
       draw_game()
    elif card_window.render_mode == 3:  
      draw_shuffle()
    elif card_window.render_mode == 4:
        card_window.printtext(50,50,0xff,0xff,0xff, "Choose the card you think contains the pattern, ethier  1 2 or 3")
        draw_in_place()
        card_window.printtext(200+card_window.card.w()/2-25,200+card_window.card.h()+10,  0xff, 0x0, 0x0, "1")
        card_window.printtext(400+card_window.card.w()/2-25,200+card_window.card.h()+10,  0x0, 0xff , 0x0, "2")
        card_window.printtext(600+card_window.card.w()/2-25,200+card_window.card.h()+10,  0x0, 0x0,  0xff, "3");
    elif card_window.render_mode  == 5:
            drawSuccess()
    elif card_window.render_mode == 6:
            drawFailure()
        

def keydown(key):
    
    if key == 27:
        masterx_dim.exit()
    
    if card_window.render_mode == 1:
        if key < 256 and chr(key) == ' ':
            card_window.render_mode = 2
        elif key == 27:
            masterx_dim.exit()
    elif card_window.render_mode == 2:
        if(key == 13):
            card_window.render_mode = 3;
    elif card_window.render_mode == 3:
        if key < 256 and chr(key) ==  ' ':
            card_window.render_mode = 4
            
            if(card_window.card_list[0].x < card_window.card_list[1].x and card_window.card_list[0].x < card_window.card_list[2].x):
                card_window.card_list[0].x = 200;
            elif(card_window.card_list[0].x > card_window.card_list[1].x and card_window.card_list[0].x > card_window.card_list[2].x):
                card_window.card_list[0].x = 600
            else:
                card_window.card_list[0].x = 400
                
                
            if(card_window.card_list[1].x < card_window.card_list[0].x and card_window.card_list[1].x < card_window.card_list[2].x):
                card_window.card_list[1].x = 200;
                card_window.winner = 1
            elif(card_window.card_list[1].x > card_window.card_list[0].x and card_window.card_list[1].x >card_window.card_list[2].x):
                card_window.card_list[1].x = 600
                card_window.winner = 3
            else:
                card_window.card_list[1].x = 400
                card_window.winner = 2
                
            if(card_window.card_list[2].x < card_window.card_list[1].x and card_window.card_list[2].x < card_window.card_list[0].x):
                card_window.card_list[2].x = 200;
            elif(card_window.card_list[2].x > card_window.card_list[1].x and card_window.card_list[2].x > card_window.card_list[0].x):
                card_window.card_list[2].x = 600
            else:
                card_window.card_list[2].x = 400
     
    elif card_window.render_mode == 4:
        
        if(key < 256 and  chr(key) ==  '1' and  card_window.winner == 1 and chr(key)  != ' '):
            card_window.render_mode = 5
        elif (key <256  and chr(key) == '2' and card_window.winner == 2 and chr(key) !=  ' '):
            card_window.render_mode = 5
        elif (key <256 and chr(key) == '3'  and  card_window.winner == 3 and   chr(key) != ' '):
            card_window.render_mode = 5;
        else:
            card_window.render_mode = 6;
   
masterx_dim=masterx.mxDim()
masterx_dim.settitle("3card - Python Demo")
card_window=masterx.mxWin("", 0, 0, masterx_dim.width(), masterx_dim.height())
card_window.render_mode = 1
card_window.render = render
card_window.keydown = keydown
card_window.setcallback("card_window")
card_window.card=masterx.mxSurf()
card_window.card.loadpng("img/pydata/card.png")
card_window.card_select = masterx.mxSurf()
card_window.card_select.loadpng("img/pydata/cardselect.png")

background = masterx.mxSurf()
background.loadpng("img/pydata/cardbg.png")
card_window.bg = background

masterx_dim.addwindow(card_window)
masterx_dim.add()
card_window.card_list = [ ]
card_window.card_list.append( card(200, 200, 1,10))#random.randint(1,3)))
card_window.card_list.append( card(400, 200, 2,15)),#random.randint(1,3)))
card_window.card_list.append( card(600, 200, 1,12))#radom.randint(1,3)))
masterx_dim.setfocus()

