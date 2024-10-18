# Rutas/pivision.py
import os # Importar la librería os
from flask import request, jsonify
from pydantic import ValidationError # Importar las librerías necesarias de Flask
from App.application import app, firebase_db, firebase_storage, get_first_level # Importar la aplicación de Flask y la instancia de Firebase
import requests # Importar la librería requests
from dotenv import load_dotenv # Importar la librería load_dotenv
from Models.models import PiVisionRequest

# Ruta para manejar llamadas al PiVision Web API
@app.route('/pivision', methods=['GET', 'POST'])
def pivision():
    # URL base de PiVision
    base_url = os.getenv('PIVISION_URL')
    # Token de autenticación
    token = os.getenv('PIVISION_TOKEN')
    # Headers para la petición
    headers = {
        'Authorization': f'Basic {token}',
        'Content-Type': 'application/json'
    }
    try:
        # Valida el JSON de entrada usando Pydantic
        data = PiVisionRequest(**request.json)
    except ValidationError as e:
        print(e)
        return jsonify(e.errors()), 400
    
    if not base_url or not token:
        return jsonify({'error': 'PIVISION_URL or PIVISION_TOKEN not set'}), 500
    try:
        if request.method == 'GET':
            # Realizar la petición a PiVision
            print(f'{base_url}/{data.path}')
            response = requests.get(f'{base_url}/{data.path}', headers=headers, verify=False)
        elif request.method == 'POST':
            # Realizar la petición a PiVision
            response = requests.post(f'{base_url}/{data.path}', headers=headers, json=data.body, verify=False)
        response.raise_for_status()
        return jsonify(response.json())
    except requests.exceptions.HTTPError as e:
        if e.response.status_code == 404:
            return jsonify({'error': str(e)}), 404
        return jsonify({'error': str(e)}), 500
    except requests.exceptions.RequestException as e:
        return jsonify({'error': str(e)}), 500
