from flask import Flask, request, jsonify
import paho.mqtt.client as mqtt
import WrinkleDetection

app = Flask(__name__)

stored_text = ""
stored_phone_number = ""
stored_age = ""
stored_gender = ""
response_text = ""
temp = ""
hum = ""

# MQTT broker configuration
broker_address = "10.21.70.16"
broker_port = 1883
mqtt_topic = "W5300-CAM"
response_topic = "CAM-W5300"

# Callback function when the client connects to the broker
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Subscribe to the response topic
    client.subscribe(response_topic)

# Callback function when a message is received from the broker
def on_message(client, userdata, msg):
    global response_text
    # print("this is what we received from mqtt: ", response_text)
    if msg.topic == response_topic:
        new_response_text = msg.payload.decode('utf-8')
        # print("ye pta lgayega: ", new_response_text)
        if new_response_text.startswith("http://"):
            response_text = new_response_text
            finalmailid = WrinkleDetection.image_save(stored_text, response_text)
            image_url = response_text  # Replace with your actual Arducam-generated image URL
            save_path = 'C:/Users/Aditi/OneDrive/Desktop/SkinTellect/'  # Replace with your desired save location on
            WrinkleDetection.save_image_from_url(image_url, save_path)
            wrinkles = WrinkleDetection.detect()
            tone = WrinkleDetection.skin_tone_analysis()
            contrastt = WrinkleDetection.skin_texture_analysis()
            pig = WrinkleDetection.pigmentation_analysis()
            WrinkleDetection.letdo(finalmailid, stored_phone_number, wrinkles, tone, contrastt, pig, stored_age, stored_gender)

# Create an MQTT client instance
client = mqtt.Client()

# Set the callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to the broker
client.connect(broker_address, broker_port)

# Start the MQTT loop to handle incoming messages in the background
client.loop_start()

@app.route('/')
def index():
    # Read and return the HTML file
    with open('templates/index.html') as f:
        return f.read()

@app.route('/mqtt-publish', methods=['POST'])
def mqtt_publish():
    global stored_text, stored_phone_number, stored_age, stored_gender
    text = request.form.get('text')
    phone_number = request.form.get('phone-number')
    stored_text = text
    stored_phone_number = phone_number
    stored_age = request.form.get('age')  # Store the age in the variable
    stored_gender = request.form.get('gender')  # Store the gender in the variable

    client.publish(mqtt_topic, "cmd:capture")

    return jsonify({'message': 'Text, phone number, age, and gender saved successfully'})

@app.route('/get-stored-text', methods=['GET'])
def get_stored_text():
    global stored_text
    return jsonify({'stored_text': stored_text})

@app.route('/get-response-text', methods=['GET'])
def get_response_text():
    global response_text
    return jsonify({'response_text': response_text})

if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True)
