# Rutas/crud.py
import base64
from datetime import timedelta
from pydantic import ValidationError
from Models.models import ImageUpload # Importar el modelo de Pydantic
from PIL import Image # Importar la librería para trabajar con imágenes
from io import BytesIO # Importar la librería para trabajar con datos binarios
from flask import request, jsonify # Importar las librerías necesarias de Flask
from App.application import app, firebase_db, firebase_storage, get_first_level # Importar la aplicación de Flask y la instancia de Firebase

# Ruta para crear un nuevo recurso
@app.route('/api/<path:path>', methods=['POST'])
def create(path):
    data = request.json
    ref = firebase_db.reference(f'/{path}')
    ref.push(data)
    return jsonify(data), 201

# Ruta para obtener uno o todos los recursos
@app.route('/api/<path:path>', methods=['GET'])
def read(path):
    ref = firebase_db.reference(f'/{path}')
    instances = ref.get()    
    if not instances:
        return jsonify({"error": "Recurso no encontrado"}), 404
    # Obtener solo el primer elemento en cada nivel
    filtered_instances = get_first_level(instances)    
    return jsonify(filtered_instances)

# Ruta para actualizar un recurso por ID
@app.route('/api/<path:path>', methods=['PUT'])
def update(path):
    data = request.json
    ref = firebase_db.reference(f'/{path}')
    instance = ref.get()
    if not instance:
        return jsonify({"error": "Recurso no encontrado"}), 404
    ref.update(data)
    return jsonify(data)

# Ruta para eliminar un recurso por ID
@app.route('/api/<path:path>', methods=['DELETE'])
def delete(path):
    ref = firebase_db.reference(f'/{path}')
    instance = ref.get()
    if not instance:
        return jsonify({"error": "Recurso no encontrado"}), 404
    ref.delete()
    return jsonify({"message": "Recurso eliminado"}), 200

@app.route('/api/upload-image', methods=['POST'])
def upload_image():
    try:
        # Valida el JSON de entrada usando Pydantic
        data = ImageUpload(**request.json)
    except ValidationError as e:
        return jsonify(e.errors()), 400

    # Decodifica la imagen base64
    image_data = base64.b64decode(data.image)
    image = Image.open(BytesIO(image_data))

    # Guarda la imagen en un buffer
    buffer = BytesIO()
    image.save(buffer, format="JPEG")
    buffer.seek(0)

    # Sube la imagen a Firebase Storage
    bucket = firebase_storage.bucket()
    blob = bucket.blob(data.path)
    blob.upload_from_file(buffer, content_type="image/jpeg")

    # Devuelve la URL de descarga de la imagen
    download_url = blob.generate_signed_url(timedelta(minutes=15))
    return jsonify({'download_url': download_url}), 201

# Ruta para obtener los valores mas recientes de una unidad
@app.route('/api/get_latest_flujo', methods=['GET'])
def get_latest_flujo():
    email = request.args.get('email')
    unidad = request.args.get('unidad')
    if not email:
        return jsonify({"error": "Se necesita de un email."}), 400
    if not unidad:
        return jsonify({"error": "Se necesita de un unidad Id."}), 400

    ref = firebase_db.reference(f'/usuarios/usuario/{email}/unidades/{unidad}/sensores/flujo/fechas')
    fechas = ref.get()
    
    if not fechas:
        return jsonify({"error": "Datos no encontrados."}), 404

    latest_date = max(fechas.keys())
    latest_hour = max(fechas[latest_date]['horas'].keys())
    latest_minute = max(fechas[latest_date]['horas'][latest_hour]['minutos'].keys())
    latest_second = max(fechas[latest_date]['horas'][latest_hour]['minutos'][latest_minute]['segundos'].keys())
    
    latest_flujo = fechas[latest_date]['horas'][latest_hour]['minutos'][latest_minute]['segundos'][latest_second]['Flujo']
    
    return jsonify({"latest_flujo": latest_flujo}), 200
