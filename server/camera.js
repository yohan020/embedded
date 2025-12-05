document.addEventListener('DOMContentLoaded', () => {
    
    const cameraImg = document.getElementById("cameraStream");
    if (cameraImg) {
        const host = window.location.hostname;
        cameraImg.src = `http://${host}:8090/server/stream`;
    }

    const checkbox = document.getElementById("btn");
    const statusText = document.getElementById("recordStatusText");
    const displayHM = document.querySelector('.h_m');
    const displayMS = document.querySelector('.s_ms');

    let timerInterval = null;
    let startTime = 0;

    function sendCommend(cmd) {
        fetch('camera.jsp?action=' + cmd)
        .then(res=>res.text())
        .then(data => console.log("서버응답:" + data.trim()))
        .catch(err => console.error("통신 실패:", err));
    }

    if(checkbox) {
        checkbox.addEventListener('change', function() {
            if (this.checked) {
                console.log("녹화 시작");
                statusText.innerText = "REC";

                startTimer();
                sendCommend('rec_start');
            } else {
                console.log('녹화 중지');
                statusText.innerText = "Stop";

                stopTimer();
                sendCommend('rec_stop');
            }
        });
    }
  
    function startTimer() {
        startTime = Date.now();
        timerInterval = setInterval(() => {
            const now = Date.now();
            const diff = now - startTime;
            const min = Math.floor(diff / 60000);
            const sec = Math.floor((diff % 60000) / 1000);
            const ms = Math.floor((diff % 1000) / 10);
            displayHM.innerText = `${pad(min)}:${pad(sec)}`;
            displayMS.innerText = pad(ms);
        }, 10);
    }

    function stopTimer() {
        clearInterval(timerInterval);
        displayHM.innerText = "00:00";
        displayMS.innerText = "00";
    }

    function pad(n) {
        return n < 10? '0' + n : n;
    }
});