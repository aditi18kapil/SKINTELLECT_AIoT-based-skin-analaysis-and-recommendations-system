import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from google.oauth2.credentials import Credentials
from google_auth_oauthlib.flow import InstalledAppFlow
from email.mime.image import MIMEImage
import requests


telegram_bot_token = '6427662431:AAFx4XMP_iqickRqzrudeA-WUsH1Y3h436I'

telegram_chat_id = '6119638322'

def send_telegram_notification(message):
    telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}/sendMessage"
    data = {
        "chat_id": telegram_chat_id,
        "text": message,
    }
    response = requests.post(telegram_api_url, data=data)
    return response.json()


def send_mailToCustomer(stored_text,found,recommendations):
    sender_email = "aditi.kapil@msds.christuniversity.in"
    sender_password = "ornimint123"
    receiver_email = stored_text


    subject = "SKINTELLECT 📑 Your report is ready report !!! "
    body =  f"{found}\n\n{recommendations}"
    image_path = 'arducam_pic.jpg'
# Create a message container (multipart/alternative represents text and HTML formats)
    message = MIMEMultipart()
    message["From"] = sender_email
    message["To"] = receiver_email
    message["Subject"] = subject

# Add the email body to the message
    message.attach(MIMEText(body, 'plain'))
    

    with open(image_path, 'rb') as fp:
     img = MIMEImage(fp.read())
     img.add_header('Content-Disposition', 'attachment', filename=image_path)
     message.attach(img)

  # Connect to the Gmail SMTP server
    try:
      server = smtplib.SMTP_SSL('smtp.gmail.com', 465)
      server.login(sender_email, sender_password)

    # Send the email
      server.sendmail(sender_email, receiver_email, message.as_string())

      print("Email sent successfully!")

    #   notification_message = "Email sent successfully to {}".format(receiver_email)
      send_telegram_notification("SKINTELLECT 📑 Another report generated !")
      


    except Exception as e:
      print("Error: ", e)

    finally:
    # Close the connection to the server
      server.quit()



