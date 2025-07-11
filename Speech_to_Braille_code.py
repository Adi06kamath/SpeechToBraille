import speech_recognition as sr
import serial
import time

# === UART Setup ===
try:
    ser = serial.Serial('COM7', 9600, timeout=1)  # Change 'COM7' if needed
    time.sleep(1)  # Wait for the serial connection to stabilize
    print("✅ Connected to STM32 on COM7.")
except Exception as e:
    print(f"❌ Could not open serial port: {e}")
    exit()

# === Speech Recognition Setup ===
recognizer = sr.Recognizer()

with sr.Microphone() as source:
    print("\n🎙️ Speak now:")
    recognizer.adjust_for_ambient_noise(source)

    try:
        audio = recognizer.listen(source, timeout=2, phrase_time_limit=3)
    except sr.WaitTimeoutError:
        print("❌ Timeout: No speech detected.")
        ser.close()
        exit()

try:
    # Convert speech to text
    text = recognizer.recognize_google(audio)
    print(f"✅ You said: '{text}'")

    # Send recognized text to STM32
    ser.write((text + '\n').encode('utf-8'))
    ser.flush()
    print("📤 Sent to STM32 successfully.")

except sr.UnknownValueError:
    print("❌ Could not understand the audio.")
except sr.RequestError as e:
    print(f"❌ Could not request results from Google Speech Recognition service; {e}")
except Exception as e:
    print(f"❌ Unexpected error: {e}")
finally:
    ser.close()
