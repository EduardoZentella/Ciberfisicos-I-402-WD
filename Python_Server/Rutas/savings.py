'''
from flask import request, jsonify # Importar las librerías necesarias de Flask
from App.application import app, db, get_first_level # Importar la aplicación de Flask y la instancia de Firebase
import datetime

# Ruta para crear un nuevo recurso
@app.route('/<path:path>', methods=['POST'])
def create(path):
    data = request.json
    ref = db.reference(f'/{path}')
    return jsonify(data), 201

def calculatePro():
    data = request.json
    startDate_str = data.get
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
endDate -= datetime.timedelta(milliseconds=1)'''

# Rutas/savings.py
from flask import request, jsonify
from App.application import app, db
import datetime

# Ruta para calcular litros de agua
@app.route('/calculate', methods=['POST'])
def calculate():
    data = request.json
    startDate_str = data.get("startDate", "")

    formats = ["%Y-%m-%d", "%Y-%m", "%Y"]
    formatUsed = None
    startDate = None

    # Intentar convertir la fecha en los distintos formatos
    for fmt in formats:
        try:
            startDate = datetime.datetime.strptime(startDate_str, fmt)
            formatUsed = fmt
            break
        except ValueError:
            continue

    if not formatUsed:
        return jsonify({"error": "Formato de fecha no válido"}), 400

    # Determinar la función a usar según el formato
    if formatUsed == "%Y-%m-%d":
        total_litros = calcular_por_dia(startDate)
    elif formatUsed == "%Y-%m":
        total_litros = calcular_por_mes(startDate)
    elif formatUsed == "%Y":
        total_litros = calcular_por_anio(startDate)

    return jsonify({"total_litros": total_litros}), 200


def calcular_por_dia(startDate):
    # Calcular el rango para un día
    endDate = startDate + datetime.timedelta(days=1) - datetime.timedelta(milliseconds=1)
    
    # Obtener datos de Firebase para el día
    ref = db.reference('/flujo_agua')
    flujo_list = ref.order_by_key().start_at(startDate.isoformat()).end_at(endDate.isoformat()).get()

    # Sumar la cantidad de litros
    total_litros = sum(item['litros'] for item in flujo_list.values() if 'litros' in item)
    
    return total_litros


def calcular_por_mes(startDate):
    # Calcular el rango para un mes
    endDate = startDate + datetime.timedelta(days=30) - datetime.timedelta(milliseconds=1)

    # Obtener datos de Firebase para el mes
    ref = db.reference('/flujo_agua')
    flujo_list = ref.order_by_key().start_at(startDate.isoformat()).end_at(endDate.isoformat()).get()

    # Sumar la cantidad de litros
    total_litros = sum(item['litros'] for item in flujo_list.values() if 'litros' in item)

    return total_litros


def calcular_por_anio(startDate):
    # Calcular el rango para un año
    endDate = startDate + datetime.timedelta(days=365) - datetime.timedelta(milliseconds=1)

    # Obtener datos de Firebase para el año
    ref = db.reference('/flujo_agua')
    flujo_list = ref.order_by_key().start_at(startDate.isoformat()).end_at(endDate.isoformat()).get()

    # Sumar la cantidad de litros
    total_litros = sum(item['litros'] for item in flujo_list.values() if 'litros' in item)

    return total_litros


#request me da el startdate, lo agarro del request json, calculo el endate con la funcion con rectificar el formato, hacder un reference de fechas
#me regresa un diccionario con las fechas, filtrar para que me de las fechas del rango y con eso tengo todos los paths 
#guardarlo en flujo list los que vaya agarrando en esas fechas