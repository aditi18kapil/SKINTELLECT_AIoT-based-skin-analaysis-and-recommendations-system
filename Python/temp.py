import recommend


def temp_hum(result_text,final_mail_id,ph_no):
    import requests
# import app

# Replace 'YOUR_API_KEY' with your actual Weatherstack API key
    api_key = '529ed1a4dad3ab5d82f9a550525abaf7'

# Replace 'YOUR_CITY' with the city name for which you want to get weather data
    city = 'pune'  # assuming it is only installed in pune

# API endpoint URL
    url = f'http://api.weatherstack.com/current?access_key={api_key}&query={city}'

# Send API request and get response data
    response = requests.get(url)
    data = response.json()

# Check if the request was successful
    if response.status_code == 200:
    # Extract temperature and humidity data from the response
       temperature = data['current']['temperature']
       humidity = data['current']['humidity']
       result_text += "Temperature of place: " + str(temperature) + "\n"
       result_text += "Humidity of place: " + str(humidity) + "\n"
       recommend.recommend_chat(result_text,final_mail_id,ph_no)
   
    
    else:
      print('Failed to fetch weather data.')
