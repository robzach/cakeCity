# -*- coding: utf-8 -*-
"""
Created on Mon Apr 17 11:02:48 2017

@author: mkell
Example taken from http://www.pyimagesearch.com/2014/08/04/opencv-python-color-detection/
"""

# import the necessary packages
import numpy as np
import argparse, time,struct
import socket
import cv2
import random

def getFrames():
    if __name__ == "__main__":
        parser = argparse.ArgumentParser( description = """Connect to the default camera and show the image stream.""")
        parser.add_argument( '-v','--verbose', action='store_true', help='Print additional diagnostics.')
        parser.add_argument( '--port', type=int, default=3700, help='UDP port number to which to send, default is 3700.')
        parser.add_argument( '--host', default='localhost', help='Host name to which to send, default is localhost.')
        args = parser.parse_args()

        # open the window and camera
        cv2.namedWindow("viewer")
        cameraNumber = 2
        cap = cv2.VideoCapture(cameraNumber)
        while(True):
            # Capture frame-by-frame
            ret, frame = cap.read()
            cv2.imshow('viewer',frame)
            detectColors(frame)
            
def threshold(contours):
    thresholdVal = 100
    newCnts = []
    for cnt in contours:
        if len(cnt) >= thresholdVal:
            newCnts.append(cnt)
    return newCnts

def detectContours(im,color):
    #convert original to black and white
    imgray = cv2.cvtColor(im, cv2.COLOR_BGR2GRAY)
    ret, thresh = cv2.threshold(imgray,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    #Find the contours
    img, contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    newContours = threshold(contours) #take out irrelevant contours
    with open("%s _Contours.data" % color,"wb") as file:
        for cnt in newContours:
            for l in cnt:
                file.write("%d, %d, 0\n" % (l[0][0], l[0][1]))
            file.write("NEWPATH, 0\n")

def threshold(contours):
    thresholdVal = 100
    newCnts = []
    for cnt in contours:
        if len(cnt) >= thresholdVal:
            newCnts.append(cnt)
    return newCnts

def detectColors(image):            
    # define the list of boundaries
    red = ([2, 2, 100], [98, 130, 255])
    blue = ([80, 31, 4], [220, 120, 110])
    green = ([0, 70, 0], [110, 255, 140])
    boundaries = {"red":red,"blue":blue,"green":green}
    # loop over the boundaries
    for color in boundaries:
        bounds = boundaries[color]
        lower,upper = bounds
        # create NumPy arrays from the boundaries
        lower = np.array(lower, dtype = "uint8")
        upper = np.array(upper, dtype = "uint8")
        # find the colors within the specified boundaries and apply
        # the mask
        mask = cv2.inRange(image, lower, upper)
        output = cv2.bitwise_and(image, image, mask = mask)
        #detect the contours
        detectContours(np.hstack([image, output]),color)
        
getFrames()




