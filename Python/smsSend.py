


def send_sms(ph_no,result_text,recommendation):
    from twilio.rest import Client
    final_text = result_text+"\n"+"\n"+recommendation
    SID = 'ACa0c77eaa8f7d080f5c8bb826c887ba98'
    AUTH_TOKEN = 'd0effba4995c97f648caa1bc97e316a0'

    cl = Client(SID, AUTH_TOKEN)

    cl.messages.create(body=final_text, from_='+13252465808', to=ph_no)

    print("SMS SENT SUCCESSFULLY ! ")
