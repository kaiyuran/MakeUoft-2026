from enum import Enum
from typing import MutableSequence
# import cv2
# import time
# import google.generativeai as genai
# import PIL.Image
# import pyttsx3
# import pygame
# import os
# import threading
# import speech_recognition as sr
import json
# import re
from dataclasses import dataclass

class Location(Enum):
    BACK = "Back"
    MIDDLE = "Middle"
    FRONT = "Front"
    LEFT = "Left"

# Give the exception a message so your AI can "read" what went wrong
class MissingItemException(Exception):
    def __init__(self, name):
        super().__init__(f"Item '{name}' not found in the inventory registry.")

@dataclass
class Item:
    """Using a dataclass automatically handles __init__ and makes printing easier."""
    amount: int
    location: Location

class ItemRegistry:
    def __init__(self):
        # Type hinting the dict for better IDE support
        self.registry: dict[str, Item] = {}
        
    def add_item(self, name: str, amount: int, location: Location):
        """Adds an item or increments the count if it exists."""
        if name in self.registry:
            self.registry[name].amount += amount
        else:
            self.registry[name] = Item(amount, location)
            
    def remove_item(self, name: str):
        """Decrements amount or removes item. Raises custom exception if missing."""
        if name not in self.registry:
            raise MissingItemException(name)
            
        if self.registry[name].amount > 1:
            self.registry[name].amount -= 1
        else:
            del self.registry[name] # 'del' is slightly cleaner than 'pop' if not using the value

    def __str__(self):
        """Allows you to print(registry) to see your whole inventory at once."""
        if not self.registry:
            return "Inventory is empty."
        return "\n".join([f"{n}: {i.amount} at {i.location.value}" for n, i in self.registry.items()])
            
    def save_to_json(self, filename: str):
        """Converts the registry to a serializable format and writes to a file."""
        serializable_data = {}
        
        for name, item in self.registry.items():
            serializable_data[name] = {
                "amount": item.amount,
                "location": item.location.value  # We must use .value to get the string "Back", "Front", etc.
            }
        
        with open(filename, 'w') as f:
            json.dump(serializable_data, f, indent=4)
        print(f"✅ Registry successfully saved to {filename}")

# Setup
pouch = ItemRegistry()
pouch.add_item("Medkit", 2, Location.FRONT)
pouch.add_item("Battery", 5, Location.MIDDLE)

# Save it
pouch.save_to_json("inventory.json")

# # --- CONFIGURATION ---
# # SECURITY UPGRADE: Try to fetch key from the Computer's Environment Variables first
# API_KEY = os.environ.get("AIzaSyC6Bcs8R1SAFVVLIXomgP3KVbZNgFpWnoU")

# CAMERA_INDEX = 1 # Try 0 or 1
# SOUND_FILE = "processing.wav" 

# # --- INITIALIZATION ---
# genai.configure(api_key=API_KEY)

# # *** DYNAMIC MODEL SELECTION ***
# print("\n[SYSTEM] Connecting to Google Neural Net...")
# chosen_model_name = None
# valid_models = []

# try:
#     for m in genai.list_models():
#         if 'generateContent' in m.supported_generation_methods:
#             valid_models.append(m.name)
    
#     priority_order = [
#         "models/gemini-1.5-flash",
#         "models/gemini-1.5-flash-latest",
#         "models/gemini-1.5-flash-001",
#     ]
    
#     for priority in priority_order:
#         if priority in valid_models:
#             chosen_model_name = priority
#             break
            
#     if not chosen_model_name and valid_models:
#         chosen_model_name = valid_models[0]
        
#     if chosen_model_name:
#         print(f"[SYSTEM] Neural Net Linked: {chosen_model_name}")
#         model = genai.GenerativeModel(chosen_model_name)
#     else:
#         print("[CRITICAL ERROR] No valid AI models found.")
#         model = None

# except Exception as e:
#     print(f"[ERROR] API Initialization failed: {e}")
#     model = None

# # Audio Setup
# try:
#     engine = pyttsx3.init()
#     engine.setProperty('rate', 160)
#     engine.setProperty('volume', 1.0)
# except Exception as e:
#     print(f"[WARNING] TTS Engine failed: {e}")
#     engine = None

# recognizer = sr.Recognizer()
# pygame.mixer.init()

# # --- DATABASE CLASS (The Logic Engine) ---
# class SentinelDB:
#     def __init__(self):
#         # Structure: { "Front": { "items": {"Chips": 150}, "missing": [] } }
#         self.pockets = {
#             "Front":  {"items": {}, "missing": []},
#             "Middle": {"items": {}, "missing": []},
#             "Back":   {"items": {}, "missing": []}
#         }

#     def get_context_str(self, pocket):
#         """Returns a string summary of what should be in the pocket"""
#         current_inv = list(self.pockets[pocket]["items"].keys())
#         return ', '.join(current_inv) if current_inv else "None (Empty)"

#     def clear_pocket(self, pocket):
#         self.pockets[pocket] = {"items": {}, "missing": []}

#     def get_calories(self, pocket):
#         """Calculates total calories in a specific pocket"""
#         total = 0
#         for label, kcal in self.pockets[pocket]["items"].items():
#             if label not in self.pockets[pocket]["missing"]:
#                 total += kcal
#         return total

#     def process_scan(self, pocket, visible_list):
#         current_data = self.pockets[pocket]["items"]
        
#         # 1. SCAN IN (Update/Add items)
#         current_labels = []
#         for item in visible_list:
#             # Data Cleaning: Normalize label to Title Case (e.g. "chips" -> "Chips")
#             label = item['label'].strip().title()
#             kcal = item.get('calories', 0)
            
#             current_labels.append(label)
#             current_data[label] = kcal

#         # 2. SCAN OUT (Detect Missing)
#         missing_now = []
#         for label in current_data.keys():
#             if label not in current_labels:
#                 missing_now.append(label)
        
#         self.pockets[pocket]["missing"] = missing_now
#         return missing_now

# db = SentinelDB()
# current_pocket = "Middle"

# def play_sound():
#     if os.path.exists(SOUND_FILE):
#         try:
#             pygame.mixer.music.load(SOUND_FILE)
#             pygame.mixer.music.play()
#         except: pass

# def speak(text):
#     print(f"\n[SGT. SAC]: {text}")
#     if engine:
#         try:
#             engine.say(text)
#             engine.runAndWait() 
#         except Exception as e:
#             print(f"[Audio Error]: {e}")

# def listen_to_user():
#     try:
#         with sr.Microphone() as source:
#             print("[SYSTEM] Listening...")
#             try:
#                 audio = recognizer.listen(source, timeout=3, phrase_time_limit=4)
#                 text = recognizer.recognize_google(audio)
#                 print(f"[USER]: {text}")
#                 return text
#             except sr.WaitTimeoutError:
#                 return ""
#             except sr.UnknownValueError:
#                 return ""
#             except Exception as e:
#                 print(f"Recog Error: {e}")
#                 return ""
#     except Exception as e:
#         print(f"Microphone Error (Is PyAudio installed?): {e}")
#         return ""

# def get_gemini_analysis(frame, pocket_name, database, user_query=""):
#     if not model: return None

#     color_converted = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
#     pil_image = PIL.Image.fromarray(color_converted)
    
#     inv_str = database.get_context_str(pocket_name)
#     query_context = f"User Command: '{user_query}'." if user_query else ""
    
#     # 1. DEFINE STRICT JSON SCHEMA
#     response_schema = {
#         "type": "OBJECT",
#         "properties": {
#             "visible_items": {
#                 "type": "ARRAY",
#                 "items": {
#                     "type": "OBJECT",
#                     "properties": {
#                         "label": {"type": "STRING"},
#                         "calories": {"type": "INTEGER"}
#                     }
#                 }
#             },
#             "speech": {"type": "STRING"}
#         },
#         "required": ["visible_items", "speech"]
#     }

#     # 2. SIMPLIFIED PROMPT
#     prompt = f"""
#     You are SGT. SAC (Sergeant Sac), a tactical backpack AI.
#     Current Pocket: {pocket_name}
#     Known Inventory: [{inv_str}]
#     {query_context}

#     TASK:
#     1. Identify all visible items. 
#     2. USE GENERIC, SHORT LABELS (e.g. 'Chips', 'Water Bottle', 'Phone').
#     3. Estimate calories for food (0 for non-food).
#     4. Generate speech. If items are missing/added, mention them. Keep it tactical.
#     """
    
#     try:
#         # 3. CALL API WITH CONFIG
#         response = model.generate_content(
#             [prompt, pil_image],
#             generation_config=genai.GenerationConfig(
#                 response_mime_type="application/json", 
#                 response_schema=response_schema
#             )
#         )
#         return json.loads(response.text)
#     except Exception as e:
#         print(f"AI Error: {e}")
#         return None

# def draw_hud(frame):
#     h, w, _ = frame.shape
    
#     sidebar_w = 250
#     overlay = frame.copy()
#     cv2.rectangle(overlay, (w - sidebar_w, 0), (w, h), (0, 0, 0), -1)
#     cv2.addWeighted(overlay, 0.6, frame, 0.4, 0, frame)
    
#     cv2.putText(frame, "SGT. SAC OS", (20, 40), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2)
#     cv2.putText(frame, f"ZONE: {current_pocket.upper()}", (20, 80), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 0), 2)

#     x_start = w - sidebar_w + 10
#     y_start = 40
    
#     cv2.putText(frame, "ASSET MANIFEST:", (x_start, y_start), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
#     y_start += 30
    
#     pocket_data = db.pockets[current_pocket]
#     items = pocket_data["items"]
#     missing = pocket_data["missing"]
    
#     # Draw Present Items
#     for label, kcal in items.items():
#         if label not in missing:
#             text = f"[+] {label}"
#             if kcal > 0: text += f" ({kcal}k)"
#             cv2.putText(frame, text, (x_start, y_start), cv2.FONT_HERSHEY_PLAIN, 1.2, (0, 255, 0), 1)
#             y_start += 25
            
#     # Draw Missing Items
#     for label in missing:
#         cv2.putText(frame, f"[-] {label} (MISSING)", (x_start, y_start), cv2.FONT_HERSHEY_PLAIN, 1.2, (0, 0, 255), 1)
#         y_start += 25

#     total_cals = db.get_calories(current_pocket)
#     if total_cals > 0:
#         cv2.putText(frame, f"RATIONS: {total_cals} KCAL", (x_start, h - 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 255), 1)

#     cv2.putText(frame, "[SPACE] SCAN | [V] VOICE | [R] RESET | [1-3] POCKET", (20, h - 30), cv2.FONT_HERSHEY_PLAIN, 1.0, (200, 200, 200), 1)

# # --- MAIN LOOP ---
# if __name__ == "__main__":
#     print("\n--- SGT. SAC ONLINE ---")
#     speak("Sergeant Sac on duty.")

#     # Try to open camera safely
#     cap = cv2.VideoCapture(CAMERA_INDEX, cv2.CAP_DSHOW)
#     if not cap.isOpened(): 
#         print(f"[WARNING] Camera {CAMERA_INDEX} failed. Trying index 0...")
#         cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)

#     if not cap.isOpened():
#         print("[CRITICAL] No cameras found.")
#     else:
#         while True:
#             ret, frame = cap.read()
#             if not ret: break

#             draw_hud(frame)
#             cv2.imshow("SGT. SAC HUD", frame)

#             key = cv2.waitKey(1) & 0xFF

#             if key == ord('q'): break
#             elif key == ord('1'): current_pocket = "Front"; play_sound()
#             elif key == ord('2'): current_pocket = "Middle"; play_sound()
#             elif key == ord('3'): current_pocket = "Back"; play_sound()
            
#             elif key == ord('r'): 
#                 db.clear_pocket(current_pocket)
#                 speak(f"{current_pocket} manifest cleared.")

#             elif key == 32 or key == ord('v'): # SPACE or V
#                 is_voice = (key == ord('v'))
#                 play_sound()
                
#                 cv2.putText(frame, "SCANNING SECTOR...", (200, 240), cv2.FONT_HERSHEY_SIMPLEX, 1.5, (0, 255, 255), 3)
#                 cv2.imshow("SGT. SAC HUD", frame)
#                 cv2.waitKey(1)

#                 user_query = ""
#                 if is_voice:
#                     user_query = listen_to_user()
#                     if not user_query: 
#                         speak("Comms unclear. Repeat.")
#                         continue

#                 result = get_gemini_analysis(frame, current_pocket, db, user_query)

#                 if result:
#                     visible_list = result.get("visible_items", [])
#                     speech_text = result.get("speech", "Scan complete.")
                    
#                     db.process_scan(current_pocket, visible_list)
                    
#                     speak(speech_text)
#                 else:
#                     speak("Visual sensors compromised. Rescan.")

#         cap.release()
#         cv2.destroyAllWindows()
    