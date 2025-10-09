Set up peripherals:
1. Connect the 2 MPU6050s on the same VCC and GND bus, with SCL to GPIO pin 0 and SDA to GPIO pin 1.
2. Connect the AD0 pin from one of the sensors to 3.3V, and the other one to GND.
3. Connect DTEK-V VGA output to a compatible monitor.

To compile and run the code:
1. Connect the DTEK-V board via USB to the VM, then enter the terminal and run "jtagd --foreground".
2. Open a new terminal tab, navigate to the folder "src", then run "make".
3. Run "dtekv-run main.bin" to start the program.
