---
sort: 5
---

# Building an Adaptor for a Wii Controller

## Supplies

- A microcontroller from the list in the [following guide](https://santroller.tangentmc.net/wiring_guides/general.html)

- A Tilt Switch, if you want tilt

  - The tool supports using basic digital tilt switches (somtimes called a mercury or ball tilt switch)
    - I recommend using two tilt sensors in series, as this can help with accidental activations
  - The tool also supports using analog tilt switches

  ```danger
  You do not want to get this type of sensor, as it does not work. If you do accidentally get one of these you *might* have luck just cutting the sensor from the top of the board and using it, but your mileage may vary doing that, as I have seen it work for some people and not for others.

  [![Basic](/assets/images/s-l500.png)](/assets/images/s-l500.png)
  ```

- A Wii extension breakout board or an extension cable, such as [![this](https://www.adafruit.com/product/4836)](https://www.adafruit.com/product/4836). You can also choose to cut the end of the extension and solder your own cables on as well if you perfer.
- If your wii extension breakout does not support 3.3v input, and you are using a 5v Pro Micro, you will need a 3.3v voltage regulator. The breakout listed above does however support either voltage so this is not required for that breakout.
- Some Wire
- A Soldering Iron

```danger
Be careful that you don't ever provide 5v power to the power pin of a Wii Extension, as they are not designed for this. The data pins however are tolerant of 5v, so you can hook these up directly to pins on your microcontroller.
```

## The finished product

[![Finished adaptor](/assets/images/adaptor.jpg)](/assets/images/adaptor.jpg)

## Steps

1.  Connect wires between the SDA and SCL pins on your breakout board / wii extension cable.
    Refer to the following image for the pinout of a Wii Extension connector.

    The Pi Pico lets you pick from various pins for the SDA and SCL pins. We provide recommended pins below, and this pinout is the same as the old Ardwiino firmware. If you need to use other pins, the options are provided below but the SDA and SCL pins must be from the same channel.

    [![pinout](/assets/images/wii.png)](/assets/images/wii.png) [![Finished adaptor](/assets/images/wii-ext.jpg)](/assets/images/wii-ext.jpg)

    ```danger
    If you are using a wii extension cable do NOT rely on the colours, the manufacturers are all over the place with this and the only way to validate them is to test each wire according to the above image. I've come across connectors wired with green as ground and black as VCC before, you just can't rely on the colours at all unfortunately.
    ```

    | Microcontroller               | SDA                              | SCL                              |
    | ----------------------------- | -------------------------------- | -------------------------------- |
    | Pi Pico (Recommended)         | GP18                             | GP19                             |
    | Pro Micro, Leonardo, Micro    | 2                                | 3                                |
    | Uno                           | A4                               | A5                               |
    | Mega                          | 20                               | 21                               |
    | Pi Pico (Advanced, Channel 0) | GP0, GP4, GP8, GP12, GP16, GP20  | GP1, GP5, GP9, GP13, GP17, GP21  |
    | Pi Pico (Advanced, Channel 1) | GP2, GP6, GP10, GP14, GP18, GP26 | GP3, GP7, GP11, GP15, GP19, GP27 |

2. Connect the vcc on the microcontroller to the vcc on the breakout
   - If you are using a 5v Pro Micro, and your breakout does not support 5v input, then you will need to hook up VCC from the microcontroller to a 3.3v regulator, and then hook up the output of the regulator to the breakout
   - If you are using the microcontroller uno, use the 3.3v pin on your microcontroller as VCC
   - If you are using the breakout linked ![above](https://www.adafruit.com/product/4836), the `vin` pin is used for both 3.3v input and 5v input. The 3v pin is actually an output and is not needed for this project.
3. Connect the gnd pin on the wii breakout / extension cable to the gnd on your microcontroller.

Now that you have wired your adapter, go [configure it](https://santroller.tangentmc.net/tool/using.html).
