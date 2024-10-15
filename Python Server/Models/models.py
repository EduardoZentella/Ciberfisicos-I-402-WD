# models/Models.py
from pydantic import BaseModel, Field, RootModel
from typing import Dict, Any

class Flujo(BaseModel):
    Flujo: float

class Segundos(RootModel):
    root: Dict[str, Flujo]

class Minutos(BaseModel):
    segundos: Dict[str, Segundos]

class Horas(BaseModel):
    minutos: Dict[str, Minutos]

class Fechas(BaseModel):
    horas: Dict[str, Horas]

class Sensores(BaseModel):
    flujo: Fechas
    calidad: Fechas

class Unidad(BaseModel):
    sensores: Sensores

class Unidades(BaseModel):
    unidad: Unidad

class Usuario(BaseModel):
    contraseña: str
    nombre: str
    unidades: Unidades
    
class Admin(BaseModel):
    contraseña: str
    nombre: str

class Usuarios(BaseModel):
    usuario: Dict[str, Usuario]
    admin: Dict[str, Admin]

class Root(BaseModel):
    usuarios: Usuarios

class FirebaseConfig(BaseModel):
    type: str
    project_id: str
    private_key_id: str
    private_key: str
    client_email: str
    client_id: str
    auth_uri: str
    token_uri: str
    auth_provider_x509_cert_url: str
    client_x509_cert_url: str
