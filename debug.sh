# 
#   Created by Matt Hartley on 22/07/2025.
#   Copyright 2025 GiantJelly. All rights reserved.
# 

gdb ./build/main.elf -ex "target remote localhost:9123" -ex "continue"
