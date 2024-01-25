import cv2
import numpy as np
cap=cv2.VideoCapture(0)

low_orange = np.array([2, 155, 150])
high_orange = np.array([12, 255, 255])
font = cv2.FONT_HERSHEY_COMPLEX
Known_distance = 13.0
Known_width = 9
ret,frame=cap.read()

def Focal_Length_Finder(Known_distance, real_width, width_in_rf_image):

    focal_length = (width_in_rf_image * Known_distance) / real_width
    return focal_length

def obj_data(img):
     obj_width = 0
     hsv=cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
     mask=cv2.inRange(hsv, low_orange, high_orange)
     _,mask1=cv2.threshold(mask, 254,255 ,cv2.THRESH_BINARY)
     cnts,_=cv2.findContours(mask1, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
     for c in cnts:
        if len(cnts)>0:
            largest_contour = max(cnts, key=cv2.contourArea)
            x,y,w,h=cv2.boundingRect(largest_contour)
            cv2.rectangle(img, (x,y), (x+w,y+h), (0,255,0), 2)
            if x>y:
                text2 ="Keadaan : Normal"
            else:
                text2 ="Keadaan : Buntung"
            cv2.putText(frame, text2, (30, 35), font, 0.7, (0, 0, 255), 2)
            obj_width = w
     return obj_width
    

def Distance_finder(Focal_Length, Known_width, obj_width_in_frame):
    distance = (Known_width * Focal_Length)/obj_width_in_frame
    return distance    

ref_image = cv2.imread("/home/pi/python project legacy/korban-orange.png")
ref_image_obj_width = obj_data(ref_image)
Focal_length_found = Focal_Length_Finder(Known_distance, Known_width, ref_image_obj_width)
cv2.imshow("ref_image", ref_image)
print(Focal_length_found)


while True:
    ret,frame=cap.read()
    frame=cv2.resize(frame, (640,480))
    obj_width_in_frame=obj_data(frame)
    if obj_width_in_frame != 0:
        Distance = Distance_finder(Focal_length_found, Known_width, obj_width_in_frame)
        cv2.putText(frame, f"Jarak: {round(Distance,2)} CM", (30, 55), font, 0.6, (255,0,0), 2) 
    cv2.imshow("FRAME",frame)

    if cv2.waitKey(1) & 0xFF==27:
        break
cap.release()
cv2.destroyAllWindows()