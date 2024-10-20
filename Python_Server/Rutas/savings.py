# Rutas/savings.py
from flask import request, jsonify
from App.application import app, firebase_db, firebase_storage, get_first_level # Importar la aplicación de Flask y la instancia de Firebase
import datetime

# Ruta para calcular litros de agua
@app.route('/api/calculate', methods=['GET'])
def calculate():
    email = request.args.get('email')

    if not email:
        return jsonify({"error": "Se necesita de un email."}), 400

    currentDate = datetime.datetime.now()

    total_litros_dia = calcular_por_dia(currentDate, email)
    total_litros_semana = calcular_por_semana(currentDate, email)
    total_litros_mes = calcular_por_mes(currentDate, email)

    return jsonify({
        "total_litros_dia": total_litros_dia,
        "total_litros_semana": total_litros_semana,
        "total_litros_mes": total_litros_mes
    }), 200


def calcular_por_dia(diaActual, email):
    # Calcular el rango para el día actual
    startDate = diaActual.replace(hour=0, minute=0, second=0, microsecond=0)
    endDate = diaActual.replace(hour=23, minute=59, second=59, microsecond=999999)
    
    # Obtener datos de Firebase para el día
    ref = firebase_db.reference(f'/usuarios/usuario/{email}/unidades')
    unidades = ref.get()

    total_litros = 0
    for unidad_id, unidad_data in unidades.items():
        flujo_ref = unidad_data['sensores']['flujo']['fechas']
        flujo_list = {date: horas for date, horas in flujo_ref.items() if startDate.date().isoformat() <= date <= endDate.date().isoformat()}

        # Sumar la cantidad de litros
        for date, horas in flujo_list.items():
            if not horas:
                continue
            for hour, minutos in horas['horas'].items():
                for minute, segundos in minutos['minutos'].items():
                    for second, data in segundos['segundos'].items():
                        if 'Flujo' in data:
                            total_litros += data['Flujo']
    
    return round(total_litros, 2)


def calcular_por_semana(diaActual, email):
    # Calcular el rango para la semana actual
    startDate = diaActual - datetime.timedelta(days=diaActual.weekday())
    startDate = startDate.replace(hour=0, minute=0, second=0, microsecond=0)
    endDate = startDate + datetime.timedelta(days=6, hours=23, minutes=59, seconds=59, microseconds=999999)

    # Obtener datos de Firebase para la semana
    ref = firebase_db.reference(f'/usuarios/usuario/{email}/unidades')
    unidades = ref.get()

    total_litros = 0
    for unidad_id, unidad_data in unidades.items():
        flujo_ref = unidad_data['sensores']['flujo']['fechas']
        flujo_list = {date: horas for date, horas in flujo_ref.items() if startDate.date().isoformat() <= date <= endDate.date().isoformat()}

        # Sumar la cantidad de litros
        for date, horas in flujo_list.items():
            if not horas:
                continue
            for hour, minutos in horas['horas'].items():
                for minute, segundos in minutos['minutos'].items():
                    for second, data in segundos['segundos'].items():
                        if 'Flujo' in data:
                            total_litros += data['Flujo']
    
    return round(total_litros, 2)

def calcular_por_mes(diaActual, email):
    # Calcular el rango para el mes actual
    startDate = diaActual.replace(day=1, hour=0, minute=0, second=0, microsecond=0)
    endDate = (startDate + datetime.timedelta(days=32)).replace(day=1) - datetime.timedelta(milliseconds=1)

    # Obtener datos de Firebase para el mes
    ref = firebase_db.reference(f'/usuarios/usuario/{email}/unidades')
    unidades = ref.get()

    total_litros = 0
    for unidad_id, unidad_data in unidades.items():
        flujo_ref = unidad_data['sensores']['flujo']['fechas']
        flujo_list = {date: horas for date, horas in flujo_ref.items() if startDate.date().isoformat() <= date <= endDate.date().isoformat()}

        # Sumar la cantidad de litros
        for date, horas in flujo_list.items():
            if not horas:
                continue
            for hour, minutos in horas['horas'].items():
                for minute, segundos in minutos['minutos'].items():
                    for second, data in segundos['segundos'].items():
                        if 'Flujo' in data:
                            total_litros += data['Flujo']
    
    return round(total_litros, 2)