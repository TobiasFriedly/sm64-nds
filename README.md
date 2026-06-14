# SM64 — Nintendo DS port + Multiplayer

A Modification of the dsi port from Hydr8gon https://github.com/Hydr8gon/sm64 
Making it work on all NDS Models. Also Added Multiplayer.

---

## 1. What you need

- A SM64 ROM that **you** dumped from your own cartridge.
  -
- **Docker** — 
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
Then open Docker Desktop once and wait until it's running.
(Or download Docker Desktop from https://www.docker.com/products/docker-desktop .)

Check it works:
```
docker --version
```

---

## 3. Build

Put your `baserom.<region>.z64` in this folder, then run these from this folder.
The commands below build the **USA** version. 


```
docker build -t sm64ds .
docker run --rm --mount type=bind,source="$(pwd)",destination=/sm64 sm64ds make VERSION=us -j8
```

