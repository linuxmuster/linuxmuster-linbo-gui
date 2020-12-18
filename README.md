# Linuxmuster Linbo GUI
A new GUI for [Linbo](https://github.com/linuxmuster/linuxmuster-linbo) based on Qt5!

# Screenshots
This is what the new GUI looks like so far:
Choosing an OS:  
![choose](/screenshots/choose.gif)  
Performing a long action (eg. syncing OS):  
![long](/screenshots/long.gif)  
Starting an OS:  
![start](/screenshots/start.gif)  
Autostarting an OS:  
![autostart](/screenshots/autostart.gif)  
An error occurred:  
![error](/screenshots/error.gif)  

# Icons
All of the SVG icons in this Gui were taken from one of the following sources and slighlty modified:
- The Linux Mint is from [Icon Fonts](http://www.onlinewebfonts.com/icon) and is licensed by CC BY 3.0
- The Debian icon was taken from [debian.org](https://www.debian.org/logos/)
- The Windows 7 icon was taken from [Wikipedia](https://commons.wikimedia.org/wiki/File:Windows_logo_-_2002%E2%80%932012_(Black).svg)
- All other icons ware taken from [fontawesome.com](https://fontawesome.com/license) and are licensed under the [Creative Commons Attribution 4.0 International license](https://fontawesome.com/license).

# Debian Packaging
For your convenience there is a script to build the package: [debian/mkdeb.sh](./debian/mkdeb.sh)  
You have to invoke it in the build root.
