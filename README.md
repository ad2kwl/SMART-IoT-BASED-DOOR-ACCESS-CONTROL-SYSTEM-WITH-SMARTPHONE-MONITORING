# Smart IoT-Based Door Access Control System with Smartphone Monitoring

A smart door lock system built around the ESP32 microcontroller, a 4x4 keypad, a buzzer, and the Blynk mobile platform for remote monitoring and notifications. The system allows multiple users to unlock the door using assigned password codes, tracks the last user, counts successful unlocks, and locks the system after repeated failed attempts.

## Project Description

This project is designed for secure and convenient door access control in homes, offices, or small facilities. It combines local keypad input with IoT connectivity to provide:

- Password-based door unlocking
- Multi-user access control
- Smartphone monitoring through Blynk
- Event logging for successful and failed access attempts
- Lockout protection after repeated wrong entries
- Audio feedback using a buzzer

## Features

- ESP32-based smart lock controller
- 4x4 keypad entry system
- Multi-user password authentication
- Admin and regular user modes
- Automatic lockout after 3 failed attempts for 30 seconds
- Input timeout for keypad entries
- Blynk dashboard integration for door status and user tracking
- Unlock count and last-user notifications
- Door state updates: Locked / Unlocked / Locked Out

## Hardware Components

- ESP32 development board
- 4x4 matrix keypad
- Passive buzzer
- Door lock mechanism or relay-controlled solenoid lock
- Wi-Fi network
- Smartphone with Blynk app (optional, for dashboard monitoring)
- Jumper wires and breadboard

## Pin Configuration

The system uses the following GPIO mapping in the code:

- Keypad rows: D2, D3, D4, D5
- Keypad columns: D6, D7, D8, D9
- Buzzer: D10

## Default Users and Passwords

The sketch currently includes the following user credentials:

- Alison: 2235
- User 2: 3355
- Admin: 5555

The keypad supports the following functions:

- E = clear current input
- F = submit / enter password

## Software Requirements

### Arduino IDE

Install the Arduino IDE and add the ESP32 board support.

### Required Libraries

Install these libraries in the Arduino IDE:

- WiFi
- WiFiClient
- BlynkSimpleEsp32
- Keypad

## Blynk Setup

This project uses Blynk for smartphone status monitoring.

1. Create a Blynk project.
2. Add widgets for status indicators if desired.
3. Update the following values in the sketch:
   - BLYNK_TEMPLATE_ID
   - BLYNK_TEMPLATE_NAME
   - BLYNK_AUTH_TOKEN
4. Ensure your ESP32 is connected to Wi-Fi and has internet access.

## Wi-Fi Configuration

Update the Wi-Fi credentials in the code before uploading:

```cpp
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";
```

## Upload and Run

1. Open the project file: `FinalYearProject.ino`
2. Connect the ESP32 to your computer.
3. Select the correct COM port and board.
4. Compile and upload the sketch.
5. Open the Serial Monitor to view status messages.
6. Enter the user password on the keypad and press `F` to unlock.

## System Behavior

- When a valid password is entered, the door unlocks for a defined period.
- After a successful unlock, the system updates Blynk status and logs the event.
- When a wrong password is entered, the buzzer emits an error tone.
- After three unsuccessful attempts, the system enters a 30-second lockout.
- During a lockout, the keypad is temporarily disabled.

## Example Workflow

1. Power on the ESP32.
2. The device connects to Wi-Fi and Blynk.
3. Enter a 4-digit password.
4. Press `F` to submit.
5. If valid, the door unlocks and the status updates on the Blynk dashboard.
6. After the lock period, the system returns to the locked state.

## Important Security Note

This project stores Wi-Fi credentials, Blynk authentication data, and user passwords directly in the source code. This is convenient for prototyping, but it is not recommended for production deployments. For a real-world installation, consider:

- Using secure credentials management
- Encrypting sensitive data
- Restricting physical access to the device
- Adding stronger authentication and logging
- Using a relay or motorized lock with proper power protection

## Project Structure

```text
SMART-IoT-BASED-DOOR-ACCESS-CONTROL-SYSTEM-WITH-SMARTPHONE-MONITORING/
├── FinalYearProject.ino
└── README.md
```

## License

This project is provided for educational and experimental purposes. Please use responsibly and ensure compliance with local laws and privacy requirements.

## Author

This project is a smart access control and monitoring system developed for an IoT-based door lock application.

## Future Improvements

- Add RFID or fingerprint authentication
- Support remote unlocking from the Blynk app
- Add door-open detection and timeout alerts
- Store access logs in EEPROM or cloud storage
- Implement stronger security for stored credentials

## Conclusion

This smart door lock system demonstrates how IoT and embedded systems can be combined to create a practical, user-friendly door access control solution. It is useful for learning about ESP32 programming, keypad-based input, Wi-Fi connectivity, and Blynk integration.
