import cv2
import numpy as np
import requests
import os
from mtcnn.mtcnn import MTCNN

# import mailsend
import combine


from skimage import color
from skimage.feature import graycomatrix, graycoprops




wrinkles = ""
tone = ""
contrastt = ""
diss = ""
pig = ""


stored_text = ""
# response_text = ""

def image_save(stored_text2,response_text2):
    stored_text = stored_text2
    # print("lets see !!!")
    # print("this is st",stored_text2)
    # print("           ")
    # print("this is url",response_text2)
    # return stored_text
    return stored_text

image_url=""
save_path=""
def save_image_from_url(image_url, save_path):
    try:
        # Send a GET request to the image URL
        response = requests.get(image_url)
        response.raise_for_status()  # Check for any errors in the request

        # Get the file name from the URL
        file_name = os.path.basename(image_url)

        # Combine the file name with the save_path to create the complete file path
        file_path = os.path.join(save_path, "arducam_pic.jpg")

        # Save the image to the specified location
        with open(file_path, 'wb') as file:
            file.write(response.content)

        # print("Image saved successfully!")
        return True

    except Exception as e:
        print(f"Error occurred: {e}")
        return False


img = cv2.imread("C:/Users/Aditi/OneDrive/Desktop/SkinTellect/arducam_pic.jpg")

def detect():
#    print("let's start...............")
    # Load the pre-trained MTCNN face detector
    detector = MTCNN()

    # Read the input image
    # img = cv2.imread("C:/Users/Aditi/OneDrive/Desktop/SkinTellect/arducam_pic.jpg")

    # Detect faces in the image using MTCNN
    faces = detector.detect_faces(img)

    # Check if any faces were detected  
    if len(faces) > 0:
        for face in faces:
            x, y, w, h = face['box']

            # Crop the face region
            face_region = img[y:y+h, x:x+w]

            # Convert the face region to grayscale
            gray_face = cv2.cvtColor(face_region, cv2.COLOR_BGR2GRAY)

            # Apply Canny edge detection on the grayscale face region
            edges = cv2.Canny(gray_face, 130, 1000)

            # Count the number of edges in the cropped face region
            num_edges = np.count_nonzero(edges)

            if num_edges > 1000:
                # print("Wrinkle Found on this face!")
                found = "Wrinkles found"
                return "wrinkles found"
                # mailsend.send_mailToCustomer(found,finalmailid)
                # combine.var(found,finalmailid)
            else:
                # print("No Wrinkle Found on your face!")
                found = "No Wrinkle found"
                return "wrinkles not found"
                # mailsend.send_mailToCustomer(found,finalmailid)
                # combine.var(found,finalmailid)
    else:
        print("No face detected in the image.")



# skintone

def skin_tone_analysis():
    # Load the image using OpenCV
    # img = cv2.imread(image_path)
    
    # Convert the image from BGR to RGB color space
    img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    
    # Convert the RGB image to LAB color space
    img_lab = color.rgb2lab(img_rgb)
    
    # Extract the L*, a*, and b* channels
    l_channel, a_channel, b_channel = cv2.split(img_lab)
    
    # Calculate the mean a* and b* values
    mean_a = np.mean(a_channel)
    mean_b = np.mean(b_channel)
    
    # Calculate the skin tone index (STI)
    sti = np.sqrt(mean_a ** 2 + mean_b ** 2)
    
    # Determine the skin tone category based on the STI value
    if sti < 20:
        skin_tone_category = "Dark skin"
    elif sti < 40:
        skin_tone_category = "Medium skin"
    else:
        skin_tone_category = "Light skin"

    # combine.var(found,final)
    return skin_tone_category
    tone = skin_tone_category
    print("Skin tone : ",skin_tone_category)
    

# Replace 'path_to_image.jpg' with the actual path to your image
# image_path = 'C:/Users/Aditi/OneDrive/Desktop/SkinTellect/arducam_pic.jpg'
# result = skin_tone_analysis(image_path)
# print("Skin Tone Category:", result)


# skintexture



def skin_texture_analysis():
    # Load the image using OpenCV
    # img = cv2.imread(image_path)
    
    # Convert the image to grayscale
    gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    # Calculate the GLCM with specific properties (contrast and dissimilarity)
    glcm = graycomatrix(gray_img, [1], [0], 256, symmetric=True, normed=True)
    contrast = graycoprops(glcm, 'contrast')[0, 0]
    dissimilarity = graycoprops(glcm, 'dissimilarity')[0, 0]
    # print("contrast : ",contrast)
    # print("dissimilarity : ",dissimilarity)
    return contrast
    contrastt = contrast
    diss = dissimilarity
    # Display the results
    # print("Contrast:", contrast)
    # print("Dissimilarity:", dissimilarity)
    # final_skintextanalysis = contrast.append(dissimilarity)

# Replace 'path_to_image.jpg' with the actual path to your image
# image_path = 'C:/Users/Aditi/OneDrive/Desktop/SkinTellect/arducam_pic.jpg'
# skin_texture_analysis(image_path)


# pigmentation


def pigmentation_analysis():
    # Load the image using OpenCV
    # img = cv2.imread(image_path)
    
    # Check if the image is loaded successfully
    if img is None:
        print("Error: Unable to load the image.")
        return None
    
    # Convert the image from BGR to HSV color space
    img_hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    # Define the lower and upper bounds of pigmented color in HSV
    lower_bound = np.array([0, 50, 50])   # Lower bound for hue (red)
    upper_bound = np.array([20, 255, 255])  # Upper bound for hue (yellow)
    
    # Create a mask to isolate pigmented regions
    mask = cv2.inRange(img_hsv, lower_bound, upper_bound)
    
    # Count the number of pigmented pixels
    pigmented_pixels = np.sum(mask == 255)
    
    # Calculate the total number of pixels in the image
    total_pixels = mask.shape[0] * mask.shape[1]
    
    # Calculate the percentage of pigmented pixels in the image
    pigmentation_percentage = (pigmented_pixels / total_pixels) * 100
    
    pig = pigmentation_percentage
    # print("ye zroor hoga : ",pig)
    # print("Pigmentation : ",pigmentation_percentage)
    return pigmentation_percentage

# Replace 'path_to_image.jpg' with the actual path to your image
# image_path = 'C:/Users/Aditi/OneDrive/Desktop/SkinTellect/arducam_pic.jpg'
# result = pigmentation_analysis(image_path)

# if result is not None:
#     print("Percentage of Pigmentation:", result)

def letdo(finalmailid,ph_no,wrinkles,tone,contrastt,pig,age,gender):
    pigm=""
    cont=""
    if(contrastt>50):
        cont='HIGH'
    elif(20 < contrastt <= 50):
        cont='MEDIUM'
    else:
        cont='LOW'    

    if(pig>50):
        pigm='HIGH'
    elif(20 < pig <= 50):
        pigm='MEDIUM'
    else:
        pigm='LOW'    
   
    combine.var(finalmailid,ph_no, wrinkles,tone,cont,pigm,age,gender)
    #  combine.index()

