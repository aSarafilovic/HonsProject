import VisionSystem
import cv2

vis = VisionSystem.VisionSystem()

if (vis.setup() == False):
    print ("failure to setup")
else:
    while True:
        ret = vis.get_coords()
        print(ret[0]," , ",ret[1])
        if 0xFF == ord('q'):
            break
vis.shutdown()