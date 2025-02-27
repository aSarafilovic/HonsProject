import numpy as np
import cv2
from statistics import mean

# turn on cam
webcam = cv2.VideoCapture(0)
#frame width
webcam.set(3, 1080)
##frame height
webcam.set(4, 840)

setuprunning = True
test = True

while setuprunning == False:
    _, imageFrame = webcam.read()

    # Convert BGR to HSV colorspace
    hsvFrame = cv2.cvtColor(imageFrame, cv2.COLOR_BGR2HSV)

    #green color
    green_lower = np.array([25, 52, 72], np.uint8)
    green_upper = np.array([102, 255, 255], np.uint8)
    #define mask
    green_mask = cv2.inRange(hsvFrame, green_lower, green_upper)

    # to detect only that particular color
    kernal = np.ones((5, 5), "uint8")

    # green color
    green_mask = cv2.dilate(green_mask, kernal)
    res_green = cv2.bitwise_and(imageFrame, imageFrame, mask=green_mask)

    # Creating contour to track green color
    contours, hierarchy = cv2.findContours(green_mask,
                                           cv2.RETR_TREE,
                                           cv2.CHAIN_APPROX_SIMPLE)

    #x and y location arrays for finding the average position of all located objects
    xlocarr = []
    ylocarr = []
    for pic, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if (area > 300):
            x, y, w, h = cv2.boundingRect(contour)
            imageFrame = cv2.rectangle(imageFrame, (x, y),
                                       (x + w, y + h),
                                       (0, 255, 0), 2)
            
            xpos = str(x + w/2)
            ypos = str(y + h/2)
            pos = xpos + " , " + ypos
            
            xlocarr.append(x + w/2)
            ylocarr.append(y + h/2)
            cv2.putText(imageFrame, pos , (x, y),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        1.0, (0, 255, 0))
    
    #x and y location arrays for finding the average position of all located objects
    xlocarr = []
    ylocarr = []
    
    print("run")
    try:
        avgx = int(mean(xlocarr))
        avgy = int(mean(ylocarr))
        avgpos  = str(avgx)+" , "+str(avgy)
        avgframe = cv2.rectangle(imageFrame, (avgx,avgy), (avgx+30,avgy+30),(0,255,0),2)
        cv2.putText(imageFrame, avgpos,(avgx,avgy),cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0,255,0))
        setuprunning = False
        print("check")
    except:
        pass
    #cv2.imshow("Color Detection", imageFrame)
    



while (1):
    _, imageFrame = webcam.read()
    
    # Convert BGR to HSV colorspace
    hsvFrame = cv2.cvtColor(imageFrame, cv2.COLOR_BGR2HSV)

    #green color
    green_lower = np.array([25, 52, 72], np.uint8)
    green_upper = np.array([102, 255, 255], np.uint8)
    #define mask
    green_mask = cv2.inRange(hsvFrame, green_lower, green_upper)

    # to detect only that particular color
    kernal = np.ones((5, 5), "uint8")

    # green color
    green_mask = cv2.dilate(green_mask, kernal)
    res_green = cv2.bitwise_and(imageFrame, imageFrame, mask=green_mask)

    # Creating contour to track green color
    contours, hierarchy = cv2.findContours(green_mask,
                                           cv2.RETR_TREE,
                                           cv2.CHAIN_APPROX_SIMPLE)

    
    for pic, contour in enumerate(contours):
        area = cv2.contourArea(contour)
        if (area > 300):
            x, y, w, h = cv2.boundingRect(contour)
            imageFrame = cv2.rectangle(imageFrame, (x, y),
                                       (x + w, y + h),
                                       (0, 255, 0), 2)
            
            xpos = str(x + w/2)
            ypos = str(y + h/2)
            pos = xpos + " , " + ypos

            cv2.putText(imageFrame, pos , (x, y),
                        cv2.FONT_HERSHEY_SIMPLEX,
                        1.0, (0, 255, 0))
    if test:
        try:      
            avgx = int(mean(xlocarr))
            avgy = int(mean(ylocarr))
            avgpos  = str(avgx)+" , "+str(avgy)
            avgframe = cv2.rectangle(imageFrame, (avgx,avgy), (avgx+30,avgy+30),(0,255,0),2)
            test = False
        except:
            pass
    else:
        cv2.putText(imageFrame, avgpos,(avgx,avgy),cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0,255,0))
    
    #cv2.putText(imageFrame, avgpos,(avgx,avgy),cv2.FONT_HERSHEY_SIMPLEX, 1.0, (0,255,0))
     
     


    # final run
    cv2.imshow("Color Detection", imageFrame)
    if cv2.waitKey(10) & 0xFF == ord('q'):
        webcam.release()
        cv2.destroyAllWindows()
        break
    
#print("Average position of all detected items: ", avgpos)