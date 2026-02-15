import cv2

# '0' is usually the built-in or first USB webcam
# If you have multiple cameras, try 1 or 2
cap = cv2.VideoCapture(0)

if not cap.isOpened():
    print("Error: Could not open the Logitech webcam.")
else:
    # Set resolution (Logitech cameras often support 1280x720)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

    # Capture a single frame
    ret, frame = cap.read()

    if ret:
        print(f"Success! Captured frame with shape: {frame.shape}")
        # Save it to the Pi so you can see it later
        cv2.imwrite('capture.jpg', frame)
    else:
        print("Error: Could not read frame.")

# Always release the hardware when done
cap.release()