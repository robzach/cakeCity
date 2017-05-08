import cv2
import numpy as np
import random
#Convert orignal image to a greyscale image
im = cv2.imread('RedContours.png')
imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
ret, thresh = cv2.threshold(imgray,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
def threshold(contours):
    thresholdVal = 100
    newCnts = []
    for cnt in contours:
        if len(cnt) >= thresholdVal:
            newCnts.append(cnt)
    return newCnts

#Find the contours
img, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
#make blank image for display
image = 0*im


#Display the contours   
cnt = contours[0]
cv2.drawContours(image,[cnt], -1, (255,255,255), 3)
image = 0*im 
cv2.namedWindow( "Contours", 1 )
cv2.imshow( "Contours", image)


