Requirements:

http://www.xmission.com/~nate/glut/glut-3.7.6-bin.zip
GLUT Libraries and header files are glut32.lib and glut.h


Compile(Linux):```

g++ -o {name_of_exe} game.cpp -lglut -lGLU -lGL


Compile(Windows):

Download the VS project folder and setup the GLUT header , lib and dll files on your system :

```
glut.h file in --> C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\VS\include\gl

glut32.lib in --> C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\VS\lib\x86

glut32.dll in --> C:\Windows\SysWOW64  and  C:\Windows\System32
```


GL Documentation :

https://www.opengl.org/resources/libraries/glut/spec3/node10.html



