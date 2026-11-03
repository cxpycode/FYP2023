from flask import Blueprint, render_template, current_app
#import requests
#import json
views = Blueprint('views', __name__)


#Dashboard Route
@views.route('/dashboard', methods=['GET','POST'])
def dashboard():  
    return render_template('dashboard.html')
    


#Code for views.dashboard (HTTP Request)

    # url = "http://192.168.1.100/getValues"
    # response = requests.get(url)

    # if response.status_code == 200:
    #     readings = json.loads(response.content)

    #     #Variables for Temperature
    #     head1 = readings[0]['name']
    #     value1 = round(readings[0]['value'],2)
    #     unit1 = readings[0]['unit']
        
    #     #Variables for Humidity
    #     head2 = readings[1]['name']
    #     value2 = round(readings[1]['value'],2)
    #     unit2 = readings[1]['unit']
        
    #     return render_template('dashboard.html', 
    #                            tempHead = head1, 
    #                            tempValue = value1, 
    #                            tempUnit = unit1, 
    #                            humiHead = head2, 
    #                            humiValue = value2, 
    #                            humiUnit = unit2)
        
    # else:
    #      return render_template('dashboard.html', 
    #                             tempReading = 'Failed to retrieve sensor data.', 
    #                             humiReading = 'Failed to retrieve sensor data.',)


        

    

    
