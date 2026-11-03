from typing import Final
from telegram import Update
from telegram.ext import Application, CommandHandler, MessageHandler, filters, ContextTypes
import paho.mqtt.client as mqtt

# Telegram Bot Settings
TOKEN: Final = "6403066167:AAGG4nVOf7Vo6c6x3oah_desETOMa4mltrY"
BOT_USERNAME: Final = "@HKSTBot"

# Define MQTT Client
mqtt_client = mqtt.Client()

#MQTT Topics
tempTopic = "sensor/temperature"
humiTopic = "sensor/humidity"
quanTopic = "sensor/quantity"


#MQTT Config
MQTT_BROKER_URL = "XXX.XXX.X.XX" #Input own URL for MQTT
MQTT_BROKER_PORT = 1883
MQTT_TOPICS = [tempTopic, humiTopic, quanTopic]
MQTT_CLIENT_ID = 'flaskman'


# MQTT Callbacks
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT Broker.")
    for topic in MQTT_TOPICS:
        client.subscribe(topic)

def on_message(client, userdata, msg):
    payload = float(msg.payload.decode('utf-8'))
    if msg.topic == tempTopic:
        print(f'temperature: {payload} deg')
        global temperature
        temperature = payload
        return temperature
    elif msg.topic == humiTopic:
        print(f'humidity: {payload}%')
        global humidity
        humidity = payload
        return humidity
    elif msg.topic == quanTopic:  
        print(f'Quantity: {payload}')  
        global quantity
        quantity = payload
        return quantity
       
        
    
# Commands
async def start_command(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text('Hello. I am Bush, Manager Bush. I am telegram bot for HKST Store and I love HKST.')

async def help_command(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text('I can help out with anything but I don\'t know')

async def custom_command(update: Update, context: ContextTypes.DEFAULT_TYPE):
    await update.message.reply_text('What do you need?')


#   Responses
def handle_response(text:str) -> str:
    processed: str = text.lower()

    if 'stock' in processed:
        return f"We have {round(int(quantity))} left!"
    
    if 'temp' in processed:
        return f"The temperature is {temperature}°C"
    
    if 'humi' in processed:
        return f"The humidity is {humidity}%"
    
    return 'I do not understand anything...'


async def handle_message(update: Update, context: ContextTypes.DEFAULT_TYPE):
    message_type: str = update.message.chat.type
    text: str = update.message.text

    print(f'User ({update.message.chat.id}) in {message_type}: "{text}"')
 
    if message_type == 'group':
        if BOT_USERNAME in text:
            new_text: str = text.replace(BOT_USERNAME, '').strip()
            response: str = handle_response(new_text)
        else:
            return
        
    else:
        response: str = handle_response(text)


    print('Bot:', response)
    await update.message.reply_text(response)

async def error(update: Update, context: ContextTypes.DEFAULT_TYPE):
    print(f'Update {update} caused error {context.error}')


if __name__ == '__main__':
    print("Bot has started...")
    app = Application.builder().token(TOKEN).build()

    #Set-up MQTT Connection
    # print("Setting up MQTT Connection")
    mqtt_client = mqtt.Client(MQTT_CLIENT_ID)
    mqtt_client.connect(MQTT_BROKER_URL, MQTT_BROKER_PORT,120)

    # #MQTT Call back
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.loop_start()
        
    # Commands
    app.add_handler(CommandHandler('start', start_command))
    app.add_handler(CommandHandler('help', help_command))
    app.add_handler(CommandHandler('custom', custom_command))

    # Messages
    app.add_handler(MessageHandler(filters.TEXT, handle_message))

    # Errors
    app.add_error_handler(error)

    # Polls the bot
    print("Polling....")
    app.run_polling(poll_interval=3)








