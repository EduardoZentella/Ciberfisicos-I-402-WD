# Server.py
from App.application import app
from Rutas import crud, gemini, savings

if __name__ == '__main__':
    app.run(debug=True)
