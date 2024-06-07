# The Eye Saver

I often forget when I have to switch out my contact lenses so I built an arduino project to help me, and maybe you too.

The included 650mAh 9V battery lasts for only 4.5 hours. I probably have something wrong with the powerDown() function or I should have the RTC only running off the backup battery but I am done with this project for now. Still would like to know though if you figure out a way to make it more efficient :)

## some stuff I learned:

1. I tried to use the RFID module to wake up the arduino but the IRQ does not seem to work: https://arduino.stackexchange.com/a/76285.

2. Initially I used watchdog for waking up the arduino but it is limited to 8 seconds sleep times which does not suit my purposes.

3. I tried to use my 7 segment displays and shift registers initially to display days remaining but they all broke :(

4. Arduino uno r3 has only 2 pins for interrupts (2, 3) so I had to compact down my design to just use 2 buttons.

## Power Consumption Calcs:
1. **Calculate the average current draw**:
   - $ \text{Current} (I) = \frac{\text{Capacity (mAh)}}{\text{Time (hours)}} $
   - $ I = \frac{650 \text{mAh}}{4.5 \text{hours}} \approx 144.44 \text{mA} $

2. **Calculate the power consumption in watts**:
   - $ Power (P) = Voltage (V) × Current (I) $
   - $ P = 9 \text{V} \times 144.44 \text{mA} $
   - Converting milliamps to amps (1mA = 0.001A):
   - $ P = 9 \text{V} \times 0.14444 \text{A} \approx 1.3 \text{W} $

3. **Calculate daily energy consumption**:
   - $ \text{Daily energy consumption} = Power (W) × Time (hours) $
   - $ \text{Daily energy consumption} = 1.3 \text{W} \times 24 \text{hours} = 31.2 \text{Wh} $
   - Convert watt-hours to kilowatt-hours (1 kWh = 1000 Wh)
   - $ \text{Daily energy consumption} = \frac{31.2 \text{Wh}}{1000} = 0.0312 \text{kWh} $

4. **Calculate monthly energy consumption**:
   - Assume there are 30 days in a month.
   - Monthly energy consumption = Daily energy consumption × Number of days in a month
   - $ \text{Monthly energy consumption} = 0.0312 \text{kWh/day} \times 30 \text{days} = 0.936 \text{kWh} $

5. **Calculate the monthly cost**:
   - Cost = Monthly energy consumption × Cost per kWh
   - $ \text{Cost} = 0.936 \text{kWh} \times \$0.192/\text{kWh} = \$0.179712 $

## Summary:

So the cost is bout \$0.18 (rounded to the nearest cent) a month.