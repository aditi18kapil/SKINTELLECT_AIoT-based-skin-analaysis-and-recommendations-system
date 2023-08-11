import requests

# Replace 'YOUR_TELEGRAM_BOT_TOKEN' with your actual Telegram bot token
telegram_bot_token = '6427662431:AAFx4XMP_iqickRqzrudeA-WUsH1Y3h436I'

def get_chat_id():
    telegram_api_url = f"https://api.telegram.org/bot{telegram_bot_token}/getUpdates"
    response = requests.get(telegram_api_url)
    data = response.json()
    
    if "result" in data and data["result"]:
        # Assuming the latest message is the most recent one, get the chat ID from it
        chat_id = data["result"][-1]["message"]["chat"]["id"]
        return chat_id
    else:
        raise ValueError("Failed to retrieve chat ID. Make sure your bot has received messages.")

if __name__ == "__main__":
    try:
        chat_id = get_chat_id()
        print("Chat ID:", chat_id)
    except Exception as e:
        print("Error:", e)
