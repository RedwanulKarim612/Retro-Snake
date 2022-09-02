# Retro-Snake
## CSE316 - Microcontroller Project

Retro snake is a hardware implementation of the game "Sanke", one of the first games we played on any digital device. We recreared the game using Microcontroller(ATMega32).

## Description
The game has two modes:
- Classic Mode
- Obstacle Mode

In both the modes, whenever the snakes eats a food one point is added to the score and the length of the snake increases. In classic mode, the game continues until the head of the snake collides with its body. In obstacle mode, there are 4 levels. When the snake eats two foods, the level increases except for the last level. The game ends when the snake's head collides with its body or an obstacle.

## Features
- The game is displayed on a grid(16x16) built with 4 8x8 bi-color LED matrices
- Two digital IR sensors are used to turn the snake's head left or right
- The score and the high score is displayed on a 16x2 I2C LCD display
- A buzzer is used to indicate a food was eaten

Youtube Link: https://youtu.be/ReesaAqyTmY

Created by:
- Yeasir Khandaker(1805105)
- Sk. Sabit Bin Mosaddek(1805106)
- Md. Redwanul Karim(1805111)

Under Supervision of:
- Md. Shariful Islam Bhuyan<br/>
  Assistant Professor, CSE, BUET
- Sukarna Barua<br/>
  Assistant Professor, CSE, BUET

- Md. Masum Mushfiq<br/>
  Lecturer, CSE, BUET
