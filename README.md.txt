 Speech-to-Braille Converter using STM32 and UART
The idea is make an affordable Speech-to-Braille conversion device to facilitate real-time communication for deafblind individuals. The innovation lies in converting spoken language into tactile Braille output using voice recognition and refreshable Braille cells.
The current working prototype uses an STM32 Nucleo board to control LEDs representing Braille dots and a laptop for speech recognition and text processing. 

 Project Overview

- Voice Input (via microphone or terminal)
- Speech-to-Text using Python
- Text sent via UART to STM32 (NUCLEOBOARD)
- STM32 maps characters to Braille dot patterns
- LEDs blink to simulate Braille dots for each letter

 Technologies Used

| Type            | Tool/Platform              |
|-----------------|----------------------------|
| Microcontroller | STM32 (NUCLEOBOARD)        |
| IDE             | STM32CubeIDE               |
| Programming     | C (STM32 HAL) + Python     |
| Communication   | UART (NUCLEOBOARD in-built)|
| Display         | 24 LEDs (4 Braille cells)  |
| OS              | Windows 11                 |



How It Works

1.  Python script converts speech to text using `speech_recognition`
2.  Sends that text over UART to STM32.
3.  STM32 extracts 4 characters at a time.
4.  For each character:
   - Looks up Braille binary pattern lookup table.
   - Lights up LEDs for 2 second.
   - Moves to next characters and display it until all the characters are displayed.

 Example Output

Say: `help`  
→ LEDs show:  
- h → 110010  
- e → 100010  
- l → 111000  
- p → 111100


 Demo Images and video

>  Check the `images/` folder for:
- Braille chart
- STM32 GPIO configuration
- LED Prototype 




 Future Improvements

- Replace LEDs with Braille cells for actual tactile reading 
- Add the speed adjustment feature to match the reading pace of the user



`STM32`, `Braille`, `Voice to Text`, `Speech Recognition`, `UART`, `Assistive Tech`, `ECE`, `Python`, `HAL`




