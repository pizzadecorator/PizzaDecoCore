from imutils import face_utils
import numpy as np
import imutils
import dlib
import cv2
import math

'''
Referenced by : https://www.pyimagesearch.com/2017/04/10/detect-eyes-nose-lips-jaw-dlib-opencv-python/

Links for dlib : https://pypi.org/simple/dlib/
In windows: you have to download dlib-19.8.1-cp36-cp36m-win_amd64.whl to your workspace
and command 'pip install dlib-19.8.1-cp36-cp36m-win_amd64.whl'

Another prerequisite : 'pip install imutils'

'''

detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor('./shape_predictor_68_face_landmarks.dat')

cap = cv2.VideoCapture(0)

while(True):
	# Capture frame-by-frame
	ret, image = cap.read()

	# Our operations on the frame come here
	gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

	'''
	image = cv2.imread('./Mila_Kunis_face_tilt.jpg')
	gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
	'''

	rects = detector(gray, 1)

	for (i, rect) in enumerate(rects):
		shape = predictor(gray, rect)
		indicators = face_utils.shape_to_np(shape)
		x, y, w, h = face_utils.rect_to_bb(rect)
		cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

		for (x, y) in indicators:
			cv2.circle(image, (x, y), 1, (0, 0, 255), -1)

	left = np.mean(indicators[36:42], axis=0).astype(int)
	right = np.mean(indicators[42:48], axis=0).astype(int)
	cv2.line(image, tuple(left), tuple(right), (255, 0, 0), 3)
	angle = int(math.atan((right[1] - left[1]) / (right[0] - left[0])) * 180 / math.pi)

	print(str(angle) + ' degree!!')
	cv2.imshow("output", image)
	if cv2.waitKey(1) & 0xFF == ord('q'):
		break
	#cv2.waitKey(0)
	#cv2.destroyAllWindows()
	
cap.release()
cv2.destroyAllWindows()