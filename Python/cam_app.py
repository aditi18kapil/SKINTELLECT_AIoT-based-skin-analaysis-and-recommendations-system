from flask import Flask, request, send_file, render_template
import os
import time


app = Flask(__name__)



@app.route('/')
def index():
    return render_template('index.html')

@app.route('/upload', methods=['POST'])
def upload():
    global ts

    text_data = request.form.get('text')
    image_data = request.files.get('image')

    # Generate a unique filename based on the current time
    timestamp = int(time.time())
    filename = f"{text_data}_{timestamp}.jpg"
    file_path = os.path.join("images", filename)

    # Save the image to the images directory
    image_data.save(file_path)

    # Generate the URL for accessing the image
    base_url = request.base_url.rsplit('/', 1)[0]
    image_url = f"{base_url}/{file_path}"

    
    return image_url

@app.route('/images/<filename>')
def get_image(filename):
    # Return the requested image file
    return send_file(os.path.join("images", filename))

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)