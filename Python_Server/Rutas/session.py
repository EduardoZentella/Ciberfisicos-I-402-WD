# Rutas/login.py
from flask import request, jsonify
from App.application import app, firebase_db, firebase_storage, get_first_level
from pydantic import ValidationError


@app.route('/login', methods=['GET'])
def login():
    data = request.json
    ref = firebase_db.reference('/usuarios/usuario')
    user = ref.child(data.get('email')).get()
    if not user:
        return jsonify({"error": "Usuario no encontrado"}), 404
    if user['contraseña'] != data.get('password'):
        return jsonify({"error": "Contraseña incorrecta"}), 401
    return jsonify({"user": get_first_level(user)}), 200
