# SM64 — Nintendo DS port + Multiplayer 

A Modification of the DSi port from Hydr8gon: https://github.com/Hydr8gon/sm64 
Making it work on all NDS Models. Also added Multiplayer.

---

## 1. What you need

- A SM64 ROM that **you** dumped from your own cartridge.
  -
- **Docker**
---

## 2. Install Docker 

**macOS**
```
brew install --cask docker-desktop
```

(Or just download Docker Desktop
from https://www.docker.com/products/docker-desktop .)

**Windows** 
```
winget install -e --id Docker.DockerDesktop
```
(Or download Docker Desktop from https://www.docker.com/products/docker-desktop .)

Check it works:
```
docker --version
```

---

## 3. Build

Put your `baserom.<region>.z64` in this folder, then run these from this folder.
The commands below build the **USA** version. 

On Windows you can also use the build_docker.bat file.

```
docker build -t sm64ds .
docker run --rm --mount type=bind,source="$(pwd)",destination=/sm64 sm64ds make VERSION=us -j8
```

If the Build fails, make sure the Docker App is running in the background by opening it and try again.
