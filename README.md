Evolve
======

Vectorizes images throught evolution of a DNA sequence.

![2014-05-10 19_21_40-evolution vectorizer](https://cloud.githubusercontent.com/assets/5155966/2936626/9e67c000-d867-11e3-93e9-4d9c5b21e132.jpg)

![2014-05-10 18_29_45-edit release tux3_evolve](https://cloud.githubusercontent.com/assets/5155966/2936525/57be55bc-d860-11e3-9dd5-84735b18b517.jpg)

Features
========
- Import of PNG, GIF, JPG and BMP
- Export to PNG, JPG, and SVG
- Import and export the DNA
- Resume the evolution from a DNA file at any time
- Can Focus on a part of the image to add a lot more details
- Multiplatform (Qt)
- Multithreaded

Compiling
=========

This is a standard Qt application, the easiest way to compile it is to download the Qt SKD, open the project with Qt Creator and click Build.
The Qt SDK is available <a href="http://qt-project.org/downloads">here</a>

On linux, you can also compile it with the following commands:<br/>
<code>
 sudo apt-get install qt5-qmake qt5-default libqt5svg5 libqt5svg5-dev git<br/>
 git clone https://github.com/infoburp/Evolve<br/>
 cd Evolve<br/>
 ./Evolve<br/>
</code>
