# app/app.py
import firebase_admin
import os
from firebase_admin import credentials, db, storage
from dotenv import load_dotenv
from flask import Flask
from Models.models import FirebaseConfig

# Cargar variables de entorno desde el archivo .env
load_dotenv()

# Crear una instancia del modelo de configuración con las variables de entorno
firebase_config = FirebaseConfig(
    type="service_account",
    project_id=os.getenv("PROJECT_ID"),
    private_key_id=os.getenv("PRIVATE_KEY_ID"),
    private_key=os.getenv("PRIVATE_KEY").replace('\\n', '\n'),
    client_email=os.getenv("CLIENT_EMAIL"),
    client_id=os.getenv("CLIENT_ID"),
    auth_uri="https://accounts.google.com/o/oauth2/auth",
    token_uri="https://oauth2.googleapis.com/token",
    auth_provider_x509_cert_url="https://www.googleapis.com/oauth2/v1/certs",
    client_x509_cert_url=os.getenv("CLIENT_X509_CERT_URL")
)

# Initialize Flask app
app = Flask(__name__)

# Inicializar Firebase solo si no está ya inicializado
if not firebase_admin._apps:
    cred = credentials.Certificate(firebase_config.model_dump())
    firebase_admin.initialize_app(cred, {
        'databaseURL': os.getenv("DATABASE_URL"),
        'storageBucket': os.getenv("STORAGE_URL")
    })
    firebase_db = db
    firebase_storage = storage 

# Función para obtener solo el primer nivel de un diccionario
def get_first_level(data):
    if isinstance(data, dict):
        return {k: {} if isinstance(v, dict) else v for k, v in data.items()}
    return data
