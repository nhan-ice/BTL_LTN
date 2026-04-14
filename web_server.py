from flask import Flask, render_template, request
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

@app.route('/')
def index():
    return render_template('web.html')

@app.route('/update', methods=['POST'])
def update():
    try:
        data = request.get_json(force=True)
        print(f"--- NHẬN DỮ LIỆU: {data} ---")
        
        socketio.emit('update_data', data)
        return "OK"
    except Exception as e:
        print("Lỗi định dạng dữ liệu:", e)
        return "Format Error", 400

if __name__ == '__main__':
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)