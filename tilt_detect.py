from imutils import face_utils
import numpy as np
import imutils
import dlib
import cv2
import math
import time
from multiprocessing import Process, Queue, Manager,Pipe
import multiprocessing

'''
Referenced by : https://www.pyimagesearch.com/2017/04/10/detect-eyes-nose-lips-jaw-dlib-opencv-python/

Links for dlib : https://pypi.org/simple/dlib/
In windows: you have to download dlib-19.8.1-cp36-cp36m-win_amd64.whl to your workspace
and command 'pip install dlib-19.8.1-cp36-cp36m-win_amd64.whl'

Another prerequisite : 'pip install imutils'

'''

detector = dlib.get_frontal_face_detector()
predictor = dlib.shape_predictor('./shape_predictor_68_face_landmarks.dat')

#cap = cv2.VideoCapture(0)
#cap.set(cv2.CAP_PROP_FPS, 5)

def f(id,fi,fl):
	while True:
		image = fi.get()
		gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
		#print("running thread"+str(id))
		
		rects = detector(gray, 1)

		indicators = None
		if(rects == None): 
			continue
		for (i, rect) in enumerate(rects):
			shape = predictor(gray, rect)
			indicators = face_utils.shape_to_np(shape)
			#x, y, w, h = face_utils.rect_to_bb(rect)
			#cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

			for (x, y) in indicators:
				cv2.circle(image, (x, y), 1, (0, 0, 255), -1)

		if(indicators == None): 
			continue
		left = np.mean(indicators[36:42], axis=0).astype(int)
		right = np.mean(indicators[42:48], axis=0).astype(int)
		cv2.line(image, tuple(left), tuple(right), (255, 0, 0), 3)
		angle = int(math.atan((right[1] - left[1]) / (right[0] - left[0])) * 180 / math.pi)

		print(str(angle))
		fl.send(image)

fps_var = 0
if __name__ == '__main__':
	multiprocessing.set_start_method('spawn')

	# global megaman
	with Manager() as manager:
		cap = cv2.VideoCapture(0)
		cap.set(cv2.CAP_PROP_FPS, 60)
		cap.set(cv2.CAP_PROP_FRAME_WIDTH, 400)
		cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 300)
		fi = Queue(maxsize=20)

		threads = 8
		proc = []

		parent_p = []
		thread_p = []
		# procids = range(0,threads)
		for t in range(0,threads):
			p_t,c_t = Pipe()
			parent_p.append(p_t)
			thread_p.append(c_t)
			#print(t)
			proc.append(Process(target=f, args=(t,fi,thread_p[t])))
			proc[t].start()

		useframe = False

		frame_id = 0
		while True:
			# Grab a single frame of video
			ret, frame = cap.read()
			cv2.imshow('Video', frame)

			if frame_id%2 == 0:
				if not fi.full():
					fi.put(frame)
					#print(frame_id)

					cv2.imshow('Video', frame)

					#print("FPS: ", int(1.0 / (time.time() - fps_var)))
					fps_var = time.time()

			#GET ALL DETECTIONS
			for t in range(0,threads):
				if parent_p[t].poll():
					frame_c = parent_p[t].recv()
					cv2.imshow('recc', frame_c)

			frame_id += 1

			# Hit 'q' on the keyboard to quit!
			if cv2.waitKey(1) & 0xFF == ord('q'):
				break
				
		cap.release()
		cv2.destroyAllWindows()
'''
while(True):
	# Capture frame-by-frame
	ret, image = cap.read()
	# Our operations on the frame come here
	gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

	#image = cv2.imread('./Mila_Kunis_face_tilt.jpg')
	#gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
	
	rects = detector(gray, 1)

	for (i, rect) in enumerate(rects):
		shape = predictor(gray, rect)
		indicators = face_utils.shape_to_np(shape)
		#x, y, w, h = face_utils.rect_to_bb(rect)
		#cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

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
'''