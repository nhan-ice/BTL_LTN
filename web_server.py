from flask import Flask, render_template, request
from flask_socketio import SocketIO

app = Flask(__name__)
# Dùng cấu hình này để chấp nhận cả ESP32 và trình duyệt web
socketio = SocketIO(app, cors_allowed_origins="*", async_mode='threading')

@app.route('/')
def index():
    return render_template('web.html')

@app.route('/update', methods=['POST'])
def update():
    # Nhận dữ liệu thô gửi từ ESP32
    val = request.data.decode('utf-8').strip()
    print(f"--- NHẬN QUA HTTP: {val} ---")
    # Vẫn dùng SocketIO để đẩy lên Web như cũ để vẽ đồ thị
    socketio.emit('update_data', {'value': val})

if __name__ == '__main__':
    # allow_unsafe_werkzeug=True là bắt buộc trên Windows
    socketio.run(app, host='0.0.0.0', port=5000, debug=False, allow_unsafe_werkzeug=True)