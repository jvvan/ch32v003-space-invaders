# Space Invaders on ch32v003

This project is a clone of the classic space invaders game made for the [BalCCon2k24 Badge (MC-0o00)](https://ch405-labs.com/mc-0o00/).

## Dependencies

This project depends on following libraries:

- [ch32v003-joypad](https://gitlab.com/ch405labs/ch32v003-joypad): Joypad driver for input handling.
- [ch32v003-st7735-driver](https://gitlab.com/ch405labs/ch32v003-st7735-driver): ST7735 display driver for rendering graphics.
- [ch32v003fun](https://github.com/cnlohr/ch32v003fun): PlatformIO framework for ch32v003

## Building

> [!TIP]
> You can download the already compiled firmware [releases](https://github.com/jvvan/ch32v003-space-invaders/releases) page.


You can use [PlatformIO](https://platformio.org/) to build this project. Just clone this repository and run `pio run` in the project directory or use the VSCode extension.


## Flashing

You can use [PlatformIO](https://platformio.org/) or [wlink](https://github.com/ch32-rs/wlink/) to flash the firmware to the badge.

### PlatformIO
```
pio run -t upload
```

### wlink
```
wlink flash .pio/build/ch32v003f4p6_evt_r0/firmware.bin
```

## Controls

The badge has 6 buttons, 3 on each side.

### Main Menu

Use the top and middle buttons to navigate the menu and the bottom button to select.

### In-game

Use the middle button to move in that direction and the top buttons to shoot.


## Special Thanks

- [BalCCon2k24](https://2k24.balccon.org/)
- [CH405 Labs](https://ch405-labs.com/)
- [ch32v003fun](https://github.com/cnlohr/ch32v003fun)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Images

![image1](https://github.com/user-attachments/assets/35e9f174-ab18-4464-9068-38515e3eacb7)
![image2](https://github.com/user-attachments/assets/063a95b2-9bf0-41e1-b3e4-079e21eda4d3)
![image3](https://github.com/user-attachments/assets/1ca8540b-19ca-450f-855c-bc837d248a08)
![image4](https://github.com/user-attachments/assets/eb62fdef-250c-47dd-8867-1f30b5c87d5b)

