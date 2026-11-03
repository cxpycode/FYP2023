# Defining routes for authentication of user purposes
from flask import Blueprint, render_template, redirect, request, flash 

auth = Blueprint('auth', __name__)

#Define the blueprint
@auth.route('/', methods = ['GET', 'POST'])
def login():
    if request.method == 'POST':
        pw = "admin123"     #Set admin password
        ur = "admin"        #set admin username
        enter_pass = request.form.get('Password')
        enter_user = request.form.get('Username')

        if enter_pass!=pw or enter_user!=ur:
            flash('Login Credentials are Incorrect.', category='error')
        else: 
            return redirect('http://127.0.0.1:5000/dashboard')
         
    return render_template("login.html", boolean=True)

@auth.route('/logout', methods = ['GET', 'POST'])
def logout():
    return "<p>Logout</p>"




