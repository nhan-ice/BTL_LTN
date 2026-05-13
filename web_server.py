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
        # force=True giúp ép kiểu dữ liệu về JSON kể cả khi Header Content-Type không chuẩn
        data = request.get_json(force=True)
        print(f"\n--- [LOG] NHẬN DỮ LIỆU TỪ ESP32 ---")
        print(data)
        
        # PHÂN LOẠI DỮ LIỆU ĐỂ BẮN LÊN GIAO DIỆN WEB (REAL-TIME)
        
        # 1. Trường hợp là dữ liệu DỰ ĐOÁN TƯƠNG LAI (Regression)
        # ESP32 cần gửi lên key là 'Future_Predict'
        if 'Future_Predict' in data:
            val = data['Future_Predict']
            print(f"[AI REGRESSION] Giá trị dự báo tương lai: {val}")
            # Gửi sang kênh 'update_ai' để web cập nhật ô hiển thị dự báo
            socketio.emit('update_ai', {'val': val})
            
        # 2. Trường hợp là dữ liệu cảm biến thời gian thực (Biểu đồ)
        elif 'Rel_IAQ' in data:
            print(f"[SENSOR] Đang cập nhật dữ liệu biểu đồ...")
            socketio.emit('update_sensor', data)
            
        # 3. Các trường hợp dữ liệu khác
        else:
            socketio.emit('update_data', data)

        return "OK", 200
        
    except Exception as e:
        print("Lỗi xử lý dữ liệu:", e)
        return "Internal Server Error", 500

if __name__ == '__main__':
    # Chạy trên toàn bộ mạng nội bộ tại cổng 5000
    socketio.run(app, host='0.0.0.0', port=5000, debug=True)