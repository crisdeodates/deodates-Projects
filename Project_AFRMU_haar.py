!/usr/bin/python
#---------------------------------------------------------------------------#
#-- PROJECT Autonomous Fire Response & Monitoring Unit                                                                                                                                       #
#-- PROGRAM GOALS                                                      
#-- 1. Detect humans and provide warnings                             
#                                                                     
#-- Programmers: Cris Thomas, Jiss Joseph Thomas                                
#-- References: OpenCV HAAR                                          
#---------------------------------------------------------------------------#
import time
#-- Setup people detection
person_cascade = cv2.CascadeClassifier('haarcascade_fullbody.xml')
cap = cv2.VideoCapture(0)
#-- People detection function using HAAR
def do_people_detect():
	detected = False
	r, frame = cap.read()
	if r:
		frame = cv2.resize(frame,(640,360)) # Downscaling
		gray_frame = cv2.cvtColor(frame, cv2.COLOR_RGB2GRAY)
		rects = person_cascade.detectMultiScale(gray_frame)       
		for (x, y, w, h) in rects:
			if x:
				detected = True			
			cv2.rectangle(frame, (x,y), (x+w,y+h),(0,255,0),2)
		cv2.imshow("preview", frame)
		print 'human(s) detected'
def main():
while True:
	do_people_detect()
