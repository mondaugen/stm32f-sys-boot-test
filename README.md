build using following command:
arm-none-eabi-gcc -DSTM32F429_439xx src/*.c src/*.s -Iinc -I${CMSIS_INCLUDES} -TSTM32F429ZI_FLASH.ld -o main.elf -ggdb3 -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork -mfloat-abi=hard -mfpu=fpv4-sp-d16 -dD

where CMSIS includes is a path to the Include folder in the CMSIS distribution
from ARM.
e.g.,
CMSIS_INCLUDES=../archives/CMSIS/Include

is for a folder two directories above the current one called "archives" that
contains the folder CMSIS, which can be obtained from ARM.
