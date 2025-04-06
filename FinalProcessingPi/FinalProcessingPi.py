import paho.mqtt.client as mqtt #MQTT client library for connecting, subscribing, and publishing.
import json
import ssl
from collections import defaultdict #Provides dictionary with default value support.
import numpy as np
from scipy.optimize import least_squares  #For solving non-linear least squares problems.
from collections import deque   #Creates a fixed-length queue.
from collections import Counter #Helps count hashable items.

# ----- MQTT Settings -----
# Subscriber broker (local Pi)
SUB_BROKER = "localhost"
SUB_PORT = 1883

# Publisher broker (Home Assistant Pi)
HA_BROKER = "192.168.1.115"
HA_PORT = 8883 #MQTT port for TLS connection.
HA_USER = "zionjiam"
HA_PASS = "98323646abc"
# Topics for publishing calculated positions.
PUBLISH_TOPIC_PLAYER1 = "m5stick/player1position"
PUBLISH_TOPIC_PLAYER2 = "m5stick/player2position"
PUBLISH_TOPIC_PLAYER1_XYZ = "m5stick/player1xyz"
PUBLISH_TOPIC_PLAYER2_XYZ = "m5stick/player2xyz"

# Mapping corner to upper/lower server IDs
# Defines which two servers belong to each room corner.
corner_servers = {
    "A": ("Server-A1", "Server-A2"),
    "B": ("Server-B1", "Server-B2"),
    "C": ("Server-C1", "Server-C2"),
    "D": ("Server-D1", "Server-D2")
}

# Server positions in meters
# Provides XYZ coordinates for each server.
server_coords = {
    "Server-A1": [0.00, 0.00, 3.40],
    "Server-A2": [0.00, 0.00, 0.00],
    "Server-B1": [3.40, 0.00, 3.40],
    "Server-B2": [3.40, 0.00, 0.00],
    "Server-C1": [0.00, 3.85, 3.40],
    "Server-C2": [0.00, 3.85, 0.00],
    "Server-D1": [3.40, 3.85, 3.40],
    "Server-D2": [3.40, 3.85, 0.00],
}

# Room dimensions
ROOM_WIDTH = 3.40   # x-axis (A↔B or C↔D)
ROOM_DEPTH = 3.85   # y-axis (A↔C or B↔D)
ROOM_HEIGHT = 3.40  # z-axis

# Store latest values from each server
latest_data = {}

# Buffer last 20 area readings
area_history = {
    "player1": deque(maxlen=20),
    "player2": deque(maxlen=20)
}

# Store last known area for each player
last_known_areas = {
    "player1": None,
    "player2": None
}

# Buffer for position history (for smoothing).
xyz_history = {
    "player1": deque(maxlen=5),
    "player2": deque(maxlen=5)
}

# --- Smoothing Function ---
def smooth_xyz(player_key, new_xyz):
    history = xyz_history[player_key]
    history.append(new_xyz)
    avg = np.mean(history, axis=0)
    return avg.tolist()

# --- Convert RSSI to Distance ---
tx_power = -59
n = 2.2
def rssi_to_distance(rssi):
    # Convert received signal strength (RSSI) to a distance using the log-distance model.
    rssi = max(rssi, -30)  # cap at -30 dBm (super strong signals)
    return 10 ** ((tx_power - rssi) / (10 * n))

def trilateration_error(pos, positions, distances):
    return [np.linalg.norm(pos - p) - d for p, d in zip(positions, distances)]

# --- Estimate 3D Position via Trilateration ---
def estimate_xyz_position(player_key):
    coords, dists = [], [] 
    for sid, data in latest_data.items():
         # Process each server's data
        for sid, data in latest_data.items():
            rssi = data.get(player_key, -999)
            if sid in server_coords and rssi > -80:
                coords.append(server_coords[sid])
                dists.append(rssi_to_distance(rssi))
    if len(coords) < 4:
        return None  # Not enough data
    coords = np.array(coords)
    dists = np.array(dists)
    initial_guess = np.array([ROOM_WIDTH / 2, ROOM_DEPTH / 2, ROOM_HEIGHT / 2])
    result = least_squares(trilateration_error, initial_guess, args=(coords, dists))
    # Clamp the result within room boundaries.
    x, y, z = result.x
    return [
        max(0, min(ROOM_WIDTH, x)),
        max(0, min(ROOM_DEPTH, y)),
        max(0, min(ROOM_HEIGHT, z))
    ]

# --- Determine Area (Corner) from 2D Position ---
def get_area_from_position(x, y):
    # Compute the nearest corner using Euclidean distance.
    corner_centers = {
        "Corner A": [0.00, 0.00],
        "Corner B": [ROOM_WIDTH, 0.00],
        "Corner C": [0.00, ROOM_DEPTH],
        "Corner D": [ROOM_WIDTH, ROOM_DEPTH],
    }

    closest_corner = min(
        corner_centers.items(),
        key=lambda item: (x - item[1][0])**2 + (y - item[1][1])**2
    )[0]

    return closest_corner

# --- Determine the Strongest (Nearest) Area Based on RSSI ---
def get_strongest_area(player_key):
    best_rssi = -999
    best_corner = "Unknown"

    # Check each corner's upper and lower server, choose the one with the strongest signal.
    for corner, (upper_id, lower_id) in corner_servers.items():
        rssi_upper = latest_data.get(upper_id, {}).get(player_key, -999)
        rssi_lower = latest_data.get(lower_id, {}).get(player_key, -999)

        strongest = max(rssi_upper, rssi_lower)
        if strongest > best_rssi:
            best_rssi = strongest
            best_corner = f"Corner {corner}"

    return best_corner

# --- MQTT Callback Functions ---
def on_connect(client, userdata, flags, rc):
    print("Connected with result code", rc)
    client.subscribe("m5stick/data")

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        server_id = payload["server_id"]
        latest_data[server_id] = payload # Store the latest data from this server.

        if len(latest_data) >= 8:
            p1_xyz = smooth_xyz("player1", estimate_xyz_position("player1_rssi"))
            p2_xyz = smooth_xyz("player2", estimate_xyz_position("player2_rssi"))
            p1_area = get_strongest_area("player1_rssi")
            p2_area = get_strongest_area("player2_rssi")

            print("\n-----------------------------")
            print(f"Player1 is nearest to: {p1_area}")
            if p1_xyz:
                print(f"Player1 estimated position (x,y,z): {p1_xyz}")
                xyz_payload_p1 = {
                    "x": p1_xyz[0],
                    "y": p1_xyz[1],
                    "z": p1_xyz[2]
                }
                client_ha.publish(PUBLISH_TOPIC_PLAYER1_XYZ, json.dumps(xyz_payload_p1))

            print(f"\nPlayer2 is nearest to: {p2_area}")
            if p2_xyz:
                print(f"Player2 estimated position (x,y,z): {p2_xyz}")
                xyz_payload_p2 = {
                    "x": p2_xyz[0],
                    "y": p2_xyz[1],
                    "z": p2_xyz[2]
                }
                client_ha.publish(PUBLISH_TOPIC_PLAYER2_XYZ, json.dumps(xyz_payload_p2))

            # Create payloads for position.
            payload_p1 = {"x": p1_xyz[0], "y": p1_xyz[1], "z": p1_xyz[2]}
            payload_p2 = {"x": p2_xyz[0], "y": p2_xyz[1], "z": p2_xyz[2]}

            # Update area history buffer
            area_history["player1"].append(p1_area)
            area_history["player2"].append(p2_area)

            # Only act when we have full 20 readings
            if len(area_history["player1"]) == 20:
                most_common_p1 = Counter(area_history["player1"]).most_common(1)[0][0]
                if most_common_p1 != last_known_areas["player1"]:
                    last_known_areas["player1"] = most_common_p1
                    payload_p1["area"] = most_common_p1
                    client_ha.publish(PUBLISH_TOPIC_PLAYER1, json.dumps(payload_p1))

            if len(area_history["player2"]) == 20:
                most_common_p2 = Counter(area_history["player2"]).most_common(1)[0][0]
                if most_common_p2 != last_known_areas["player2"]:
                    last_known_areas["player2"] = most_common_p2
                    payload_p2["area"] = most_common_p2
                    client_ha.publish(PUBLISH_TOPIC_PLAYER2, json.dumps(payload_p2))

            # Alternative: publish immediately
            # client_ha.publish(PUBLISH_TOPIC_PLAYER1, json.dumps(payload_p1))
            # client_ha.publish(PUBLISH_TOPIC_PLAYER2, json.dumps(payload_p2))
    except Exception as e:
        print("Error parsing message:", e)

# --- MQTT Client Setup ---
# 1. Local MQTT client (subscriber)
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(SUB_BROKER, SUB_PORT, 60)

# 2. Home Assistant MQTT client (publisher with TLS).
client_ha = mqtt.Client()
client_ha.username_pw_set(HA_USER, HA_PASS)
client_ha.tls_set(cert_reqs=ssl.CERT_NONE)
client_ha.tls_insecure_set(True)
client_ha.connect(HA_BROKER, HA_PORT, 60)
client_ha.loop_start()

client.connect("localhost", 1883, 60)
client.loop_forever()

# Sets up two MQTT clients - one for subscribing to local data and one for publishing to Home Assistant.
