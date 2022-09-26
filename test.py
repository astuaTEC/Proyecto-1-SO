import cv2
import numpy as np

image = cv2.imread("./imgs/img1-bw.jpg", 0)

image_array = np.array(image)


for i in range(image.shape[0]):
    print("---------")
    for j in range(image.shape[1]):
        print(image_array[i][j])