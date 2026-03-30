"""
server.py — Railway Track Health Monitoring Server
Author : Sameeksha R
 
Receives HTTP POST alerts from TI CC3200 over Wi-Fi.
Logs all events to a CSV file and prints real-time console alerts.
Optionally forwards critical events to an email/SMS webhook.
 
Run:
    pip install flask pandas
    python server.py
 
The CC3200 posts JSON to: http://<server_ip>:8080/alert
"""
 
from flask import Flask, request, jsonify
import csv
import os
import json
from datetime import datetime
 
app = Flask(__name__)
 
LOG_FILE    = "received_alerts.csv"
CRITICAL_G  = 3.0     # g threshold for critical vibration alert
CRITICAL_V  = 150.0   # km/h threshold for critical speed alert
 
# ── Initialise CSV log ────────────────────────────────────────────────
def init_log():
    if not os.path.exists(LOG_FILE):
        with open(LOG_FILE, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow([
                "server_time", "device_ts_ms", "speed_kmph",
                "accel_rms_g", "piezo_adc", "latitude",
                "longitude", "event"
            ])
 
# ── Alert endpoint ────────────────────────────────────────────────────
@app.route("/alert", methods=["POST"])
def receive_alert():
    try:
        data = request.get_json(force=True)
    except Exception:
        return jsonify({"status": "error", "msg": "invalid JSON"}), 400
 
    required = ["timestamp_ms", "speed_kmph", "accel_rms_g",
                "piezo_adc", "latitude", "longitude", "event"]
    if not all(k in data for k in required):
        return jsonify({"status": "error", "msg": "missing fields"}), 400
 
    server_time = datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")
 
    # Console output
    severity = "🔴 CRITICAL" if (
        data["speed_kmph"] > CRITICAL_V or data["accel_rms_g"] > CRITICAL_G
    ) else "🟡 WARNING"
 
    print(f"\n{'='*60}")
    print(f"  {severity}  |  {server_time}")
    print(f"  Event     : {data['event']}")
    print(f"  Speed     : {data['speed_kmph']:.2f} km/h")
    print(f"  Accel RMS : {data['accel_rms_g']:.4f} g")
    print(f"  Piezo ADC : {data['piezo_adc']}")
    print(f"  Location  : {data['latitude']:.6f}, {data['longitude']:.6f}")
    print(f"  Device ts : {data['timestamp_ms']} ms")
    print(f"{'='*60}")
 
    # Write to CSV
    with open(LOG_FILE, 'a', newline='') as f:
        writer = csv.writer(f)
        writer.writerow([
            server_time,
            data["timestamp_ms"],
            data["speed_kmph"],
            data["accel_rms_g"],
            data["piezo_adc"],
            data["latitude"],
            data["longitude"],
            data["event"]
        ])
 
    return jsonify({"status": "ok", "received": data["event"]}), 200
 
# ── Health check ──────────────────────────────────────────────────────
@app.route("/status", methods=["GET"])
def status():
    return jsonify({
        "status": "running",
        "log_file": LOG_FILE,
        "server_time": datetime.now().isoformat()
    }), 200
 
# ── Main ──────────────────────────────────────────────────────────────
if __name__ == "__main__":
    init_log()
    print("=== Railway Monitor Server — Sameeksha R ===")
    print(f"Listening on port 8080 — log: {LOG_FILE}\n")
    app.run(host="0.0.0.0", port=8080, debug=False)
