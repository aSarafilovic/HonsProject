import numpy as np 
import cv2
from statistics import mean
from picamera2 import Picamera2, Preview
from time import sleep
import libcamera

class VisionSystem():

    def setup(self):
        #define parameters for the camera
        main = {'size': (1920,1080)}
        sensor = {'output_size': (1920,1080)}
        try:
            #setup the camera objects
            self.cam = Picamera2()
            config = self.cam.create_video_configuration(main, sensor=sensor)
            self.cam.start(config)
        #if there's no camera detected then the system wont start
        except:
            return False
        #if everything is fine then the system will start
        return True

    def get_coords(self):
        #get the image from the camera
        imageFrame = cv2.cvtColor(self.cam.capture_array(),cv2.COLOR_BGR2RGB)
        hsvFrame = cv2.cvtColor(imageFrame,cv2.COLOR_BGR2HSV)

        #define the yellow colour
        yellow_lower = np.array([20,100,100])
        yellow_upper = np.array([30,255,255])
        yellow_mask = cv2.inRange(hsvFrame,yellow_lower,yellow_upper)

        #to detect only that particular colour
        kernal = np.ones((5,5), "uint8")

        #yellow colour mask final setup
        yellow_mask = cv2.dilate(yellow_mask, kernal)
        #establish yellow detection
        yellow_mask = cv2.inRange(hsvFrame,yellow_lower,yellow_upper)

        #detect only the yellow objects in the frame
        contours,heirarchy = cv2.findContours(yellow_mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        #select only the largest yellow object in the frame
        biggest_contour = None
        max_area = 0
        for pic, contour in enumerate(contours):
            area = cv2.contourArea(contour)
            if ((area > 800) and (area > max_area)):
                biggest_contour = contour
                max_area = cv2.contourArea(contour)

        #get the centre point of this object
        x,y,w,h = cv2.boundingRect(biggest_contour)
        xpos = str(x+w/2)
        ypos = str(y+h/2)

        #
        #cv2.imshow("Color Detection", imageFrame)

        #return the centre point of this object
        ret = [xpos,ypos]
        return ret

    def shutdown(self):
        #just clean everything up
        self.cam.stop()
        self.cam.close()
        print("night night")