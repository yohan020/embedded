Import io 
from flask import Flask, render_template, Response
from picamera2 import Picamera2

app = Flask(__name__) 

picam2 = Picamera2()
config = picam2.create_video_configuration(main={"size": (800, 600)})
picam2.configure(config)
picam2.start()

def gen_frames():
    while True:
        request = picam2.capture_array()
        
        stream = io.BytesIO()
        picam2.capture_file(stream, format='jpeg')
        frame = stream.getvalue()
        
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(gen_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080
