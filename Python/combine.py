import temp
import recommend


def var(finalmailid,ph_no,wrinkles,tone,contrastt,pig,age,gender):
    result_text = ""
    final_mail_id=finalmailid
    gender=gender
    age=age
    # Append the values with labels to the new variable
    result_text += "Gender: " + str(gender) + "\n"
    result_text += "Age: " + str(age) + "\n"
    result_text += "Wrinkles: " + str(wrinkles) + "\n"
    result_text += "Skin Tone: " + str(tone) + "\n"
    result_text += "Skin Contrast: " + str(contrastt) + "\n"
    result_text += "Skin Pigmentation: " + str(pig) + "\n"
   
   
    
    temp.temp_hum(result_text,final_mail_id,ph_no)
    




