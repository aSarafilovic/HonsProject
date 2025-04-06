import numpy as np
import cv2
from statistics import mean
from picamera2 import Picamera2, Preview


#set up picamera
cam = Picamera2()
main = {'size': (1920,1080)}
sensor = {'output_size': (2304,1296)}
config = cam.create_video_configuration(main, sensor=sensor)
cam.start(config)


while (1):
    imageFrame = cv2.cvtColor(cam.capture_array(),cv2.COLOR_BGR2RGB)
    kernal = np.ones((5, 5), "uint8")
    
    # Convert BGR to HSV colorspace
    hsvFrame = cv2.cvtColor(imageFrame, cv2.COLOR_BGR2HSV)

    #green color
    green_lower = np.array([25, 52, 72], np.uint8)
    green_upper = np.array([102, 255, 255], np.uint8)
    #define mask
    green_mask = cv2.inRange(hsvFrame, green_lower, green_upper)

    #red
    red_lower = np.array([136, 87, 111], np.uint8)
    red_upper = np.array([100, 255, 255], np.uint8)
    #define mask
    red_mask = cv2.inRange(hsvFrame, red_lower, red_upper)
    red_mask = cv2.dilate(red_mask, kernal)


    # to detect only that particular color
    #kernal = np.ones((5, 5), "uint8")

    # green color
    green_mask = cv2.dilate(green_mask, kernal)
    #res_green = cv2.bitwise_and(imageFrame, imageFrame, mask=green_mask)

    #yellow colour
    yellow_lower = np.array([20,100,100])
    yellow_upper = np.array([30,255,255])
    yellow_mask = cv2.inRange(hsvFrame,yellow_lower,yellow_upper)

    yellow_mask = cv2.dilate(red_mask,kernal)
    res_yellow = cv2.bitwise_and(imageFrame, imageFrame, mask = yellow_mask)

    # Creating contour to track green color
    contours, hierarchy = cv2.findContours(green_mask,
                                           cv2.RETR_TREE,
                                           cv2.CHAIN_APPROX_SIMPLE)

    
    #for pic, contour in enumerate(contours):
    #    area = cv2.contourArea(contour)
    #    if (area > 800):
    #        x, y, w, h = cv2.boundingRect(contour)
    #        imageFrame = cv2.rectangle(imageFrame, (x, y),(x + w, y + h),(0, 255, 0), 2)
    #        
    #        xpos = str(x + w/2)
    #        ypos = str(y + h/2)
    #        pos = xpos + " , " + ypos
    #
            #cv2.putText(imageFrame, pos , (x, y),cv2.FONT_HERSHEY_SIMPLEX,1.0, (0, 255, 0))
    

    #try:      
     #   avgx = int(mean(xlocarr))
     #   avgy = int(mean(ylocarr))
    #    avgpos  = str(avgx)+" , "+str(avgy)            
    #    avgframe = cv2.rectangle(imageFrame, (avgx,avgy), (avgx+30,avgy+30),(0,255,0),2)
    #    test = False
    #except:
    #    pass
    #else:
    #cv2.putText(imageFrame, avgpos,(avgx,avgy),cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0,255,0))
    
    #cv2.putText(imageFrame, avgpos,(avgx,avgy),cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0,255,0))
     
     


    # final run
    cv2.imshow("Color Detection", imageFrame)
    if cv2.waitKey(10) & 0xFF == ord('q'):
        cam.stop()
        cam.close()
        cv2.destroyAllWindows()
        break
    
#print("Average position of all detected items: ", avgpos)