from flask import Flask, render_template, request
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

@app.route('/')
def index():
    return render_template('web.html')

@app.route('/update', methods=['POST'])
def update():
    val = request.data.decode('utf-8').strip()
    print(f"--- NHẬN QUA HTTP: {val} ---")
    socketio.emit('update_data', {'value': val})

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=False, allow_unsafe_werkzeug=True)