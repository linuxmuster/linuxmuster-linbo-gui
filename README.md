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

This is the GUI of [Linbo](https://github.com/linuxmuster/linuxmuster-linbo7) based on Qt6!

### Maintainance Details
    
Linuxmuster.net official | ✅ YES
:---: | :---: 
[Community support](https://ask.linuxmuster.net) | ✅ YES**
Actively developed | ✅ YES
Maintainer organisation |  Linuxmuster.net e.V. 
Primary maintainer | dorian@linuxmuster.net


** The linuxmuster community consits of people who are nice and happy to help. They are not directly involved in the development though, and might not be able to help in any case.

### Releases
The latest release can be found [here](https://github.com/linuxmuster/linuxmuster-linbo-gui/releases/latest)  
The attached file `linuxmuster-linbo-gui7_*.deb` is the Package which you can install on your server.
Please note: the prebuilt packages do not contain a 32-bit version.

### Compatibility with Linbo
If you install an incompatible combination of the gui and linbo itself, the gui will not function!

- `>= v7.1.0` is only compatible with Linbo `>= v4.1.0`
- `< v7.1.0` is only compatible with Linbo `< v4.1.0`

### Building yourself
For your convenience there is a script to build the package: [build.sh](./build.sh)  
You have to invoke it in the build root.  

### Screenshots
Some screenshots can be found here: https://github.com/linuxmuster/linuxmuster-linbo-gui/tree/master/.github/media  
Please note that they might not be up to date though.
  
### Icons
All of the SVG icons in this Gui were taken from one of the following sources and slighlty modified:
- The Linux Mint was taken from [Icon Fonts](http://www.onlinewebfonts.com/icon) and is licensed by CC BY 3.0
- The Debian icon was taken from [debian.org](https://www.debian.org/logos/)
- The Windows 7 icon was taken from [Wikipedia](https://commons.wikimedia.org/wiki/File:Windows_logo_-_2002%E2%80%932012_(Black).svg)
- All other icons were taken from [fontawesome.com](https://fontawesome.com/license) and are licensed under the [Creative Commons Attribution 4.0 International license](https://fontawesome.com/license).

### Creating a release
The following steps have to be followed to create a release:
- Update the changelog file
- Update the version code in the CMakeLists.txt file
- commit all changes
- Do a git push: `git push`
- Create a git tag in the format v+VERSION (eg. v7.0.0): `git tag vVERSION`
- Push tags: `git push --tags`

In case of a mistake, the tag can be deleted:
- 1. Locally: `git tag -d vVERSION`
- 2. Remotely: `git push --delete origin vVERSION`

### Version schema:
- Genreal: `major.minor.patch` ([semver](https://semver.org/))
- Prereleases (release candidates) must end with `~XX` where `XX` is the number of the Prerelease
  - So, before version `7.1.1` is released, there may be versions `7.1.1~01`, `7.1.1~02`, and so on
- Releases don't have a suffix
  - So, once version `7.1.1` is ready, it is published as `7.1.1`
- This concept ensures that stable releases are always evaluated as a higher version Number than perreleases.
