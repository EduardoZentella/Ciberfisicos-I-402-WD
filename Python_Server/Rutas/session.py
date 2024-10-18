# Rutas/login.py
from flask import request, jsonify
from App.application import app, firebase_db, firebase_storage, get_first_level
from pydantic import ValidationError
from Models.models import Usuarios


@app.route('/login', methods=['GET'])
def login():
    email = request.args.get('email')
    password = request.args.get('contraseña')
    ref = firebase_db.reference('/usuarios/usuario')
    user = ref.child(email).get()
    if not user:
        return jsonify({"error": "Usuario no encontrado"}), 404
    if user['contraseña'] != password:
        return jsonify({"error": "Contraseña incorrecta"}), 401
    return jsonify({"user": get_first_level(user)}), 200

@app.route('/register', methods=['POST'])
def register():
    data = request.json
    try:
        usuario = Usuarios.Usuario(**data)
    except ValidationError as e:
        return jsonify({"error": str(e)}), 400

    ref = firebase_db.reference('/usuarios/usuario')
    ref.child(data.get('email')).set(usuario.dict())
    return jsonify(usuario.dict()), 201