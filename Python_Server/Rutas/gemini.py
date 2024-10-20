# Rutas/gemini.py
from datetime import datetime,timedelta# Importar la librería para trabajar con fechas
from PIL import Image # Importar la librería para trabajar con imágenes
from io import BytesIO # Importar la librería para trabajar con datos binarios
from flask import request, jsonify
import base64 # Importar la librería para trabajar con datos en base64
from pydantic import ValidationError # Importar la librería para validar los datos de entrada
from App.application import app, firebase_db, firebase_storage, get_first_level, load_dotenv # Importar la aplicación de Flask y la instancia de Firebase
import google.generativeai as genai # Importar la librería generativeai de Gemini
import os # Importar la librería para interactuar con el sistema operativo
from Models.models import ImageRequest, ContentRequest # Importar los modelos de Pydantic
import numpy as np # Importar la librería para trabajar con arreglos multidimensionales

# Configurar el modelo de Gemini
genai.configure(api_key= os.getenv("GEMINI_API_KEY"))
model = genai.GenerativeModel("gemini-1.5-flash")

# Ruta para analizar imágenes con un prompt
@app.route('/ai/analyze-images/custom', methods=['POST'])
def analyze_image_custom():
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
        image_bytes = BytesIO(arr)
        image_pil = Image.open(image_bytes)
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
    
# Ruta para analizar imágen por Base64 string
@app.route('/ai/analyze-images/base64', methods=['GET'])
def analyze_image_base64():
    image64_str = request.args.get('image64')

    if not image64_str:
        return jsonify({"error": "Se necesita una imagen en base64."}), 400

    # Verificar que la cadena sea un formato base64 válido
    try:
        decoded_image = base64.urlsafe_b64decode(image64_str + '=' * (4 - len(image64_str) % 4))
    except Exception as e:
        return jsonify({"error": "Formato de imagen no válido. msg: " + str(e)}), 400
    
    # Convertir la imagen a un objeto de imagen de PIL
    image_bytes = BytesIO(decoded_image)
    image_pil = Image.open(image_bytes)

    # Generar contenido usando la API de Gemini
    prompt = "Hola! En la imagen, como ves la calidad del agua? Si es azul es de buena calidad, nublada es calidad baja y cafe o otro color es peligrosa."
    response = model.generate_content([image_pil, prompt])
    
    return jsonify({'text': response.text}), 200


# Ruta para analizar imágenes por fecha
@app.route('/ai/analyze-images/date', methods=['GET'])
def analyze_images_date():
    email = request.args.get('email')
    range = request.args.get('range')
    start_date = request.args.get('start_date', datetime.now().strftime("%Y-%m-%d"))
    if not email:
        return jsonify({"error": "Se necesita un email."}), 400
    if not range:
        return jsonify({"error": "Se necesita un rango de fecha."}), 400
    
    # Convertir start_date a un objeto datetime
    current_date = datetime.strptime(start_date, "%Y-%m-%d")
    
    # Ajustar el rango según el tipo de rango proporcionado
    if range.lower() == "dia":
        start_date = current_date.replace(hour=0, minute=0, second=0, microsecond=0)
        end_date = start_date + timedelta(days=1) - timedelta(milliseconds=1)
    elif range.lower() == "3dias":
        start_date = current_date.replace(hour=0, minute=0, second=0, microsecond=0)
        end_date = start_date + timedelta(days=3) - timedelta(milliseconds=1)
    elif range.lower() == "semana":
        start_date = current_date - timedelta(days=current_date.weekday())
        start_date = start_date.replace(hour=0, minute=0, second=0, microsecond=0)
        end_date = start_date + timedelta(days=7) - timedelta(milliseconds=1)
    else:
        return jsonify({"error": "Rango no válido. Use 'Dia', '3Dias' o 'Semana'."}), 400
    
    # Obtener las URLs de las imágenes
    image_urls_by_unit = obtener_imageUrls(email, start_date, end_date)
    
    if not image_urls_by_unit:
        return jsonify({"error": "No se encontraron imágenes en el rango de fechas proporcionado"}), 404
    
    results_by_unit = {}

    prompt = "Hola! En las imagenes, como ves la calidad general del agua? Si es azul es de buena calidad, nublada es calidad baja y cafe o otro color es peligrosa."
    
    # Analizar las imágenes por unidad
    for unidad, image_urls in image_urls_by_unit.items():
        images = []
        bucket = firebase_storage.bucket()
        for image_url in image_urls:
            blob = bucket.get_blob(image_url)
            image_data = blob.download_as_string()
            arr = np.frombuffer(image_data, np.uint8)
            image_bytes = BytesIO(arr)
            image_pil = Image.open(image_bytes)
            images.append(image_pil)
        
        # Generar contenido usando la API de Gemini
        files = [image for image in images]
        if len(files) > 1:
            response = model.generate_content([prompt] + files)
        else:
            response = model.generate_content(files + [prompt])
        
        results_by_unit[unidad] = {'text': response.text}
        
        return jsonify({'unidades': results_by_unit})
    
def obtener_imageUrls(email, startDate, endDate):
    ref = firebase_db.reference(f'/usuarios/usuario/{email}/unidades')
    unidades = ref.get()
    
    if not unidades:
        return None
    
    image_urls_by_unit = {}
    
    for unidad in unidades.keys():
        ref_unidad = firebase_db.reference(f'/usuarios/usuario/{email}/unidades/{unidad}/sensores/calidad/fechas')
        fechas = ref_unidad.get()
        
        if fechas:
            for fecha, horas in fechas.items():
                fecha_dt = datetime.strptime(fecha, "%Y-%m-%d")
                if startDate <= fecha_dt <= endDate:
                    for hora, minutos in horas['horas'].items():
                        for minuto, imagen in minutos['minutos'].items():
                            url = imagen['imagen']
                            path = url.split('gs://ciberfisicos-i-402---equipo-5.appspot.com/')[-1]
                            if unidad not in image_urls_by_unit:
                                image_urls_by_unit[unidad] = []
                            image_urls_by_unit[unidad].append(path)
    
    return image_urls_by_unit
