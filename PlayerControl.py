

#how its gonna work:
#   Take in the x,y position of the ball,
#deliniate between the motor sets based on the balls x position
#   Strikers: 1300+ Kick: 1300-1400
#   Midfielders: 800 -> 1250 Kick: 800-900
#   Defenders: 400 -> 750 Kick: 400-500
#   Goalie: 200 -> 350 Kick: 200-300
#
#   Anything behind the foosmen will make them go horizontal
#
#Movement of the players based on the Y position
#   run some tests and see what the limits and spacings for each row are and then have them move to certain position based on ball positon in relation to that
#   Basically this is gonna be a bit complicated, call your shot for the write up
#
#Command Line:
#   the position will determine the composition of the command line (the instructions)
#   messenger tool, holds the methods to alter the command line and send it to the arduino
#   tool is used by the player controller to send the messages to the arduino
#
#
#

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
            pass #Motor value = 1 (strikers)
            if self.x < 1400:
                pass #Action value = 1 (kick)
        elif self.x > 800 and self.x < 1300:
            pass #Motor value = 2 (midfielders)
            if self.x < 900:
                pass #Action value = 1
        elif self.x > 400 and self.x < 800:
            pass #Motor value = 3 (defenders)
            if self.x < 500:
                pass #Action value = 1
        else:
            pass #Motor value = 4 (goalie)
            if self.x < 300:
                pass #Action value = 1

        #decide on the lateral moves (probably best to be done within the other one isn't it, since it's so dependant on the range of each motor)

    def execute(self):
        self.msg.send()