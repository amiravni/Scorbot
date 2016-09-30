import cv2
# Create the haar cascade
faceCascade = cv2.CascadeClassifier("C:\WinPython2.7\python-2.7.10.amd64\Lib\site-packages\opencv_python-2.4.13.dist-info\haarcascade_frontalface_default.xml")

cam = cv2.VideoCapture(0)
ret_val, img = cam.read()
cv2.imshow('my webcam', img)
while True:
    ret_val, img = cam.read()
    img = cv2.flip(img,1)
    
    if cv2.waitKey(1) == 27: 
        break  # esc to quit

    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # Detect faces in the image
    faces = faceCascade.detectMultiScale(
        gray,
        scaleFactor=1.1,
        minNeighbors=5,
        minSize=(30, 30),
        flags = cv2.cv.CV_HAAR_SCALE_IMAGE
    )      
    print "Found {0} faces!".format(len(faces))
    
    # Draw a rectangle around the faces
    idx = 0
    delta = 30
    for (x, y, w, h) in faces:
        #cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), 2)
        img2 = img[ max(1,y-delta):min(480,y+h+delta),max(x-delta, 1):min(640,x+w+delta)]
        img3 =  cv2.flip(img2,1)
        img2[:,img2.shape[1]/2:img2.shape[1] ] = img3[:, img2.shape[1]/2:img2.shape[1] ]        
        cv2.imshow('face1', img2) 
        cv2.waitKey(0)
        #cv2.imshow('face2', img3)         
        cv2.imwrite('c:/cv/face.jpg', img2) 
        idx = 1        
        break
    if idx > 1:
        gray = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
        
        gray = cv2.bilateralFilter(gray, 11, 17, 17)
        #edged = cv2.Canny(gray, 30, 200)
        #cv2.imshow('edges', edged)   
        # find contours in the edged image, keep only the largest
        # ones, and initialize our screen contour
        (cnts, _) = cv2.findContours(edged.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        cnts = sorted(cnts, key = cv2.contourArea, reverse = True)[:10]
        screenCnt = None    
        # loop over our contours
        for c in cnts:
            # approximate the contour
            peri = cv2.arcLength(c, True)
            approx = cv2.approxPolyDP(c, 0.02 * peri, True)
        
            cv2.drawContours(img2, [approx], -1, (0, 255, 0), 3)
            #cv2.imshow("Game Boy Screen", img2)
            #cv2.waitKey(0)  
    
    #cv2.imshow('my webcam', img)
    #cv2.waitKey(0)
    
    
        
cv2.destroyAllWindows()

#cv2.destroyAllWindows()