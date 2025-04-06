import PlayerControl

#define the player controller
player = PlayerControl.PlayerControl()
#do all the setup
if(player.setup()):
    #run the gameplay loop
    while True:
        #get the position of the ball
        player.get_coords()
        #make the player decision
        player.decision()
        #execute the move (send it to the arduino)
        player.execute()