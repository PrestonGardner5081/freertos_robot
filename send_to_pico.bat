cmake --build c:/Users/ptg10/Documents/all_projects/pico_car/freertos_robot/build --config Debug --target all 

@REM if having issues, make sure to install winusb in zadig instead of libusb
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -s "C:/Users/ptg10/Documents/all_projects/pico_car/openocd-0.12.0-rc2+dev-g9d92577-202211162039-x86/scripts" -c "program C:/Users/ptg10/Documents/all_projects/pico_car/freertos_robot/build/RTOS_RC/rtos_rc.elf verify reset exit"

@REM putty -load "pico_bt"