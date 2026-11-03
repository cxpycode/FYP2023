from flask import Flask 
from flask_socketio import SocketIO
import paho.mqtt.client as mqtt
from threading import Lock

#Background Thread
thread = None
thread_lock = Lock()

#Topics
tempTopic = "sensor/temperature"
humiTopic = "sensor/humidity"
quanTopic = "sensor/quantity"


#   Creating Web Application
def create_app():
    app = Flask(__name__)
    socketio = SocketIO(app)
    app.config['SECRET_KEY'] = "matthrew"
    

    #Broker Configurations
    app.config['MQTT_BROKER_URL'] = "192.168.1.102"
    app.config['MQTT_BROKER_PORT'] = 1883
    app.config['MQTT_TOPICS'] = [tempTopic, humiTopic, quanTopic]
    app.config['MQTT_CLIENT_ID'] = 'flaskman'

    #Initialize MQTT Client
    mqtt_client = mqtt.Client(app.config['MQTT_CLIENT_ID'])
    mqtt_client.connect(app.config['MQTT_BROKER_URL'], app.config['MQTT_BROKER_PORT'],120)


    #Handle MQTT Connection
    def on_connect(client, userdata, flags, rc):
        print("Connected to MQTT Broker.")
        for topic in app.config['MQTT_TOPICS']:
            client.subscribe(topic)

    def on_message(client, userdata, msg):
        payload = float(msg.payload.decode('utf-8'))
        if msg.topic == tempTopic:
            global temperature
            temperature = payload
            # socketio.emit("update_temperature", temperature)
            
        elif msg.topic == humiTopic:
            global humidity
            humidity = payload
            # socketio.emit("update_humidity", humidity)
            
        elif msg.topic == quanTopic:
            global quantity
            quantity = payload
        #     # socketio.emit("update_quantity", quantity)
    
    # #MQTT Call back
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.loop_start()


    #Register SocketIO instance with Flask app
    app.config['SOCKETIO'] = socketio

    #Background function
    def background_thread():
        print("Getting sensor readings")
        while True:
            socketio.emit("update_temperature", temperature)
            socketio.emit("update_humidity", humidity)
            socketio.emit("update_quantity", quantity)
            socketio.sleep(1)

    #SocketIO Event Handlers
    @socketio.on('connect')
    def handle_connect():
        global thread
        print("Client connected")

        global thread
        with thread_lock:
            if thread is None:
                thread = socketio.start_background_task(background_thread)

    @socketio.on('disconnected')
    def handle_disconnected():
        print("Client disconnected")

    from .views import views
    from .auth import auth

    app.register_blueprint(views, url_prefix = '/')
    app.register_blueprint(auth, url_prefix = '/')

    return app



# #MQTT broker Settings
# mqttPort = 1883
# mqttIP = "192.168.1.102"
# tempTopic = "sensor/DHT11/temperature"
# humiTopic = "sensor/DHT11/humidity"
# #Initialize MQTT Client
# mqtt_client = mqtt.Client()

# #Handle MQTT Connection
# def on_connect(client, userdata, flags, rc):
#     print("Connected to MQTT Broker.")
#     client.subscribe(tempTopic)
#     client.subscribe(humiTopic)

# def on_message(client, userdata, msg):
#     topic = msg.topic
#     payload = float(msg.payload.decode())

#     if topic == tempTopic:
#         temperature = payload
#         socketio.emit("update_temperature", 
#                       {"data":temperature})
#     elif topic == humiTopic:
#         humidity = payload
#         socketio.emit("update_humidity", 
#                       {"data":humidity})


# #MQTT Call back
# mqtt_client.on_connect = on_connect
# mqtt_client.on_message = on_message

# #Start MQTT Client
# mqtt_client.connect(mqttIP,mqttPort,120)
# mqtt_client.loop_start()


# #   SocketIO Event handler
# @socketio.on('connect')
# def handle_connect():
#     print("Client connected")



# @socketio.on('request_data')
# def handle_request_data():
#     print('Received message')
#     socketio.emit('updata_data',{'tempValue':temperature,'humiValue':humidity})