import os

# toolchains options
ARCH        ='arm'
CPU         ='beken'
CROSS_TOOL  ='gcc'

if os.getenv('RTT_ROOT'):
    RTT_ROOT = os.getenv('RTT_ROOT')
else:
    RTT_ROOT = os.path.join(os.path.normpath(os.getcwd()), 'rt-thread')

print 'RTT_ROOT is: %s' %(RTT_ROOT)

if os.getenv('RTT_CC'):
    CROSS_TOOL = os.getenv('RTT_CC')

if  CROSS_TOOL == 'gcc':
    PLATFORM    = 'gcc'
    EXEC_PATH   = r'C:\Developer\Tools\env\tools\gnu_gcc\arm_gcc\mingw\bin'
else:
    print 'Please make sure your toolchains is GNU GCC!'
    exit(0)

if os.getenv('RTT_EXEC_PATH'):
    EXEC_PATH = os.getenv('RTT_EXEC_PATH')

BUILD = 'release'
# BUILD = 'debug'

if PLATFORM == 'gcc':
    # toolchains
    PREFIX  = 'arm-none-eabi-'
    CC      = PREFIX + 'gcc'
    CXX     = PREFIX + 'g++'
    AS      = PREFIX + 'gcc'
    AR      = PREFIX + 'ar'
    LINK    = PREFIX + 'g++'
    TARGET_EXT = 'elf'
    SIZE    = PREFIX + 'size'
    OBJDUMP = PREFIX + 'objdump'
    OBJCPY  = PREFIX + 'objcopy'

    DEVICE  = ' -mcpu=arm968e-s -mthumb-interwork -mthumb -ffunction-sections -fdata-sections'
    CFLAGS  = DEVICE
    AFLAGS  = ' -c' + DEVICE + ' -x assembler-with-cpp'
    LFLAGS  = DEVICE + ' -nostartfiles -Wl,--gc-sections,-Map=rtthread.map,-cref,-u,system_vectors -T link.lds'
    CPATH   = ''
    LPATH   = ''

    if BUILD == 'debug':
        CFLAGS += ' -O0 -gdwarf-2'
        AFLAGS += ' -gdwarf-2'
    else:
        CFLAGS += ' -Os -std=c99'

    CXXFLAGS = CFLAGS

DUMP_ACTION = OBJDUMP + ' -D -S $TARGET > rtt.asm\n'
POST_ACTION = OBJCPY + ' -O binary $TARGET rtthread.bin\n' + SIZE + ' $TARGET \n'

# rt_ota_packaging_tool
# ..\tools\rt_ota_packaging_tool\rt_ota_packaging_tool_cli.exe  -f rtthread.bin -v 1957 -p app -o rtthread.rbl -c gzip -s aes -i 0123456789ABCDEF -k 0123456789ABCDEF0123456789ABCDEF
import platform

# platform.system() ==> Windows Linux 
# platform.architecture() ==> ('64bit', 'ELF') ('64bit', 'WindowsPE')

sys = platform.system()
if sys == 'Windows':
    RBL_CMD = 'rt_ota_packaging_tool_cli.exe'
else:
    bit, exe = platform.architecture()
    if bit == '64bit':
        RBL_CMD = 'rt_ota_packaging_tool_cli-x64'
    else:
        RBL_CMD = 'rt_ota_packaging_tool_cli-x86'

RBL_CMD += ' -f rtthread.bin'

import datetime
version = datetime.datetime.now().strftime('%Y%m%d_%H%M%S') # 20191219_124927
RBL_CMD += ' -v ' + version

RBL_CMD += ' -p app'
RBL_CMD += ' -o rtthread.rbl'
RBL_CMD += ' -c gzip'

RBL_CMD += ' -s aes -i ' + '0123456789ABCDEF' + ' -k ' + '0123456789ABCDEF0123456789ABCDEF'

POST_ACTION += 'tool/rt_ota_packaging_tool/' + RBL_CMD