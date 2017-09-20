import cv2
import numpy as np
#import skvideo.io  # used to import video files, as cv2 didn't work on my computer

# Threshold
THRESHOLD_LEVEL = 157 # 0-255

# Hough line parameters
DEG_ACCURACY = 1  # the accuracy of lines direction
LENGTH_MIN = 40  # in pixels, the minimum length of a line  [Probably what have the most impact on the result]

# Line cluster parameters - define when 2 line should be consider as the same
RHO_CLUSTER_INTERVAL = 300  # (pixels) use to compare the position of line according to the origin
THETA_CLUSTER_INTERVAL = 50 * np.pi / 180   # (rad) use to compare the direction of lines

# Input
# Image file
img_colored = cv2.imread('test.png')

# Video file
#video = skvideo.io.vreader("test.avi")

# Live webcam
#cv2.VideoCapture(-1)

#for img_colored in video:  # loop on each frame


# Convert to image to a gray image
gray = cv2.cvtColor(img_colored, cv2.COLOR_BGR2GRAY)

# Blur the image to decrease noise
img = cv2.GaussianBlur(gray, (5,5), 0)

# Separate pixel into two category according to the gray level
ret,img = cv2.threshold(img, THRESHOLD_LEVEL, 255, 0)

# Hough lines computation
lines = cv2.HoughLines(img, 1, np.pi/180 * DEG_ACCURACY, LENGTH_MIN)

clusters = {}  # Clusters of line (rho, theta, weight)

(rho_old, theta_old) = (0 ,0)
(new_rho, new_theta, new_weight) = (0, 0, 0)

if lines is not None: # if lines are found
    for [(rho, theta)] in lines:
        new = True

        # Check if the line isn't too close to one already found
        for (r, t) in clusters:
            if abs(rho - r) < RHO_CLUSTER_INTERVAL and abs(theta - t) < THETA_CLUSTER_INTERVAL: # TODO not sure if this condition define if two lines are close
                weight = clusters[(r, t)]
                new_rho = (r * weight + rho) / (weight + 1)
                new_theta = (t * weight + theta) /(weight + 1)
                new_weight = weight + 1
                (rho_old, theta_old) = (r, t)
                new = False
        # If the line is not close to any other one, create a nex cluster
        if new:
            clusters[(rho, theta)] = 1
        # Otherwise update the corresponding cluster
        else:
            clusters[(new_rho, new_theta)] = new_weight
            del clusters[(rho_old, theta_old)]

    # Draw all clusters
    for (rho, theta) in clusters:
        a = np.cos(theta)
        b = np.sin(theta)
        x0 = a*rho
        y0 = b*rho
        x1 = int(x0 + 1000*(-b))
        y1 = int(y0 + 1000*(a))
        x2 = int(x0 - 1000*(-b))
        y2 = int(y0 - 1000*(a))
        cv2.line(img_colored,(x1,y1),(x2,y2),(0,0,255),2)

    # Get the the cluster with most weight
    #(rho, theta) = sorted(clusters, key = clusters.__getitem__)[-1]

cv2.imshow("threshold", img)
cv2.imshow("lines", img_colored)
cv2.waitKey(0)