from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*")

@app.route('/')
def web():
    return render_template('web.html')

@app.route('/update', methods=['POST'])
def update_data():
    data = request.json
    value = data.get('value') 
    print(f"Nhận từ ESP32 qua WiFi: {value}")

    socketio.emit('update_data', {'value': value})
    return jsonify({"status": "success"}), 200

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)
