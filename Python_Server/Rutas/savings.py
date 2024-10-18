# Rutas/savings.pyç
from flask import request, jsonify # Importar las librerías necesarias de Flask
from App.application import app, db, get_first_level # Importar la aplicación de Flask y la instancia de Firebase
import datetime

# Ruta para crear un nuevo recurso
@app.route('/promedio/<path:path>', methods=['POST'])

#request me da el startdate, lo agarro del request json, calculo el endate con la funcion con rectificar el formato, hacder un reference de fechas
#me regresa un diccionario con las fechas, filtrar para que me de las fechas del rango y con eso tengo todos los paths 
#guardarlo en flujo list los que vaya agarrando en esas fechas

def promedio(path):
    data = request.json
    ref = db.reference(f'/{path}')
    return jsonify(data), 201

def calculatePro():
    startDate = datetime.datetime.now()
    endDate = None
    formats = ["%Y-%m-%d", "%Y-%m", "%Y"]
    formatUsed = None
    for fmt in formats:
        try:
            if fmt == startDate.strftime(format/fmt):
                formatUsed = fmt
                break
        except ValueError:
            continue
    match formatUsed:
        case "%Y-%m-%d":
            endDate = startDate + datetime.timedelta(days=1)
        case "%Y-%m":
            endDate = startDate + datetime.timedelta(days=30)
        case "Y":
            endDate = startDate + datetime.timedelta(days=365)
    endDate -= datetime.timedelta(milliseconds=1)






