# rutas/crud.py
from flask import request, jsonify # Importar las librerías necesarias de Flask
from App.application import app, db, get_first_level # Importar la aplicación de Flask y la instancia de Firebase

# Ruta para crear un nuevo recurso
@app.route('/<path:path>', methods=['POST'])
def create(path):
    data = request.json
    ref = db.reference(f'/{path}')
    ref.push(data)
    return jsonify(data), 201

# Ruta para obtener uno o todos los recursos
@app.route('/<path:path>', methods=['GET'])
def read(path):
    ref = db.reference(f'/{path}')
    instances = ref.get()    
    if not instances:
        return jsonify({"error": "Recurso no encontrado"}), 404
    # Obtener solo el primer elemento en cada nivel
    filtered_instances = get_first_level(instances)    
    return jsonify(filtered_instances)

# Ruta para actualizar un recurso por ID
@app.route('/<path:path>', methods=['PUT'])
def update(path):
    data = request.json
    ref = db.reference(f'/{path}')
    instance = ref.get()
    if not instance:
        return jsonify({"error": "Recurso no encontrado"}), 404
    ref.update(data)
    return jsonify(data)

# Ruta para eliminar un recurso por ID
@app.route('/<path:path>', methods=['DELETE'])
def delete(path):
    ref = db.reference(f'/{path}')
    instance = ref.get()
    if not instance:
        return jsonify({"error": "Recurso no encontrado"}), 404
    ref.delete()
    return jsonify({"message": "Recurso eliminado"}), 200