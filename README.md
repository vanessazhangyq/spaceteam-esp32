# Spaceteam ESP32 Visual Enhancement Project

This project builds upon the raw **Spaceteam ESP32** game ([original code](https://github.com/ttseng/COMS3930-Fall2024/blob/main/Module%203/espaceteam.ino)) by enhancing the **visual experience** for players, making the game more interactive and engaging. Spaceteam is a cooperative multitasking game where players receive and execute commands to achieve a shared goal. We focused on adding three key **visual feedback features** to improve the user experience.

## New Features

### 1. **Green Background Flash for Successful Actions**

When an action is successfully completed (a `"DONE"` message is received), all players' screens briefly flash **green**. This provides a shared visual signal for success, fostering a sense of accomplishment and teamwork.

- **Why It’s Useful**: The green flash adds a celebratory and attention-grabbing moment to the game, emphasizing collaborative achievements.

### 2. **Visual Feedback for Button Presses**

When a player presses a button, the corresponding action text changes its color to **green**, giving immediate feedback that the button press has been registered.

- **Why It’s Useful**: This feedback confirms to the player that their button press has been recognized, reducing confusion during fast-paced gameplay.

### 3. **Action Shapes and Descriptions**

Each action is now paired with a unique **shape** alongside the textual command. Players must **describe both the shape and the command** to their teammates.

- **How It Works**: Shapes are randomly chosen from a predefined set of images, and commands are generated by combining random elements from arrays of nouns and objects [challenge: implementing the bitmap objects in the command is not yet fully functional].
    - The game uses the following shapes for actions:
        - Square | Circle | Triangle | Star | Diamond | Hexagon | Octagon | Cross | Arrow | Heart
- **Why It’s Useful**: Adding shapes introduces a new layer of complexity and fun! :p

## Setup Instructions

1. **Hardware Requirements**:
    - ESP32 boards (one for each player)
    - Cables connecting the board and laptop
2. **Software Requirements**:
    - Arduino IDE with ESP32 board definitions
3. **Game Installation**:
    - Clone this repository
    - Upload the code to each ESP32 device using the Arduino IDE

have fun playing!

![Giphy Animation](https://media.giphy.com/media/TshmwZLU276Sq65f7j/giphy.gif)

