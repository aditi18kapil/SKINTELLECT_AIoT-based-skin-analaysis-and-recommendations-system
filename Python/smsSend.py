


def send_sms(ph_no,result_text,recommendation):
    from twilio.rest import Client
    final_text = result_text+"\n"+"\n"+recommendation
    SID = 'AC30dc3d3a14755fdd175edf525321a404'
    AUTH_TOKEN = '304388f7d60e58434d0e594d2a51340e'

    cl = Client(SID, AUTH_TOKEN)

    cl.messages.create(body=final_text, from_='+18645275279', to=ph_no)

    print("SMS SENT SUCCESSFULLY ! ")
