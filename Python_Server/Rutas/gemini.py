# Rutas/gemini.py
from PIL import Image # Importar la librería para trabajar con imágenes
from io import BytesIO # Importar la librería para trabajar con datos binarios
from flask import request, jsonify
from pydantic import ValidationError # Importar la librería para validar los datos de entrada
from App.application import app, firebase_db, firebase_storage, get_first_level, load_dotenv # Importar la aplicación de Flask y la instancia de Firebase
import google.generativeai as genai # Importar la librería generativeai de Gemini
import os # Importar la librería para interactuar con el sistema operativo
from Models.models import ImageRequest, ContentRequest # Importar los modelos de Pydantic
import cv2
import numpy as np

# Configurar el modelo de Gemini
genai.configure(api_key= os.getenv("GEMINI_API_KEY"))
model = genai.GenerativeModel("gemini-1.5-flash")

# Ruta para analizar imágenes con un prompt
@app.route('/ai/analyze-image', methods=['POST'])
def analyze_image():
    try:
        # Valida el JSON de entrada usando Pydantic
        data = ImageRequest(**request.json)
    except ValidationError as e:
        return jsonify(e.errors()), 400
    
    # Lista para almacenar las imágenes
    images = []
    
    # Descarga cada imagen desde Firebase
    bucket = firebase_storage.bucket()
    for image_url in data.image_urls:
        blob = bucket.get_blob(image_url)
        image_data = blob.download_as_string()
        arr = np.frombuffer(image_data, np.uint8)
        # Cargar la imagen en memoria
        image = cv2.imdecode(arr, cv2.IMREAD_COLOR)
        image_pil = Image.fromarray(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
        image_pil.show()
        images.append(image_pil)
    
    # Genera contenido usando la API de Gemini con el prompt y las imágenes
    # Convertir las imágenes a un formato adecuado para la API de Gemini
    files = [image for image in images]
    if len(files) > 1:
        response = model.generate_content([data.prompt] + files)
    else:
        response = model.generate_content(files + [data.prompt])
    
    # Devuelve la respuesta de Gemini
    return jsonify({'text': response.text})

# Ruta para generar texto con Gemini
@app.route('/ai/generate-content', methods=['POST'])
def generate_content():
    try:
        # Valida el JSON de entrada usando Pydantic
        data = ContentRequest(**request.json)
    except ValidationError as e:
        return jsonify(e.errors()), 400
    
    # Genera contenido usando la API de Gemini
    response = model.generate_content(data.prompt)
    
    # Verifica si la respuesta de Gemini contiene el texto generado
    if hasattr(response, 'text'):
        return jsonify({'text': response.text}), 200
    else:
        return jsonify({'error': 'No se pudo generar contenido'}), 500
    
