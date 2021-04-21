<p align="center">
  <img src="https://raw.githubusercontent.com/linuxmuster/linuxmuster-linbo-gui/master/resources/images/linbo_logo_small.png" alt="Linbo logo" width="70%" />
</p>

---

<p align="center">
  <a href="https://github.com/linuxmuster/linuxmuster-linbo-gui/releases/latest"><img src="https://img.shields.io/github/v/release/linuxmuster/linuxmuster-linbo-gui?logo=github&logoColor=white" alt="GitHub release"/></a>
  <a href="https://github.com/linuxmuster/linuxmuster-linbo-gui/releases/latest"><img src="https://github.com/linuxmuster/linuxmuster-linbo-gui/workflows/Build%20Release/badge.svg" /></a>
  <a href="https://crowdin.com/project/linuxmusternet"><img src="https://github.com/linuxmuster/linuxmuster-linbo-gui/workflows/Sync%20translations%20with%20Crowdin/badge.svg" /></a>
  <a href="https://www.gnu.org/licenses/agpl-3.0" ><img src="https://img.shields.io/badge/License-AGPL%20v3-blue.svg" /></a>
  <a href="https://ask.linuxmuster.net"><img src="https://img.shields.io/discourse/users?logo=discourse&logoColor=white&server=https%3A%2F%2Fask.linuxmuster.net" alt="Community Forum"/></a>
  <a href="https://crowdin.com/project/linuxmusternet"><img src="https://badges.crowdin.net/linuxmusternet/localized.svg" /></a>
</p>

This is the GUI of [Linbo](https://github.com/linuxmuster/linuxmuster-linbo) based on Qt5!

# Maintainance Details
    
Linuxmuster.net official | ![#c5f015](https://via.placeholder.com/15/f03c15/000000?text=+)  YES
:---: | :---: 
[Community support](https://ask.linuxmuster.net) | ![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+)  YES**
Actively developed | ![#c5f015](https://via.placeholder.com/15/c5f015/000000?text=+)  YES
Maintainer organisation |  Netzint GmbH  
Primary maintainer | dorian@itsblue.de  


** The linuxmuster community consits of people who are nice and happy to help. They are not directly involed in the development though, and might not be able to help in any case.

# Releases
The latest release can be found [here](https://github.com/linuxmuster/linuxmuster-linbo-gui/releases/latest)  
The attached file `linuxmuster-linbo-gui7_*.deb` is the Package which you can install on your server.
Please note: the prebuilt packages do not contain a 32-bit version.

# Building yourself
For your convenience there is a script to build the package: [build.sh](./build.sh)  
You have to invoke it in the build root.  
If you want to build the 32-bit version, you will have to insert `32` in the following places:
- In the file `/build.sh`: Line 64 change `(64)` to `(64 32)`
- In the file `/debian/mkarchives.sh`: Line 24 change `for bits in 64; do` to `for bits in 32 64; do`
Please note: It will take a long time to build the first time you do it.  

# Screenshots
This is what the new GUI looks like so far:  
Choosing an OS (minimalistic Layout):  
![choose](/screenshots/choose.gif)  
Choosing an OS (legacy layout):  
![legacy](/screenshots/legacy.gif)  
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
- The Linux Mint was taken from [Icon Fonts](http://www.onlinewebfonts.com/icon) and is licensed by CC BY 3.0
- The Debian icon was taken from [debian.org](https://www.debian.org/logos/)
- The Windows 7 icon was taken from [Wikipedia](https://commons.wikimedia.org/wiki/File:Windows_logo_-_2002%E2%80%932012_(Black).svg)
- All other icons were taken from [fontawesome.com](https://fontawesome.com/license) and are licensed under the [Creative Commons Attribution 4.0 International license](https://fontawesome.com/license).

# Creating a release
The following steps have to be followed to create a release:
1. Update the changelog file
2. Update the version code in the .pro file
3. commit all changes
4. Do a git push: `git push`
5. Wait for translations sync pipeline
  - if it pushes new changes, do a git pull: `git pull`
6. Create a git tag in the format v+VERSION (eg. v7.0.0): `git tag vVERSION`
7. Push tags: `git push --tags`

In case of a mistake, the tag can be deleted:
1. Locally: `git tag -d vVERSION`
2. Remotely: `git push --delete origin vVERSION`
