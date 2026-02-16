import requests
import time

arduino_ip = "http://192.168.x.x" # Change to your IP

print("Triggering LED 0...")
requests.post(f"{arduino_ip}/LED=0")

print("Triggering LED 2 immediately after...")
# This should switch the light to LED 2 and reset the 3s timer
requests.post(f"{arduino_ip}/LED=2")

print("Test complete. Check if LED 2 stays on for 3 seconds.")