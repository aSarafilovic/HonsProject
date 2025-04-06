import Messenger
import VisionSystem

class PlayerControl():
    #actual code design
    #get the coords

    def setup(self):
        #set up the messenger and vison system objects
        self.msg = Messenger.Messenger()
        self.vis = VisionSystem.VisionSystem()
        self.x = 0
        self.y = 0
        #if either returns false, then return false also.
        if (self.msg.setup() and self.vis.setup()):
            return True
        else:
            return False

    def get_coords(self):
        #get the coordinates from the vision system
        xy = self.vis.get_coords()
        self.x = xy[0]
        self.y = xy[1]

    def decision(self):
        #get the coordinates necessary for the decision
        self.get_coords()
        
        #if there's no ball detected then 

        #decide on the player moves
        if self.x > 1300:
            #strikers
            self.msg.motor_char(1)
            if self.x < 1400:
                self.msg.action_char(1)
                if (self.y <900 and self.y > 200):
                    moveTo = (round(((self.y - 200)/700)))
                    if moveTo < 10:
                        moveTo = "00"+str(moveTo)
                    if moveTo < 100:
                        moveTo = "0"+str(moveTo)
                    self.msg.loc_chars(moveTo)
                elif (self.y > 900):
                    self.msg.loc_chars("100")
                else:
                    self.msg.loc_chars("000")

        
        elif self.x > 800 and self.x < 1300:
            #midfielders
            self.msg.motor_char(2)
            if self.x < 900:
                self.msg.action_char(1)
                if (self.y <1000 and self.y > 100):
                    pass
                elif (self.y > 1000):
                    self.msg.loc_chars("100")
                else:
                    self.msg.loc_chars("000")
        
        elif self.x > 400 and self.x < 800:
            #defenders
            self.msg.motor_char(3)
            if self.x < 500:
                self.msg.action_char(1)
                if (self.y <900 and self.y > 250):
                    pass
                elif (self.y > 900):
                    self.msg.loc_chars("100")
                else:
                    self.msg.loc_chars("000")
        
        else:
            #goalie
            self.msg.motor_char(4)
            if self.x < 300:
                self.msg.action_char(1)
                if (self.y <650 and self.y > 450):
                    pass
                elif (self.y > 650):
                    self.msg.loc_chars("100")
                else:
                    self.msg.loc_chars("000")

        #decide on the lateral moves (probably best to be done within the other one isn't it, since it's so dependant on the range of each motor)

    def execute(self):
        self.msg.send()