How to compile?
export p_home=/D/MyStudy/GitHub/LuaSimple/LuaTestLinux
gcc -I$p_home/include -L$p_home/lib main.c -llua -o main -lm
